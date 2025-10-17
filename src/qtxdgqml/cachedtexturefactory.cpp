/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * LXQt - a lightweight, Qt based, desktop toolset
 * https://lxqt.org
 *
 * Copyright: 2025 LXQt team
 * Authors:
 *   LXQt team
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "cachedtexturefactory.h"
#include <QSGTexture>
#include <QQuickWindow>
#include <QMutexLocker>
#include <QDebug>

// Static member initialization
QHash<QQuickWindow*, QHash<QString, CachedTextureFactory::TextureEntry>>
    CachedTextureFactory::s_perWindowCache;

QMutex CachedTextureFactory::s_cacheMutex;
int CachedTextureFactory::s_textureCreateCount = 0;
int CachedTextureFactory::s_textureReuseCount = 0;

CachedTextureFactory::CachedTextureFactory(const QImage &image,
                                           const QString &cacheKey,
                                           bool enableAtlas)
    : m_image(image)
    , m_cacheKey(cacheKey)
    , m_enableAtlas(enableAtlas)
{
}

CachedTextureFactory::~CachedTextureFactory()
{
    // TextureFactory 不负责删除 QSGTexture
    // QSGTexture 由 Scene Graph 管理生命周期
}

QSGTexture* CachedTextureFactory::createTexture(QQuickWindow *window) const
{
    if (!window) {
        qWarning() << "CachedTextureFactory::createTexture: null window";
        return nullptr;
    }

    QMutexLocker locker(&s_cacheMutex);

    // 获取此窗口的缓存
    QHash<QString, TextureEntry> &windowCache = s_perWindowCache[window];

    // 检查缓存是否命中
    auto it = windowCache.find(m_cacheKey);
    if (it != windowCache.end()) {
        TextureEntry &entry = it.value();

        // 验证纹理仍然有效 (QPointer自动检测对象是否存在)
        if (entry.texture) {
            // 缓存命中！更新统计和时间戳
            entry.lastUsed = QDateTime::currentDateTime();
            entry.reuseCount++;
            s_textureReuseCount++;

            qDebug() << "GPU cache HIT:" << m_cacheKey
                     << "reuse_count=" << entry.reuseCount;

            return entry.texture;
        } else {
            // 纹理已失效，移除缓存项
            qDebug() << "GPU cache STALE:" << m_cacheKey;
            windowCache.erase(it);
        }
    }

    // 缓存未命中，创建新纹理
    qDebug() << "GPU cache MISS:" << m_cacheKey;

    // 检查缓存大小，必要时淘汰
    qint64 newTextureBytes = estimateTextureBytes(m_image.size());
    qint64 currentBytes = 0;
    for (const auto &entry : windowCache) {
        currentBytes += entry.bytes;
    }

    qint64 maxBytes = MAX_GPU_CACHE_MB * 1024 * 1024;
    if (currentBytes + newTextureBytes > maxBytes) {
        qint64 bytesToFree = (currentBytes + newTextureBytes) - maxBytes;
        qDebug() << "GPU cache full, evicting"
                 << (bytesToFree / 1024 / 1024) << "MB";
        evictLRU(window, bytesToFree);
    }

    // 创建纹理
    QQuickWindow::CreateTextureOptions options = QQuickWindow::TextureHasAlphaChannel;

    // 小纹理启用 Atlas（如果允许）
    if (m_enableAtlas && m_image.width() <= 64 && m_image.height() <= 64) {
        options |= QQuickWindow::TextureCanUseAtlas;
        qDebug() << "Atlas enabled for" << m_cacheKey;
    }

    QSGTexture *newTexture = window->createTextureFromImage(m_image, options);

    if (!newTexture) {
        qWarning() << "Failed to create texture for" << m_cacheKey;
        return nullptr;
    }

    // 存入缓存
    TextureEntry entry;
    entry.texture = newTexture;
    entry.bytes = newTextureBytes;
    entry.lastUsed = QDateTime::currentDateTime();
    entry.reuseCount = 0;

    windowCache[m_cacheKey] = entry;
    s_textureCreateCount++;

    qDebug() << "GPU texture created:" << m_cacheKey
             << "size=" << m_image.size()
             << "bytes=" << (newTextureBytes / 1024) << "KB"
             << "atlas=" << (options & QQuickWindow::TextureCanUseAtlas);

    return newTexture;
}

QSize CachedTextureFactory::textureSize() const
{
    return m_image.size();
}

int CachedTextureFactory::textureByteCount() const
{
    return estimateTextureBytes(m_image.size());
}

void CachedTextureFactory::clearGpuCache(QQuickWindow *window)
{
    QMutexLocker locker(&s_cacheMutex);

    if (window) {
        // 清除特定窗口的缓存
        s_perWindowCache.remove(window);
        qDebug() << "GPU cache cleared for window" << window;
    } else {
        // 清除所有窗口的缓存
        s_perWindowCache.clear();
        qDebug() << "GPU cache cleared for all windows";
    }

    // 重置统计
    s_textureCreateCount = 0;
    s_textureReuseCount = 0;
}

int CachedTextureFactory::gpuCacheCount()
{
    QMutexLocker locker(&s_cacheMutex);

    int count = 0;
    for (const auto &windowCache : s_perWindowCache) {
        count += windowCache.size();
    }

    return count;
}

qint64 CachedTextureFactory::gpuCacheBytes()
{
    QMutexLocker locker(&s_cacheMutex);

    qint64 totalBytes = 0;
    for (const auto &windowCache : s_perWindowCache) {
        for (const auto &entry : windowCache) {
            totalBytes += entry.bytes;
        }
    }

    return totalBytes;
}

double CachedTextureFactory::textureReuseRate()
{
    QMutexLocker locker(&s_cacheMutex);

    int total = s_textureCreateCount + s_textureReuseCount;
    if (total == 0) {
        return 0.0;
    }

    return static_cast<double>(s_textureReuseCount) / total;
}

void CachedTextureFactory::resetStats()
{
    QMutexLocker locker(&s_cacheMutex);
    s_textureCreateCount = 0;
    s_textureReuseCount = 0;
}

void CachedTextureFactory::evictLRU(QQuickWindow *window, qint64 bytesToFree)
{
    // 假设 s_cacheMutex 已加锁

    QHash<QString, TextureEntry> &windowCache = s_perWindowCache[window];

    // 收集所有条目并按 lastUsed 排序
    QList<QString> keys;
    for (auto it = windowCache.begin(); it != windowCache.end(); ++it) {
        keys.append(it.key());
    }

    // 按最后使用时间排序（最旧的在前）
    std::sort(keys.begin(), keys.end(), [&windowCache](const QString &a, const QString &b) {
        return windowCache[a].lastUsed < windowCache[b].lastUsed;
    });

    // 淘汰最旧的纹理直到释放足够空间
    qint64 freedBytes = 0;
    int evictedCount = 0;

    for (const QString &key : keys) {
        if (freedBytes >= bytesToFree) {
            break;
        }

        const TextureEntry &entry = windowCache[key];
        freedBytes += entry.bytes;
        evictedCount++;

        // 注意：不删除 QSGTexture，它由 Scene Graph 管理
        // QPointer 会自动检测到纹理被删除
        qDebug() << "Evicted GPU texture:" << key
                 << "freed=" << (entry.bytes / 1024) << "KB";

        windowCache.remove(key);
    }

    qDebug() << "LRU eviction complete:"
             << "evicted=" << evictedCount
             << "freed=" << (freedBytes / 1024 / 1024) << "MB";
}

qint64 CachedTextureFactory::estimateTextureBytes(const QSize &size)
{
    // 假设 RGBA8 格式（每像素 4 字节）
    return static_cast<qint64>(size.width()) * size.height() * 4;
}

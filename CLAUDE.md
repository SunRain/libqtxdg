"# libqtxdg - Qt XDG 标准实现库

> 最后更新：2025-11-08 19:47:55

## 项目概述

libqtxdg 是 freedesktop.org XDG 规范的 Qt 实现库，由 LXQt 项目维护。该库提供了完整的 XDG 标准支持，包括桌面文件、图标加载、菜单系统、MIME 类型处理等功能。

**版本信息：** 4.2.0
**Qt 版本要求：** Qt 6.6.0+
**构建系统：** CMake 3.18.0+
**许可证：** LGPL 2.1+

## 项目愿景

为 Qt 应用程序提供标准化的跨桌面环境集成能力，实现：
- 统一的目录管理规范
- 标准化的应用程序描述格式
- 一致的图标主题机制
- 可靠的文件类型关联系统

## 架构总览

```mermaid
graph TD
    A[\"(根) libqtxdg\"] --> B[\"src\"];
    B --> C[\"qtxdg<br/>核心库\"];
    B --> D[\"xdgiconloader<br/>图标加载器\"];
    B --> E[\"qtxdgqml<br/>QML插件\"];
    A --> F[\"examples<br/>示例程序\"];
    A --> G[\"test<br/>测试套件\"];
    A --> H[\"util<br/>开发工具\"];
    A --> I[\"cmake<br/>构建模块\"];
    A --> J[\"config<br/>配置文件\"];

    click C \"./src/qtxdg/CLAUDE.md\" \"查看核心库文档\"
    click D \"./src/xdgiconloader/CLAUDE.md\" \"查看图标加载器文档\"
    click E \"./src/qtxdgqml/CLAUDE.md\" \"查看QML插件文档\"
    click F \"./examples/CLAUDE.md\" \"查看示例文档\"

    style A fill:#2c3e50,color:#fff
    style C fill:#4a90e2,color:#fff
    style D fill:#50c878,color:#fff
    style E fill:#9b59b6,color:#fff
```

## 核心特性

- ✅ XDG 桌面文件解析与处理
- ✅ XDG 图标主题加载（支持 GTK+ 缓存加速）
- ✅ XDG 菜单规范实现
- ✅ MIME 类型与默认应用管理
- ✅ XDG 目录规范支持
- ✅ 自动启动管理
- ✅ QML 插件支持（Qt6）

## 模块索引

| 模块 | 路径 | 类型 | 职责 | 文件数 |
|------|------|------|------|--------|
| **qt6xdg** | `src/qtxdg/` | 核心库 | XDG 规范核心实现 | 42 |
| **qt6xdgiconloader** | `src/xdgiconloader/` | 图标加载器 | XDG 图标主题加载与缓存 | 4 |
| **qt6xdgqml** | `src/qtxdgqml/` | QML 插件 | QML 应用 XDG 功能封装 | 20 |
| **examples** | `examples/` | 示例程序 | C++ 和 QML 使用示例 | 8 |
| **test** | `test/` | 测试套件 | 单元测试与集成测试 | 5 |
| **util** | `util/` | 开发工具 | 开发辅助工具 | 3 |

## 运行与开发

### 快速开始

```bash
# 克隆仓库
git clone https://github.com/lxqt/libqtxdg.git
cd libqtxdg

# 创建构建目录
mkdir build && cd build

# 配置构建（启用示例和QML插件）
cmake .. -DBUILD_EXAMPLES=ON -DBUILD_QML_PLUGIN=ON

# 编译
make -j$(nproc)

# 运行示例
./examples/use-qtxdg/use-qtxdg
QML_IMPORT_PATH=./qml ./examples/qml-example/qtxdg-qml-example
```

### 构建选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `BUILD_TESTS` | OFF | 构建测试套件 |
| `BUILD_DEV_UTILS` | OFF | 构建开发工具 |
| `BUILD_EXAMPLES` | ON | 构建示例程序 |
| `BUILD_QML_PLUGIN` | ON | 构建 QML 插件 |
| `QTXDG_INSTALL_DEFAPPS_CONFIG` | ON | 安装默认应用配置 |

### 依赖要求

**必需依赖：**
- Qt 6.6.0+ (Widgets, Xml, DBus, Gui, Svg)
- CMake 3.18.0+
- GLib 2.41.0+ (GIO)
- lxqt-build-tools 2.0.0+

**可选依赖：**
- Qt6::Qml, Qt6::Quick (QML 插件)
- Qt6::Test (测试套件)

## 测试策略

### 单元测试
```bash
cmake .. -DBUILD_TESTS=ON
make
ctest
```

**测试覆盖:**
- `tst_xdgdirs` - 目录管理功能
- `tst_xdgdesktopfile` - 桌面文件处理
- `qtxdg_test` - 综合测试

### 集成测试
- QML 插件功能测试
- 图标加载器性能测试
- 跨平台兼容性验证

### QML 测试
```bash
# 运行 QML 示例验证所有 wrapper
./examples/qml-example/qtxdg-qml-example
```

## 编码规范

遵循 Qt6 C++17 编码风格：

1. **代码风格**
   - 使用 C++17 特性
   - 遵循 Qt 命名约定
   - 使用 Qt 容器和智能指针
   - 启用 `QT_NO_KEYWORDS` 避免宏冲突

2. **文件组织**
   - 头文件使用 `.h` 扩展名
   - 源文件使用 `.cpp` 扩展名
   - 私有实现使用 `_p.h` 后缀

3. **API 设计**
   - 提供清晰的公共接口
   - 使用 RAII 管理资源
   - 支持链式调用（适当场景）

详细规范参考：`Qt6_CPP17_Coding_Style.md`

## AI 使用指引

### 项目文档结构
```
CLAUDE.md                 # 项目总览（本文件）
├── src/qtxdg/CLAUDE.md   # 核心库详细文档
├── src/xdgiconloader/CLAUDE.md  # 图标加载器文档
├── src/qtxdgqml/CLAUDE.md # QML 插件文档
└── examples/CLAUDE.md     # 示例程序文档
```

### AI 辅助开发建议

1. **模块开发**：参考对应模块的 `CLAUDE.md` 了解具体接口
2. **代码生成**：优先使用 Qt 风格的 C++17 特性
3. **测试编写**：参考现有测试用例的模式
4. **文档更新**：修改代码后同步更新相关文档

### 开发工作流
```bash
# 1. 查看相关模块文档
cat src/qtxdg/CLAUDE.md

# 2. 运行测试验证当前状态
make && ctest

# 3. 开发新功能

# 4. 更新文档
vim src/qtxdg/CLAUDE.md
```

## 相关资源

### 官方资源
- **项目主页：** https://github.com/lxqt/libqtxdg
- **LXQt 项目：** https://lxqt-project.org/
- **API 文档：** https://docs.lxqt.org/libqtxdg/

### XDG 规范
- **基础目录规范：** https://specifications.freedesktop.org/basedir-spec/
- **桌面入口规范：** https://specifications.freedesktop.org/desktop-entry-spec/
- **图标主题规范：** https://specifications.freedesktop.org/icon-theme-spec/
- **菜单规范：** https://specifications.freedesktop.org/menu-spec/
- **MIME 应用规范：** https://specifications.freedesktop.org/mime-apps-spec/

### 构建与部署
- **CMake 文档：** https://cmake.org/documentation/
- **Qt6 文档：** https://doc.qt.io/qt-6/
- **QML 开发：** https://doc.qt.io/qt-6/qtqml-index.html

---

## 变更记录 (Changelog)

### 2025-11-08 19:47:55 - AI 上下文完整初始化
- ✨ 完成 AI 上下文完整扫描和文档生成
- 📊 扫描覆盖率：88.3% (128/145 文件)
- 🏗️ 生成完整的模块结构图和导航
- 📝 更新所有模块的 CLAUDE.md 文档
- 🔍 创建 `.claude/index.json` 索引文件
- 📈 识别下次扫描的重点区域

### 2025-11-02 - 项目架构文档更新
- 📋 更新模块索引和依赖关系图
- 🔧 完善构建配置说明
- 📚 添加 QML 插件使用指南
- 🧪 更新测试策略说明

### 2024-10-XX - 初始架构文档
- 🎯 创建项目总览文档
- 📐 确定模块结构
- 🔄 建立文档规范

---

*本文档由 AI 辅助生成和维护，最后扫描时间：2025-11-08 19:47:55*
*扫描覆盖率：88.3% | 已扫描文件：128 | 估算总文件：145*"
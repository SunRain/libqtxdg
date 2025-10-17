# Qt Widgets 依赖拆分 - 重构说明

## 重构日期
2025-11-25

## 重构目标
将 libqtxdg 中依赖 Qt Widgets 的类拆分为独立的可选模块，使核心库（Qt6Xdg）不再强制依赖 Qt Widgets。

## 重构内容

### 1. 新增模块：Qt6XdgWidgets

**目录：** `src/qtxdgwidgets/`

**包含类：**
- `XdgAction` - 基于 XdgDesktopFile 的 QAction 封装
- `XdgMenuWidget` - 基于 XdgMenu 的 QMenu 封装

**依赖关系：**
```
Qt6XdgWidgets → Qt6Xdg (核心库)
Qt6XdgWidgets → Qt6::Widgets
```

### 2. 核心库更新：Qt6Xdg

**移除的依赖：**
- ❌ Qt6::Widgets（已移除）

**保留的依赖：**
- ✅ Qt6::Xml
- ✅ Qt6::DBus
- ✅ Qt6::Gui (QIcon 等)
- ✅ Qt6XdgIconLoader

**移除的公共类：**
- `XdgAction` → 移至 Qt6XdgWidgets
- `XdgMenuWidget` → 移至 Qt6XdgWidgets

### 3. 构建选项

**新增 CMake 选项：**
```cmake
-DBUILD_WIDGETS_LIB=ON/OFF  # 默认: ON
```

**使用示例：**

```bash
# 构建完整版本（包含 Widgets 组件）
cmake .. -DBUILD_WIDGETS_LIB=ON

# 构建精简版本（不包含 Widgets 组件）
cmake .. -DBUILD_WIDGETS_LIB=OFF
```

### 4. 文件结构变化

```
src/
├── qtxdg/                    # 核心库（无 Widgets 依赖）
│   ├── xdgdesktopfile.h/cpp
│   ├── xdgdirs.h/cpp
│   ├── xdgicon.h/cpp
│   ├── xdgmenu.h/cpp
│   └── ...
├── qtxdgwidgets/             # ✨ 新增：Widgets 组件
│   ├── xdgaction.h/cpp
│   ├── xdgmenuwidget.h/cpp
│   └── CMakeLists.txt
└── xdgiconloader/            # 图标加载器
    └── ...
```

### 5. 包配置文件

**新增配置：**
- `Qt6XdgWidgets.pc` - pkg-config 文件
- `qt6xdgwidgets-config.cmake.in` - CMake 配置模板

**更新配置：**
- `Qt6Xdg.pc` - 移除 Qt6Widgets 依赖
- `qt6xdg-config.cmake.in` - 移除 Qt6Widgets 依赖

### 6. 编译验证

#### ✅ 完整构建测试
```bash
cmake .. -DBUILD_WIDGETS_LIB=ON -DBUILD_EXAMPLES=ON -DBUILD_QML_PLUGIN=ON
make -j4
```

**生成库文件：**
- `libQt6Xdg.so.4.2.0` (374KB) - 核心库
- `libQt6XdgWidgets.so.4.2.0` (62KB) - Widgets 组件
- `libQt6XdgIconLoader.so.4.2.0` (111KB) - 图标加载器

**依赖验证：**
```bash
# Qt6Xdg 核心库（无 Widgets 依赖）
$ ldd src/qtxdg/libQt6Xdg.so.4.2.0 | grep Qt6
libQt6Xml.so.6
libQt6Gui.so.6
libQt6DBus.so.6
libQt6Core.so.6

# Qt6XdgWidgets（有 Widgets 依赖）
$ ldd src/qtxdgwidgets/libQt6XdgWidgets.so.4.2.0 | grep Qt6
libQt6Widgets.so.6
libQt6Xdg.so.4
libQt6Xml.so.6
libQt6Gui.so.6
libQt6DBus.so.6
libQt6Core.so.6
```

#### ✅ 精简构建测试
```bash
cmake .. -DBUILD_WIDGETS_LIB=OFF -DBUILD_EXAMPLES=OFF -DBUILD_QML_PLUGIN=OFF
make -j4
```

**结果：** 仅构建核心库和图标加载器，不构建 Qt6XdgWidgets。

## 兼容性说明

### 向后兼容
**对于现有用户：**
- 默认 `BUILD_WIDGETS_LIB=ON`，与之前行为一致
- 所有类和 API 保持不变
- 只需链接 `Qt6XdgWidgets` 即可使用 `XdgAction` 和 `XdgMenuWidget`

### 迁移指南

**如果使用 XdgAction 或 XdgMenuWidget：**

**CMakeLists.txt 更新：**
```cmake
# 旧版本
find_package(Qt6Xdg REQUIRED)
target_link_libraries(myapp Qt6Xdg)

# 新版本（需要 Widgets 组件）
find_package(Qt6Xdg REQUIRED)
find_package(Qt6XdgWidgets REQUIRED)
target_link_libraries(myapp Qt6Xdg Qt6XdgWidgets)
```

**pkg-config 更新：**
```bash
# 旧版本
pkg-config --cflags --libs Qt6Xdg

# 新版本（需要 Widgets 组件）
pkg-config --cflags --libs Qt6Xdg Qt6XdgWidgets
```

## 效益分析

### 优点
1. ✅ **减少依赖：** 核心库不再强制依赖 Qt Widgets
2. ✅ **体积优化：** 不使用 Widgets 组件的应用可减少依赖体积
3. ✅ **灵活性：** 支持无头服务、纯 QML 应用等场景
4. ✅ **向后兼容：** 默认行为与旧版本一致

### 使用场景

**适合使用 Qt6Xdg（核心库）：**
- 桌面文件解析
- XDG 目录管理
- MIME 类型处理
- 图标加载
- 无头服务
- 纯 QML 应用

**需要额外使用 Qt6XdgWidgets：**
- 使用 `XdgAction` 创建菜单项
- 使用 `XdgMenuWidget` 显示应用菜单
- 传统 Qt Widgets 应用

## 测试清单

- [x] 完整构建（BUILD_WIDGETS_LIB=ON）
- [x] 精简构建（BUILD_WIDGETS_LIB=OFF）
- [x] 核心库依赖验证（无 Qt6Widgets）
- [x] Widgets 库依赖验证（有 Qt6Widgets）
- [x] 示例程序编译（无使用 Widgets 组件）
- [x] QML 插件编译

## 相关文件

**修改的文件：**
- `CMakeLists.txt` - 新增 BUILD_WIDGETS_LIB 选项
- `src/CMakeLists.txt` - 添加 qtxdgwidgets 子目录
- `src/qtxdg/CMakeLists.txt` - 移除 Widgets 依赖和相关文件
- `cmake/qt6xdg-config.cmake.in` - 移除 Qt6Widgets 查找

**新增的文件：**
- `src/qtxdgwidgets/` - 新模块目录
- `src/qtxdgwidgets/CMakeLists.txt` - 构建配置
- `src/qtxdgwidgets/xdgaction.h/cpp` - 从 qtxdg 移动
- `src/qtxdgwidgets/xdgmenuwidget.h/cpp` - 从 qtxdg 移动
- `cmake/qt6xdgwidgets-config.cmake.in` - CMake 配置模板

## 后续建议

1. **文档更新：** 更新 README 和 API 文档
2. **测试补充：** 添加针对 Qt6XdgWidgets 的单元测试
3. **示例程序：** 创建使用 XdgAction/XdgMenuWidget 的示例
4. **发布说明：** 在下个版本 CHANGELOG 中说明此变更

---

**重构完成时间：** 2025-11-25 09:50
**重构作者：** 

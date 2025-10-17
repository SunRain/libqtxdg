# libqtxdg 项目总提示词（Droid 使用配置）

用于指导 Droid 在 libqtxdg 仓库内开展工作：涵盖项目背景、目录结构、构建/测试、编码规范（绑定 Qt6_CPP17_Coding_Style.md）、AI 行为（绑定 AI_CODING_BEHAVIOR.md）与提交校验，确保动作一致、可验证、可复制。

—

## 1) 项目概览（来自 CLAUDE.md）
- 名称：libqtxdg — Qt 对 XDG 规范的实现库（LXQt 项目）
- 版本：4.2.0；技术栈：C++17、Qt 6.6+、CMake 3.18+
- 模块：
  - Qt6Xdg（src/qtxdg）：核心实现（XdgDesktopFile、XdgMenu、XdgDirs、XdgIcon、XdgMimeApps、XdgDefaultApps、XdgAutoStart、XdgAction）
  - Qt6XdgIconLoader（src/xdgiconloader）：图标主题与缓存，含 plugin/
  - Qt6XdgQml（src/qtxdgqml）：QML 包装，URI=org.lxqt.qtxdg，依赖 Qt6Xdg
  - examples、test、cmake、config、util

—

## 2) 构建 / 测试（必须遵循）
- 标准构建：
  ```bash
  mkdir build && cd build
  cmake .. -DBUILD_QML_PLUGIN=ON -DBUILD_EXAMPLES=ON
  make -j$(nproc)
  sudo make install
  ```
- 开启测试：
  ```bash
  cmake .. -DBUILD_TESTS=ON
  make
  ctest
  ```
- CMake 选项（默认）：BUILD_TESTS=OFF, BUILD_DEV_UTILS=OFF, BUILD_EXAMPLES=ON, BUILD_QML_PLUGIN=ON, QTXDG_INSTALL_DEFAPPS_CONFIG=ON
- QML 模块输出/安装：build/qml/org/lxqt/qtxdg/；${CMAKE_INSTALL_LIBDIR}/qt6/qml/org/lxqt/qtxdg/

—

## 3) 编码与风格（绑定 ./Qt6_CPP17_Coding_Style.md）
- 主风格来源：`./Qt6_CPP17_Coding_Style.md`
  - 关键强制项（完整细则以该文档为准）：
    - C++17；4 空格缩进；单语句必须加括号；零警告提交（`-Wall -Wextra -Wpedantic`）
    - 命名：成员变量 `m_`，静态/全局 `s_`，常量 `k` 前缀；类/函数/变量驼峰
    - 禁止：异常、RTTI、`dynamic_cast`、裸 new/delete、C 风格转换
    - Qt6 约定：新式信号槽、`QStringLiteral`/`u""_qs`、`Q_OBJECT` 必备、Qt 父子树或智能指针管理
  - 工具化建议：`.clang-format`（KDE 风格）、`.clang-tidy` 最小零警告集（详见源文档 §10）
- 实施要求：
  - 新增公共类：头文件加入 `*_PUBLIC_H_FILES`，类名加入 `*_PUBLIC_CLASSES`
  - 任何变更需同步模块 CMakeLists.txt，保持 include 顺序与 include guards

—

## 4) AI 编码行为策略（绑定 ./AI_CODING_BEHAVIOR.md）
- 策略来源：`./AI_CODING_BEHAVIOR.md`
  - 三层规范：强制规范（必须遵守）/ 可选推荐（新代码默认采用）/ 项目特定约定（沿用主流写法）
  - 决策树：先检查强制规范 → 新/旧代码分流 → 尊重用户明确偏好（前提不违强制）
  - 违规处理：若用户要求违反强制规范，必须拒绝并提供合规替代方案
- 在本项目中的落地：
  - 新功能/新文件：默认采用 `Qt6_CPP17_Coding_Style.md §5` 推荐的现代 C++17 写法（如 `std::optional`、结构化绑定等）
  - 维护既有代码：保持文件内既有风格（不强制重构为现代写法），但在必要处补齐强制项（如 `QStringLiteral`）

—

## 5) 常见任务操作模板
- 新增核心类（src/qtxdg）：添加 .h/.cpp → 更新 CMake（源、`*_PUBLIC_H_FILES`、`*_PUBLIC_CLASSES`）→ 可选补测试
- 扩展 QML 包装（src/qtxdgqml）：新增 Wrapper → `qt_add_qml_module` 注册类型（URI 保持 org.lxqt.qtxdg）→ 构建后验证输出目录
- 图标加载器改动（src/xdgiconloader）：遵循 `xdgiconloader_p.h` 私有实现约束；如改插件，同步 plugin/ 元数据
- 示例与测试：在对应 CMakeLists.txt 注册；由 BUILD_EXAMPLES/BUILD_TESTS 控制

—

## 6) 工具与工作流约定（Droid 内部）
- 代码探索：优先 Grep/Glob/LS；使用绝对路径；避免 cd 副作用
- Git 只读检查先行：`git status`、`git diff`、`git log` 收集上下文
- 环境事实：Linux；ripgrep 可用；gh 不可用；Python 3.13.7 可用（项目以 C++/Qt 为主）

—

## 7) 运行与验证清单
- 构建通过（含启用项）；ctest 全量通过（若启用 BUILD_TESTS）
- QML 类型正确生成并可导入；示例可编译
- 风格/静态检查（参考 `Qt6_CPP17_Coding_Style.md §10` 与 `§11` 自检清单）无阻塞问题

—

## 8) 提交前前置校验（绑定风格/AI 行为）
- 风格/静态检查：
  - clang-format --dry-run 无差异；clang-tidy 零关键告警
  - 检查命名前缀与括号规则；避免异常/RTTI/dynamic_cast/裸 new；字符串使用 `QStringLiteral`
- AI 行为符合性（参考 `AI_CODING_BEHAVIOR.md`）：
  - 未违反强制规范；新代码采用推荐实践；旧代码保持既有风格

—

## 9) 提交与安全检查（在用户明确要求提交时执行）
- 提交前：`git status` / `git diff` / `git diff --cached` 审查；排查敏感信息
- 按仓库风格撰写提交信息（突出动机与影响）；若 pre-commit 失败，修正后重试一次
- 不执行 `git push`，除非用户明确要求

—

## 10) 执行模式
- Spec 模式：先提交计划/设计（本文件即为 Spec 基线）；待确认后执行
- Execute 模式：按确认方案修改，并在完成后用 1–4 句汇总变更与验证结果

—

## 11) 参考
- 项目架构与构建：`./CLAUDE.md`
- 编码风格：`./Qt6_CPP17_Coding_Style.md`
- AI 行为：`./AI_CODING_BEHAVIOR.md`
- XDG 规范：https://specifications.freedesktop.org/

—

> 注：本提示词与项目内 Droid 配置协同使用；如需调整，请在上述源文档中更新规则并同步本文件。

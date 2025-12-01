#=============================================================================
# Copyright 2015 Luís Pereira <luis.artur.pereira@gmail.com>
# Copyright 2013 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

#-----------------------------------------------------------------------------
# 检测编译器类型
#-----------------------------------------------------------------------------
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(QTXDG_COMPILER_IS_CLANGCXX 1)
endif()

#-----------------------------------------------------------------------------
# 设置 C++17 标准
#-----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ ISO Standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#-----------------------------------------------------------------------------
# 全局编译定义
#-----------------------------------------------------------------------------
add_compile_definitions(
    QT_USE_QSTRINGBUILDER
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
    QT_NO_URL_CAST_FROM_STRING
    QT_NO_CAST_FROM_BYTEARRAY
    QT_NO_FOREACH
)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_compile_definitions(QT_STRICT_ITERATORS)
endif()

#-----------------------------------------------------------------------------
# 设置符号可见性
#-----------------------------------------------------------------------------
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#-----------------------------------------------------------------------------
# 编译器警告标志
#-----------------------------------------------------------------------------
if(CMAKE_COMPILER_IS_GNUCXX OR QTXDG_COMPILER_IS_CLANGCXX)
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wchar-subscripts -Wno-long-long
        -Wpointer-arith -Wundef -Wformat-security
        -Wnon-virtual-dtor -Woverloaded-virtual
    )

    # 禁用异常
    add_compile_options(-fno-exceptions)
endif()

if(QTXDG_COMPILER_IS_CLANGCXX)
    # qCDebug(), qCWarning 触发冗长的警告，禁用它
    add_compile_options(-Wno-gnu-zero-variadic-macro-arguments)
endif()

#-----------------------------------------------------------------------------
# 链接器标志 - 禁止未定义符号
#-----------------------------------------------------------------------------
if(CMAKE_COMPILER_IS_GNUCXX OR QTXDG_COMPILER_IS_CLANGCXX)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        # Apple ld64 链接器
        set(NO_UNDEFINED_FLAGS "-Wl,-undefined,error")
        set(SYMBOLIC_FLAGS "")
    else()
        # GNU/Linux ELF 链接器
        set(NO_UNDEFINED_FLAGS "-Wl,--no-undefined")
        set(SYMBOLIC_FLAGS "-Wl,-Bsymbolic-functions")
    endif()

    set(CMAKE_SHARED_LINKER_FLAGS
        "${NO_UNDEFINED_FLAGS} ${SYMBOLIC_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS
        "${NO_UNDEFINED_FLAGS} ${SYMBOLIC_FLAGS} ${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS
        "${SYMBOLIC_FLAGS} ${CMAKE_EXE_LINKER_FLAGS}")
endif()

#-----------------------------------------------------------------------------
# 启用彩色诊断输出（Ninja 生成器）
#-----------------------------------------------------------------------------
if(CMAKE_GENERATOR STREQUAL "Ninja" AND
    ((CMAKE_COMPILER_IS_GNUCXX AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9) OR
     (QTXDG_COMPILER_IS_CLANGCXX AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS 3.5)))
    add_compile_options(-fdiagnostics-color=always)
endif()

#-----------------------------------------------------------------------------
# Honor visibility properties for all target types.
#
# The ``<LANG>_VISIBILITY_PRESET`` and
# ``VISIBILITY_INLINES_HIDDEN`` target properties affect visibility
# of symbols during dynamic linking.  When first introduced these properties
# affected compilation of sources only in shared libraries, module libraries,
# and executables with the ``ENABLE_EXPORTS`` property set.  This
# was sufficient for the basic use cases of shared libraries and executables
# with plugins.  However, some sources may be compiled as part of static
# libraries or object libraries and then linked into a shared library later.
# CMake 3.3 and above prefer to honor these properties for sources compiled
# in all target types.  This policy preserves compatibility for projects
# expecting the properties to work only for some target types.
#
# The ``OLD`` behavior for this policy is to ignore the visibility properties
# for static libraries, object libraries, and executables without exports.
# The ``NEW`` behavior for this policy is to honor the visibility properties
# for all target types.
#
# This policy was introduced in CMake version 3.3.  CMake version
# 3.3.0 warns when the policy is not set and uses ``OLD`` behavior. Use
# the ``cmake_policy()`` command to set it to ``OLD`` or ``NEW``
# explicitly.
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Turn on more aggrassive optimizations not supported by CMake
# References: https://wiki.qt.io/Performance_Tip_Startup_Time
#-----------------------------------------------------------------------------
if (CMAKE_COMPILER_IS_GNUCXX OR QTXDG_COMPILER_IS_CLANGCXX)
    # -flto: use link-time optimizations to generate more efficient code
    if (CMAKE_COMPILER_IS_GNUCXX)
        set(LTO_FLAGS "-flto -fuse-linker-plugin")
        # When building static libraries with LTO in gcc >= 4.9,
        # "gcc-ar" and "gcc-ranlib" should be used instead of "ar" and "ranlib".
        # references:
        #   https://gcc.gnu.org/gcc-4.9/changes.html
        #   http://hubicka.blogspot.tw/2014/04/linktime-optimization-in-gcc-2-firefox.html
        #   https://github.com/monero-project/monero/pull/1065/commits/1855213c8fb8f8727f4107716aab8e7ba826462b
        if (NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9.0")  # gcc >= 4.9
            set(CMAKE_AR "gcc-ar")
            set(CMAKE_RANLIB "gcc-ranlib")
        endif()
    elseif (QTXDG_COMPILER_IS_CLANGCXX)
        # The link-time optimization of clang++/llvm seems to be too aggrassive.
        # After testing, it breaks the signal/slots of QObject sometimes.
        # So disable it for now until there is a solution.
        # set(LTO_FLAGS "-flto")
    endif()
    # apply these options to "Release" build type only
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${LTO_FLAGS}")
endif()

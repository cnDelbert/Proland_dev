# Proland_dev

My development clone of Inria's Proland: [Proland home page](http://proland.imag.fr/). Delbert forked from [LarsFlaeten/Proland_dev](https://github.com/LarsFlaeten/Proland_dev).

The goal is to update and modify the Proland code to be in accordance with OpenGL 3.3, and to work on a standard laptop with intel HD Graphics (Proland was earlier tested only on NVIDIA hardware).

## Introduction

Proland is a C++/OpenGL library for the real-time rendering of multi-resolution terrains (up to whole planets), the real-time management and edition of vector data (representing for instance roads or rivers), the rendering of atmosphere and clouds, the rendering and animation of the oceans, and the rendering of forests. All data are loaded or generated on the fly according to the viewpoint, and can be combined procedurally. For instance the shape and texture of a terrain can be modified using vector data representing roads and rivers.

Proland uses the Ork rendering library. You can find my version of ork here: [My Ork on github](https://github.com/cnDelbert/ork)

## Background

This clone of the Proland librabry is mainly meant to be able to build and run Proland libraries and examples on my Linux computer. The code hierarchy is largely maintained, but the build system is changed to CMake.

## Overview of the library

This directory contains the Proland core library, several "plugins" for this library, and demos using this library and plugins. Each directory 
has its own licence, source tree, examples, documentation and project files. The directories are mostly independent, except for some dependencies between plugins (and between all the plugins and the core library).

Proland is made of a core library and of 7 plugins:

* Core library
* Terrain plugin
* Atmo plugin
* Ocean plugin
* Forrest plugin
* Graph plugin
* River plugin
* Edit plugin

The original Code::Blocks project of Inria has been removed by LF.

## Screenshots

Some new screen shots using srtm/blue marble data sets

![alt text](web/blue_marble.png "Earth with clouds")

![alt text](web/italy.png "Italy")

![alt text](web/Pico_do_fogo.png "Cape Verde islands, Pico do Fogo")

Below is a screenshot of the use of precomputed atmospheric scattering.

![alt text](web/Atmo_test.png "Test of precomputed atmospheric scattering")

Below is a screenshot of the use of precomputed atmospheric scattering together with the ocean module, using the Ocean FFT algorithm.

![alt text](web/Ocean_and_atmo2.png "Test of precomputed atmospheric scattering and ocean")

![alt text](web/Procedural_landscape_test.png "Procedural landscape test")

![alt text](web/first_procedural_planet.png "First procedural planet")

![alt text](web/Blue_marble_test.png "First earth test (with low res texture)")

## Current status

* All modules are includes and now runs on a normal Linux computer with OpenGL 3.3
* Some issues still exists with the use of CPUBuffers from Ork
* Som bugs in the forest/trees rendering (These shaders where translated from Shader model 4 to 3.3 to work on my laptop) 

## Requirements

- Ork library.
- TIFF.
- AntTweakBar.

#### Ork

TBD.

#### AntTweakBar

AntTweakBar is required by **proland** project

AntTweakBar should use the version modified by me in this [repo](https://github.com/cnDelbert/AntTweakBar).

AntTweakBar的pre-build二进制中编译了使用 `texture2D` 的shader，会导致demo编译时崩溃。下载后打开 `AntTweakBar_VS2012.sln` 使用Visual Studio进行编译即可。

#### pThreads

Pthreads-win32 currently implements a large subset of the POSIX standard threads related API. The latest version is 2.9.1(2012-05-27).

The source tree and precompiled .DLL, .LIB and necessary header files are included in the zip file named "pthread-w32-v-v-v-release.zip" at:
<ftp://sourceware.org/pub/pthreads-win32>.

Unzip the zip file and set the include dirs and lib dirs inside `Pre-built.2` folder. 

Add the following line to the top of header file `pthreads.h` to avoid `timespec` re-definition error:

```c++
#define HAVE_STRUCT_TIMESPEC 1
```

根据以下选项使用相应的pThreads库：

```text
In general:
	pthread[VG]{SE,CE,C}[c].dll
	pthread[VG]{SE,CE,C}[c].lib

where:
	[VG] indicates the compiler
	V	- MS VC, or
	G	- GNU C

	{SE,CE,C} indicates the exception handling scheme
	SE	- Structured EH, or
	CE	- C++ EH, or
	C	- no exceptions - uses setjmp/longjmp

	c	- DLL compatibility number indicating ABI and API
		  compatibility with applications built against
		  a snapshot with the same compatibility number.
		  See 'Version numbering' below.
```

在 CMake 指定安装路径时，include路径指定 `Pre-built.2` 路径下的 `include` 文件夹；lib路径指定 `lib` 下的 `x86` （Win32）或 `x64` (X64)路径。


#### TIFF

LibTIFF is required by **proland** project

Tiff is library and tools for TIFF images.

The Master HTTP Site http://www.remotesensing.org/libtiff has been deprecated.

The latest version of `libtiff` on site <http://www.libtiff.org/> is v3.6.1, while the version of [Tiff for Windows](http://gnuwin32.sourceforge.net/packages/tiff.htm) is v3.8.2.

The GnuWin32 distribution comes in two versions. The ordinary version uses the standard Unix equivalents, such as `fopen` and `read`, for input and output, the other version ([Tiff-win32](http://gnuwin32.sourceforge.net/packages/tiff-win32.htm)) uses the Win32 API functions, such as CreateFile and ReadFile, for input and output. The ordinary version ([Tiff](http://gnuwin32.sourceforge.net/packages/tiff.htm)) is more suitable for porting Unix programs, the Win32-API version is more suitable for writing Win32 programs.

When we clicked the Mirror Download Site <http://libtiff.maptools.org/dl/>, a redirection to <http://download.osgeo.org/libtiff> is performed. The latest version of libtiff can be found in the mirror site is v4.3.0rc1 which released on 2021-Apr-16. 

It seems that the [SimpleSystems](http://www.simplesystems.org/) hold the latest libtiff: <http://www.simplesystems.org/libtiff/>.

Libtiff v3.6.1 does not have a CMakeLists.txt, we use v4.3.0 instead.

Zlib1 and JPEG62 are recomended. Or, TIFF warning would show up.

> TBD: TIFF编译建议指定使用 zlib 和 JPEG 库，否则在运行 demo 时会有相关 Warning 并引起渲染错误。



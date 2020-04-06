# OpenImageDenoise-GUI-Qt
Qt GUI for [Intel® OpenImageDenoise](https://openimagedenoise.github.io/).  
Rewritten based on [oidn-gui](https://github.com/chr-9/oidn-gui) for multi-platform support.  
Currently, only multi-channel OpenEXR is supported.  
Build/Tested on ArchLinux.

## Features
- Read/Write multi-channel OpenEXR files
- Batch processing
- CLI Support

## Requirement
- [OpenImageDenoise](https://github.com/OpenImageDenoise/oidn)
- [OpenEXR](https://github.com/AcademySoftwareFoundation/openexr)(IlmImf)
- [SequenceParser](https://github.com/mikrosimage/sequenceparser)
- [Qt5](https://www.qt.io/)

## UI
![ui](https://github.com/chr-9/OIDN-GUI-Qt/blob/master/doc/ui.png)

## Example
![image01t](https://raw.githubusercontent.com/chr-9/oidn-gui/master/doc/01t.png)
![image01t_denoised](https://raw.githubusercontent.com/chr-9/oidn-gui/master/doc/01t_denoised.png)
<!-- ![image01](https://raw.githubusercontent.com/chr-9/oidn-gui/master/doc/01.png) -->
<!-- ![image01_denoised](https://raw.githubusercontent.com/chr-9/oidn-gui/master/doc/01_denoised.png) -->

## Todo
- Release appimage
- Read/write ini files
- Merge output with existing channels
- (Support for multiple aov files)

## License
 [![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)  
 [Qt Open Source Licensing](https://doc.qt.io/qt-5/licensing.html)

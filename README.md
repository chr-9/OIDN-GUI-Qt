# OpenImageDenoise-GUI-Qt
Qt GUI for [Intel® OpenImageDenoise](https://openimagedenoise.github.io/).  
Rewritten based on [oidn-gui](https://github.com/chr-9/oidn-gui) for multi-platform support.  
Currently, only multi-channel OpenEXR is supported.  
Build/Tested on ArchLinux environment.

## Dependency
- [OpenImageDenoise](https://github.com/OpenImageDenoise/oidn)
- [OpenEXR](https://github.com/AcademySoftwareFoundation/openexr)
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
- Reading and writing Ini files
- Merge output with existing channels
- Support for multiple aov files

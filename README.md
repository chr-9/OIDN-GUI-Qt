# OpenImageDenoise-GUI-Qt
Qt GUI for [Intel® OpenImageDenoise](https://openimagedenoise.github.io/).  
Rewritten based on [oidn-gui](https://github.com/chr-9/oidn-gui) for multi-platform support.  
Currently, only multi-channel OpenEXR is supported.  
Build/Tested on ArchLinux, Windows10/11.

## Features
- Read/Write multi-channel OpenEXR files
- Batch processing

## Requirement
- [Qt5](https://www.qt.io/)
- [OpenEXR](https://github.com/AcademySoftwareFoundation/openexr)(IlmImf) 3.x not supported.
- [OpenImageDenoise](https://github.com/OpenImageDenoise/oidn)
- [Boost](https://www.boost.org/)
- [SequenceParser](https://github.com/mikrosimage/sequenceparser)(Included in this repository)

## Usage
- Add Albedo, Normal AOV
- If animation, Enable LockSamplingPattern or fixing NoiseSeed in renderer settings to suppress flickering

## UI
![ui](https://github.com/chr-9/OIDN-GUI-Qt/blob/master/doc/ui.png)

## Example
![image01t](https://raw.githubusercontent.com/chr-9/oidn-gui/master/doc/01t.png)
![image01t_denoised](https://raw.githubusercontent.com/chr-9/oidn-gui/master/doc/01t_denoised.png)

## TODO
- Release appimage / ~~Windows executable~~
- Separate UI and denoise threads
- ~~Read/write ini files~~
- Merge output with existing channels
- Pause/Resume feature
- (Support for multiple aov files)
- (CLI Support)
- Deep EXR Support

## License
 [![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)  
 [Qt Open Source Licensing](https://doc.qt.io/qt-5/licensing.html)

PSP Video Converter (PSPVC) — v0.4 C Edition
PSPVC is a lightweight GTK3 application for converting videos into PSP‑compatible MP4 format.
This release includes both:

PSPVC C Edition — a complete rewrite in C using GTK3

PSPVC Python Edition — the original Python version (pspvc-python)

The C version starts faster, integrates more cleanly with ffmpeg/x264, and has improved UI behavior.
The Python version remains available for users who prefer it.

✨ Features
* Convert any ffmpeg-supported video into PSP‑compatible MP4
* Auto‑generate THM thumbnails
* Auto-suggest PSP filenames (MAQxxxxx.MP4)
* Multiple encoding profiles (H.264/AVC)
* Adjustable audio volume
* Clean GTK3 interface
* Cancel → Close logic with proper ffmpeg termination
* Optional Python version included
* Packaged pixmaps and icons
* Desktop launcher (pspvc.desktop)

📦 Dependencies
* Required (C version)
* GTK3
* ffmpeg
* x264
* pkg-config
* gcc (for building)

Optional (Python version)
* Python 3
* PyGObject (GTK3 bindings)
* ffmpeg
* x264

🔧 Building from source
It's as simple as:
```
./configure
make
sudo make install
```
This installs:
```
/usr/bin/pspvc
/usr/bin/pspvc-python
/usr/share/pspvc/pixmaps/*
/usr/share/applications/pspvc.desktop
```

🐍 Python version
The original Python version is included as:
```pspvc-python```
It is installed into `/usr/bin/` and can be run manually:
pspvc-python

📁 Install locations
| Component | Path |
| --- | --- |
| C binary | ``/usr/bin/pspvc`` |
| Python version | ``/usr/bin/pspvc-python`` |
| Pixmaps | ``/usr/share/pspvc/pixmaps/`` |
| Desktop file | ``/usr/share/applications/pspvc.desktop`` |
| Icon | ``/usr/share/icons/hicolor/.../pspvc.png`` |

📜 License
This project is licensed under the GPLv2 
See the LICENSE file for details.

❤️ Credits
Original concept by Philippe Maes
Python Edition rewrite and updated C Edition rewrite and enhancements by RingtailedFox

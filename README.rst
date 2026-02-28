Cinnamon is a Linux desktop that provides advanced innovative features and a traditional user experience.

The desktop layout is similar to GNOME 2 with underlying technology forked from the GNOME Shell.
Cinnamon makes users feel at home with an easy-to-use and comfortable desktop experience.


Contributing
============
Cinnamon is on GitHub at https://github.com/linuxmint/cinnamon.

Note that some issues may not be with Cinnamon itself. For a list of related components,
please see https://projects.linuxmint.com/cinnamon/.


Handling Windows, Android, and iOS files
=========================================
Cinnamon includes best-effort, non-destructive support for opening the following
file types on Linux via the ``cinnamon-file-handler`` utility:

`.exe` – Windows executables
  Linux cannot natively run Windows executables.  When ``wine`` or ``proton``
  is installed the handler offers to launch the file with that tool.  If
  neither is present the user is shown installation instructions.

  **Optional dependencies** (install one):

  * Wine – ``sudo apt install wine`` / ``sudo dnf install wine`` /
    ``sudo pacman -S wine``
  * Proton – install via *Steam → Settings → Compatibility*

`.apk` – Android packages
  APK files can be side-loaded to an attached Android device or emulator via
  ``adb``.  When ``adb`` is installed the handler shows the exact command to
  run.  If ``adb`` is absent the user is guided to install it.

  **Optional dependencies**:

  * Android Debug Bridge (adb) –
    ``sudo apt install adb`` /
    ``sudo dnf install android-tools`` /
    ``sudo pacman -S android-tools``
  * Android emulator – `Waydroid <https://waydroid.com/>`_ or Android Studio AVD

`.ipa` – iOS application archives
  iOS apps **cannot be installed on Linux** due to Apple's code-signing and
  DRM requirements.  The handler extracts available metadata from the archive
  (IPA files are ZIP archives) and shows a message explaining the limitation.
  No installation is attempted.

None of these handlers modify existing Cinnamon flows or change the behaviour
of already-supported file types.


License
=======
Cinnamon is distributed under the terms of the GNU General Public License,
version 2 or later. See the COPYING file for details.


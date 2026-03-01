#!/usr/bin/python3

import os
import gettext
from mintcommon import additionalfiles

DOMAIN = "lemon"
PATH = "/usr/share/locale"

os.environ['LANGUAGE'] = "en_US.UTF-8"
gettext.install(DOMAIN, PATH)

prefix = """[Desktop Entry]
Exec=env WEBKIT_DISABLE_COMPOSITING_MODE=1 lemon-settings
Icon=preferences-desktop
Terminal=false
Type=Application
Categories=Settings;
StartupNotify=false
OnlyShowIn=X-Lemon;
Keywords=Preferences;Settings;
"""

additionalfiles.generate(DOMAIN, PATH, "files/usr/share/applications/lemon-settings.desktop", prefix, _("System Settings"), _("Control Center"), "")

prefix = """[Desktop Entry]
Exec=lemon-settings-users
Icon=system-users
Terminal=false
Type=Application
Categories=System;Settings;
StartupNotify=false
OnlyShowIn=X-Lemon;
Keywords=Preferences;Settings;
"""

additionalfiles.generate(DOMAIN, PATH, "files/usr/share/applications/lemon-settings-users.desktop", prefix, _("Users and Groups"), _("Add or remove users and groups"), "")

prefix = """[Desktop Entry]
Exec=lemon-dbus-command ToggleKeyboard
Icon=lemon-virtual-keyboard
Terminal=false
Type=Application
Categories=Utility;
OnlyShowIn=X-Lemon;
Keywords=onboard;keyboard;caribou;
"""

additionalfiles.generate(DOMAIN, PATH, "files/usr/share/applications/lemon-onscreen-keyboard.desktop", prefix, _("Virtual keyboard"), _("Turn on-screen keyboard on or off"), "")

# Screensaver authentication library
screensaver_sources = [
    'cs-auth-pam.c',
    'setuid.c',
    'subprocs.c',
]

liblemon_screensaver = static_library(
    'lemon_screensaver',
    screensaver_sources,
    include_directories: include_root,
    dependencies: [glib, gio, gio_unix, pam],
)

# PAM helper executable
executable(
    'lemon-screensaver-pam-helper',
    'lemon-screensaver-pam-helper.c',
    link_with: liblemon_screensaver,
    include_directories: include_root,
    dependencies: [glib, gio, gio_unix],
    install: true,
    install_dir: libexecdir,
)

subdir('backup-locker')

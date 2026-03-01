backup_locker_headers = [
    'gdk-event-filter.h',
    'event-grabber.h',
]

backup_locker_sources = [
    'backup-locker.c',
    'gdk-event-filter-x11.c',
    'event-grabber.c',
]

executable(
    'lemon-backup-locker',
    backup_locker_sources,
    c_args: [
        '-DLOCALEDIR="@0@"'.format(join_paths(prefix, datadir, 'locale')),
        '-DG_LOG_DOMAIN="lemon-backup-locker"',
        '-fno-lto',
    ],
    link_args: ['-fno-lto'],
    include_directories: include_root,
    dependencies: [config_h, X11, xcomposite, xext, gtk, glib, gdkx11, xdo],
    install: true,
    install_dir: libexecdir,
)

install_data(
    'lemon-unlock-desktop',
    install_dir: bindir,
    install_mode: 'rwxr-xr-x',
)

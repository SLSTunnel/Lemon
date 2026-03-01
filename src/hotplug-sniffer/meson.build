sniffer_headers = [
    'hotplug-mimetypes.h',
    'lemon-mime-sniffer.h',
]

sniffer_sources = [
    'lemon-mime-sniffer.c',
    'hotplug-sniffer.c',
]

sniffer_deps = [
    glib,
    gio,
    gtk,
]

executable(
    'lemon-hotplug-sniffer',
    sniffer_sources,
    include_directories: include_root,
    dependencies: sniffer_deps,
    install: true,
    install_dir: libexecdir,
)

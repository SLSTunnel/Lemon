ignore = [
    'lemon-recorder-src.h',
    'lemon-recorder.h',
    st_headers,
    st_private_headers,
    tray_headers,
    sniffer_headers,
    backup_locker_headers,
]

if not internal_nm_agent
    ignore += 'lemon-network-agent.h'
endif

if not get_option('build_recorder')
    ignore += 'lemon-recorder.h'
endif

gnome.gtkdoc(
    'lemon',
    mode: 'xml',
    main_xml: 'lemon-docs.sgml',
    gobject_typesfile: 'lemon.types',
    dependencies: [liblemon_deps, st_dep, lemon_dep],
    src_dir: [
        join_paths(meson.project_source_root(), 'src'),
        join_paths(meson.project_build_root(), 'src'),
    ],
    scan_args: '--rebuild-types',
    mkdb_args: ['--xml-mode', '--output-format=xml'],
    ignore_headers: ignore,
    install: true,
)

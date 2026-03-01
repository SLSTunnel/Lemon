subdir('st')
subdir('hotplug-sniffer')

include_src = include_directories('.')

calendar_generated = gnome.gdbus_codegen('lemon-calendar',
  sources: 'org.lemon.CalendarServer.xml',
  interface_prefix: 'org.lemon.',
  namespace: 'Lemon'
)

lemon_headers = [
    'lemon-action-mode.h',
    'lemon-app.h',
    'lemon-app-system.h',
    'lemon-doc-system.h',
    'lemon-embedded-window.h',
    'lemon-generic-container.h',
    'lemon-glsl-effect.h',
    'lemon-gtk-embed.h',
    'lemon-global.h',
    'lemon-perf-log.h',
    'lemon-screen.h',
    'lemon-screenshot.h',
    'lemon-slicer.h',
    'lemon-stack.h',
    'lemon-touchegg-client.h',
    'lemon-tray-icon.h',
    'lemon-tray-manager.h',
    'lemon-util.h',
    'lemon-window-tracker.h',
    'lemon-wm.h',
]

lemon_private_headers = [
    'lemon-app-private.h',
    'lemon-app-system-private.h',
    'lemon-embedded-window-private.h',
    'lemon-global-private.h',
    'lemon-window-tracker-private.h',
    'lemon-wm-private.h',
]

lemon_sources = [
    'lemon-plugin.c',
    'lemon-app.c',
    'lemon-app-system.c',
    'lemon-doc-system.c',
    'lemon-embedded-window.c',
    'lemon-generic-container.c',
    'lemon-glsl-effect.c',
    'lemon-gtk-embed.c',
    'lemon-global.c',
    'lemon-keyring-prompt.c',
    'lemon-keyring-prompt.h',
    'lemon-perf-log.c',
    'lemon-polkit-authentication-agent.c',
    'lemon-polkit-authentication-agent.h',
    'lemon-screen.c',
    'lemon-screenshot.c',
    'lemon-secure-text-buffer.c',
    'lemon-secure-text-buffer.h',
    'lemon-slicer.c',
    'lemon-stack.c',
    'lemon-touchegg-client.c',
    'lemon-tray-icon.c',
    'lemon-tray-manager.c',
    'lemon-util.c',
    'lemon-window-tracker.c',
    'lemon-wm.c',
    lemon_headers,
    calendar_generated
]

lemon_enum_types = gnome.mkenums_simple(
    'lemon-enum-types',
    sources: lemon_headers,
)

lemon_sources += lemon_enum_types

tray_headers = [
    'tray/na-tray-child.h',
    'tray/na-tray-manager.h',
]

tray_sources = [
    'tray/na-tray-child.c',
    'tray/na-tray-manager.c',
    tray_headers,
]

liblemon_deps = [
    atk,
    cjs,
    clutter,
    cmenu,
    cogl,
    cogl_path,
    dbus,
    gcr,
    gdkx11,
    gi,
    gio,
    gio_unix,
    gl,
    gtk,
    muffin,
    pango,
    polkit,
    st_dep,
    xapp,
    xml,
]

liblemon_deps += nm_deps

if internal_nm_agent
    lemon_sources += [
        'lemon-network-agent.c',
        'lemon-network-agent.h',
    ]
endif

non_gir = []
if get_option('build_recorder')
    lemon_sources += [
        'lemon-recorder.c',
        'lemon-recorder.h',
    ]

    non_gir += [
        'lemon-recorder-src.c',
        'lemon-recorder-src.h',
    ]

    liblemon_deps += [
        gstreamer,
        gstreamer_base,
    ]
endif

install_rpath = ':'.join([muffin_typelibdir, pkglibdir])


liblemon = library(
    'lemon',
    [lemon_sources, non_gir, tray_sources, st_enum_types[1]],
    include_directories: [include_root, include_directories('st', 'tray')],
    install_rpath: install_rpath,
    build_rpath: muffin_typelibdir,
    dependencies: liblemon_deps,
    install: true,
    install_dir: pkglibdir,
)

lemon_dep = declare_dependency(
    include_directories: include_directories('.'),
    dependencies: liblemon_deps,
    link_with: liblemon,
    link_args: ['-Wl,-Bsymbolic', '-Wl,-z,relro', '-Wl,-z,now'],
)

executable(
    'lemon-perf-helper',
    'lemon-perf-helper.c',
    include_directories: include_root,
    dependencies: [config_h, gtk],
    install: true,
    install_dir: libexecdir,
    install_rpath: install_rpath,
    build_rpath: muffin_typelibdir
)

# FIXME
# js_tester = executable(
#     'run-js-test',
#     ['run-js-test.c', lemon_enum_types[1], st_enum_types[1]],
#     c_args: [
#         '-DLEMON_PKGLIBDIR="@0@"'.format(join_paths(prefix, pkglibdir)),
#     ],
#     include_directories: include_root,
#     dependencies: lemon_dep,
#     install: false,
# )

executable(
    'lemon',
    ['main.c', lemon_enum_types[1], st_enum_types[1]],
    c_args: [
        '-DLOCALEDIR="@0@"'.format(join_paths(prefix, datadir, 'locale')),
        '-DLEMON_PKGLIBDIR="@0@"'.format(join_paths(prefix, pkglibdir)),
    ],
    install_rpath: install_rpath,
    build_rpath: muffin_typelibdir,
    include_directories: [include_root, st_inc],
    dependencies: lemon_dep,
    install: true,
)

subdir('screensaver')

lemon_gir_includes = [
    'Clutter-0',
    'ClutterX11-0',
    'CoglPango-0',
    'Cogl-0',
    'Meta-0',
    'CMenu-3.0',
    'Gcr-3',
    'PolkitAgent-1.0',
    st_gir[0],
]

if internal_nm_agent
    lemon_gir_includes += ['NM-1.0']
endif

lemon_gir = gnome.generate_gir(
    liblemon,
    namespace: 'Lemon',
    nsversion: '0.1',
    sources: lemon_sources,
    dependencies: liblemon_deps,
    identifier_prefix: 'Lemon',
    symbol_prefix: 'lemon',
    export_packages: 'lemon',
    includes: lemon_gir_includes,
    install: true,
    install_dir_typelib: pkglibdir,
    install_dir_gir: pkgdatadir,
    extra_args: [
        '-DST_COMPILATION',
        '--quiet',
        '--warn-all',
    ],
)


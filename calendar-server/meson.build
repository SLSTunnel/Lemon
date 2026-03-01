
server_launcher_conf = configuration_data()
server_launcher_conf.set('MUFFINLIBDIR', muffin_typelibdir)
server_launcher_conf.set('PKGLIBDIR', join_paths(prefix, pkglibdir))
server_launcher_conf.set('LIBEXECDIR', join_paths(prefix, libexecdir))

launcher = configure_file(
    input: 'lemon-calendar-server.in',
    output: 'lemon-calendar-server',
    configuration: server_launcher_conf,
    install_dir: bindir,
    install_mode: 'rwxr-xr-x'
)

install_data(
    'lemon-calendar-server.py',
    install_dir: libexecdir,
    install_mode: 'rwxr-xr-x'
)


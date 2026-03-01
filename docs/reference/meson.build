if get_option('docs')
    version_conf = configuration_data()
    version_conf.set('VERSION', version)

    configure_file(
        input: 'version.xml.in',
        output: 'version.xml',
        configuration: version_conf,
    )

    subdir('lemon')
    subdir('lemon-js')
    subdir('lemon-tutorials')
    subdir('st')
endif

service_files = [
  'org.lemon.BackupLocker.service',
  'org.lemon.CalendarServer.service',
  'org.Lemon.HotplugSniffer.service',
  'org.Lemon.Melange.service',
  'org.Lemon.Slideshow.service',
]

foreach service_file : service_files
    configure_file(
        input: service_file + '.in',
        output: service_file,
        configuration: desktop_conf,
        install_dir: servicedir
    )
endforeach

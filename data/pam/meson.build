pamdir = get_option('pam_prefix')
if pamdir == ''
  pamdir = sysconfdir
endif

if get_option('use_debian_pam')
  install_data(
    'lemon.pam.debian',
    rename: 'lemon',
    install_dir: join_paths(pamdir, 'pam.d')
  )
else
  install_data(
    'lemon.pam',
    rename: 'lemon',
    install_dir: join_paths(pamdir, 'pam.d')
  )
endif

sassc = find_program('pysassc', 'sassc')

custom_target(
  'lemon_css',
  input: 'lemon.scss',
  output: 'lemon.css',
  command: [sassc, '@INPUT@', '@OUTPUT@'],
  build_always_stale: true,
  install: true,
  install_dir: join_paths(pkgdatadir, 'theme')
)

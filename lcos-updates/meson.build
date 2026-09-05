project(
  'lcos-updates',
  'cpp',
  version: '0.1',
  license: 'GPL-3.0-or-later',
  meson_version: '>=0.56',
  default_options: [
    'cpp_std=c++17',
    'warning_level=2',
    'prefix=/usr',
  ],
)

gtkmm = dependency('gtkmm-3.0')
inc = include_directories('src')

apt_parse = static_library(
  'apt-parse',
  'src/apt-parse.cpp',
  include_directories: inc,
  install: false,
)

executable(
  'lcos-updates',
  'src/main.cpp',
  'src/window.cpp',
  include_directories: inc,
  link_with: apt_parse,
  dependencies: gtkmm,
  install: true,
  install_dir: get_option('bindir'),
)

helper = executable(
  'lcos-updates-helper',
  'src/helper.cpp',
  include_directories: inc,
  link_with: apt_parse,
  install: true,
  install_dir: get_option('libexecdir'),
)

install_data(
  'data/org.lunduke.LcosUpdates.desktop',
  install_dir: get_option('datadir') / 'applications',
)

install_data(
  'data/org.lunduke.lcos-updates.policy',
  install_dir: get_option('datadir') / 'polkit-1' / 'actions',
)

test_parser = executable(
  'test-parser',
  'tests/test-parser.cpp',
  include_directories: inc,
  link_with: apt_parse,
  install: false,
)

test(
  'simulate-parser',
  test_parser,
  args: [meson.project_source_root() / 'tests' / 'fixtures'],
)

test(
  'helper-refuse-dist-upgrade',
  helper,
  args: ['dist-upgrade'],
  should_fail: true,
)

test(
  'helper-refuse-full-upgrade',
  helper,
  args: ['full-upgrade'],
  should_fail: true,
)

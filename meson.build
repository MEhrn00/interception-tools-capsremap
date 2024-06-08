project(
  'capsremap',
  'cpp',
  version : '0.1.0',
  license : 'MIT',
  license_files : 'LICENSE',
  meson_version : '>=1.1.0',
  default_options : ['warning_level=3']
)


sources = files(
  'src/main.cpp',
)

conf_data = configuration_data()
conf_data.set_quoted('PROJECT_NAME', meson.project_name())
conf_data.set_quoted('PROJECT_VERSION', meson.project_version())
conf_data.set_quoted('PROJECT_DESCRIPTION', 'interception tools plugin for remapping capslock')

configure_file(output : 'capsremap_config.h', configuration : conf_data)
configuration_inc = include_directories('.')

boost_dep = dependency('boost', version : '>=1.74', static : get_option('prefer_static'), modules : ['program_options'])

if get_option('prefer_static')
  program_link_args = ['-static-libstdc++', '-static-libgcc', '-static']
else
  program_link_args = []
endif

executable(
  'capsremap',
  sources : sources,
  include_directories : [configuration_inc],
  link_args : program_link_args,
  dependencies : [boost_dep],
  override_options : ['cpp_std=c++20'],
  install : true,
)

if get_option('install_config')
  install_data('conf/capsremap.yaml', install_dir : '/etc/interception/udevmon.d')
endif

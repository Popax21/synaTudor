project('synaTudor', ['c'],
    version: '1.0.0',
    meson_version: '>= 0.57.0'
)

INSTALL_DIR = '/sbin/tudor'

subdir('libtudor')
subdir('cli')
subdir('tudor-host')
subdir('tudor-host-launcher')
# libfprint-tod module requires libfprint-tod-git to be installed
if get_option('build_tod').enabled() or (get_option('build_tod').auto() and dependency('libfprint-2-tod-1', required: false).found())
    subdir('libfprint-tod')
endif
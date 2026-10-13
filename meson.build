project('synaTudor', ['c'],
    version: '1.0.0',
    meson_version: '>= 0.57.0'
)

add_global_arguments('-Wno-incompatible-pointer-types', '-Wno-int-conversion', '-Wno-implicit-function-declaration', '-Wno-return-mismatch', language : 'c')

INSTALL_DIR = '/sbin/tudor'

subdir('libtudor')
subdir('cli')
subdir('tudor-host')
subdir('tudor-host-launcher')
subdir('libfprint-tod')

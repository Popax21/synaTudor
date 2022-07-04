project('tudor-libfprint-tod', ['c'],
    version: '1.0.0',
    meson_version: '>= 0.46.0'
)

libfprint_tod = dependency('libfprint-2-tod-1')
udev = dependency('udev')

tudor_inc = include_directories('../out/inc')

shared_module('tudor-tod',
    sources: [
        'device.c',
        'record.c',
        'ipc.c',
        'open.c'
    ],
    c_args: '-Wno-missing-braces',
    dependencies: libfprint_tod,
    include_directories: tudor_inc,
    install: true,
    install_dir: libfprint_tod.get_pkgconfig_variable('tod_driversdir')
)

install_data('60-tudor-libfprint-tod.rules',
    install_dir: udev.get_pkgconfig_variable('udevdir'),
)

install_data([ '../out/libfprint_host', '../out/libtudor.so'],
    rename: ['libfprint_host', 'libtudor.so'],
    install_dir: '/sbin/tudor'
)
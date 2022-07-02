project('tudor-libfprint-tod', ['c'],
    version: '1.0.0',
    meson_version: '>= 0.46.0'
)

libfprint_tod = dependency('libfprint-2-tod-1')
udev = dependency('udev')

shared_module('tudor-tod',
    sources: [
        'device.c'
    ],
    dependencies: libfprint_tod,
    install: true,
    install_dir: libfprint_tod.get_pkgconfig_variable('tod_driversdir')
)

install_data('60-tudor-libfprint-tod.rules',
    install_dir: udev.get_pkgconfig_variable('udevdir'),
)

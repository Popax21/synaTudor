tudor_host_launcher_src = [
    'src/main.c'
]

gio_dep = dependency('gio-2.0')
gio_unix_dep = dependency('gio-unix-2.0')
dbus_dep = dependency('dbus-1')

tudor_host_launcher_inc = include_directories('inc')

executable('tudor_host_launcher', tudor_host_launcher_src,
    dependencies: [gio_dep, gio_unix_dep],
    c_args: ['-D_GNU_SOURCE', '-Wno-missing-braces'],
    include_directories: tudor_host_launcher_inc,
    install: true,
    install_dir: INSTALL_DIR,
)

install_data('tudor-host-launcher.service', install_dir: '/usr/lib/systemd/system/')
install_data('net.reactivated.TudorHostLauncher.conf', install_dir: dbus_dep.get_variable(pkgconfig: 'datadir') / 'dbus-1/system.d')
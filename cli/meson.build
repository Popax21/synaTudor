tudor_cli_src = [
    'src/main.c',
    'src/cli.c',
    'src/datastore.c'
]

thread_dep = dependency('threads')
libcrypto_dep = dependency('libcrypto')
libusb_dep = dependency('libusb-1.0')

tudor_cli = executable('tudor_cli', tudor_cli_src,
    dependencies: [thread_dep, libcrypto_dep, libusb_dep, libtudor_dep],
    c_args: '-D_GNU_SOURCE',
    link_args: ['-Wl,-z,noexecstack', '-Wl,--copy-dt-needed-entries'],
    include_directories: ['src'],
    install_rpath: '$ORIGIN',
    install: true,
    install_dir: INSTALL_DIR
)
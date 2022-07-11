tudor_host_src = [
    'src/main.c',
    'src/sandbox.c',
    'src/ipc.c',
    'src/handler.c'
]

tudor_host_flags = []

if get_option('UNMOUNTFS')
    tudor_host_flags += ['-DUNMOUNTFS']
endif

thread_dep = dependency('threads')
libcrypto_dep = dependency('libcrypto')
libusb_dep = dependency('libusb-1.0')
libcap_dep = dependency('libcap')
libseccomp_dep = dependency('libseccomp')

tudor_host_inc = include_directories('inc')

tudor_host = executable('tudor_host', tudor_host_src,
    dependencies: [thread_dep, libcrypto_dep, libusb_dep, libcap_dep, libseccomp_dep, libtudor_dep],
    c_args: ['-D_GNU_SOURCE'] + tudor_host_flags,
    link_args: ['-Wl,-z,noexecstack', '-Wl,--copy-dt-needed-entries'],
    include_directories: ['src', tudor_host_inc],
    install_rpath: '$ORIGIN',
    install: true,
    install_dir: INSTALL_DIR
)
libtudor_src = [
    'src/log.c',
    'src/wdf.c',
    'src/stub.c',
    'src/loader.c',

    'src/pe/data_dir.c',
    'src/pe/reloc.c',
    'src/pe/resource.c',
    'src/pe/opt_header.c',
    'src/pe/sect.c',
    'src/pe/pe.c',
    'src/pe/import.c',
    'src/pe/tls.c',
    'src/pe/export.c',
    'src/pe/exception.c',
    'src/pe/coff_header.c',

    'src/winapi/string.c',
    'src/winapi/list.c',
    'src/winapi/thread.c',
    'src/winapi/handle.c',
    'src/winapi/error.c',
    'src/winapi/api.c',
    'src/winapi/module.c',
    'src/winapi/log.c',
    'src/winapi/proc.c',
    'src/winapi/time.c',
    'src/winapi/reg.c',
    'src/winapi/sync.c',
    'src/winapi/heap.c',
    'src/winapi/pipe.c',
    'src/winapi/prop.c',
    'src/winapi/suspend.c',
    'src/winapi/io.c',
    'src/winapi/system.c',

    'src/winapi/crypt/rsa_aes.c',
    'src/winapi/crypt/evp_digest.c',
    'src/winapi/crypt/hmac.c',
    'src/winapi/crypt/crypt.c',

    'src/winapi/bcrypt/bcrypt.c',
    'src/winapi/bcrypt/ecc.c',
    'src/winapi/bcrypt/aes.c',

    'src/winapi/wdf/string.c',
    'src/winapi/wdf/driver.c',
    'src/winapi/wdf/request.c',
    'src/winapi/wdf/mem.c',
    'src/winapi/wdf/device.c',
    'src/winapi/wdf/object.c',
    'src/winapi/wdf/log.c',
    'src/winapi/wdf/file.c',
    'src/winapi/wdf/evtqueue.c',
    'src/winapi/wdf/queue.c',
    'src/winapi/wdf/reg.c',
    'src/winapi/wdf/usb.c',

    'src/tudor/async.c',
    'src/tudor/driver.c',
    'src/tudor/device.c',
    'src/tudor/storage.c',
    'src/tudor/reg.c'
]

driver_dll_names = [
    'synaFpAdapter104.dll',
    'synaWudfBioUsb104.dll'
]

driver_dlls = custom_target('driver-download', 
    output: driver_dll_names,
    input: ['download_driver.sh', 'installer.sha'],
    command: [find_program('bash'), '@INPUT0@', '@INPUT1@', '@PRIVATE_DIR@', '@OUTDIR@'] + driver_dll_names
)

driver_dll_blobs = generator(find_program('ld'),
    output: '@BASENAME@.o',
    arguments: ['-r', '-b', 'binary', '-o', '@OUTPUT@', '@INPUT@']
).process([driver_dlls])

libtudor_flags = []

if get_option('DBGIMPORT')
    libtudor_flags += ['-DDBGIMPORT']
endif

if get_option('DBGWDF')
    libtudor_flags += ['-DDBGWDF']
endif

thread_dep = dependency('threads')
libcrypto_dep = dependency('libcrypto', version: '>=3.0.0')
libusb_dep = dependency('libusb-1.0')

libtudor_inc = include_directories('inc')

libtudor = shared_library('tudor', [libtudor_src, driver_dll_blobs],
    dependencies: [thread_dep, libcrypto_dep, libusb_dep],
    c_args: ['-D_GNU_SOURCE'] + libtudor_flags,
    link_args: ['-Wl,-z,noexecstack', '-Wl,--copy-dt-needed-entries'],
    include_directories: ['src', libtudor_inc],
    install: true,
    install_dir: INSTALL_DIR
)

libtudor_dep = declare_dependency(
    link_with: libtudor,
    include_directories: libtudor_inc
)
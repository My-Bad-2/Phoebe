project('limine', 'c', license: 'BSD-2-Clause')

limine_arch = get_option('arch')
limine_binaries = [meson.current_source_dir() / 'limine-uefi-cd.bin']

if limine_arch == 'amd64'
    limine_binaries += [
        meson.current_source_dir() / 'limine-bios.sys',
        meson.current_source_dir() / 'limine-bios-cd.bin',
        meson.current_source_dir() / 'BOOTX64.EFI',
    ]
endif

limine_exec = meson.current_source_dir() / 'limine'
run_command(
    meson.get_compiler('c').get_id(),
    meson.current_source_dir() / 'limine.c',
    '-Wall',
    '-Wextra',
    '-std=c99',
    '-O2',
    '-o', limine_exec,
    check: true,
)

limine_dep = declare_dependency(include_directories: include_directories('.'))

if meson.version().version_compare('>=0.54.0')
    meson.override_dependency('limine', limine_dep)
endif
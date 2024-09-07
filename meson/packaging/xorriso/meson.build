iso_files = get_variable('iso_files', [])
xorriso = find_program('xorriso', required: false)
iso_dir = '/tmp' / meson.project_name()
xorriso_args = ['-as', 'mkisofs']

if get_option('kernel_arch') == 'amd64'
    xorriso_args += [
        '-b', 'boot/limine-bios-cd.bin',
        '-no-emul-boot',
        '-boot-load-size', '4',
        '-boot-info-table',
        '--efi-boot', 'boot/limine-uefi-cd.bin',
        '-efi-boot-part',
        '--efi-boot-image',
        '--protective-msdos-label',
    ]
    supports_bios = true
else
    supports_bios = false
endif

if xorriso.found()
    if supports_bios
        iso = custom_target(
            'iso',
            input: kernel,
            output: meson.project_name() + '.iso',
            command: [
                'rm',
                '-rf', iso_dir,
                '&&',
                'mkdir',
                '-p', iso_dir / 'boot',
                '&&',
                'cp',
                '@INPUT@',
                iso_dir / 'boot/kernel.elf',
                '&&',
                'cp',
                iso_files,
                iso_dir / 'boot',
                '&&',
                xorriso,
                xorriso_args,
                iso_dir,
                '-o', '@OUTPUT@',
                '&&',
                limine.get_variable('limine_exec'),
                'bios-install',
                '@OUTPUT@',
                '&&',
                'rm',
                '-rf', iso_dir,
            ],
            build_by_default: true,
            console: true,
            install: false,
            depends: kernel,
        )
    else
        iso = custom_target(
            'iso',
            input: kernel,
            output: meson.project_name() + '.iso',
            command: [
                'rm',
                '-rf', iso_dir,
                '&&',
                'mkdir',
                '-p', iso_dir,
                '&&',
                'cp',
                '@INPUT@',
                iso_dir / 'kernel.elf',
                '&&',
                'cp',
                iso_files,
                iso_dir,
                '&&',
                xorriso,
                xorriso_args,
                iso_dir,
                '-o', '@OUTPUT@',
                '&&',
                'rm',
                '-rf', iso_dir,
            ],
            build_by_default: true,
            console: true,
            install: false,
            depends: kernel,
        )
    endif
endif
ovmf_binary = get_variable('ovmf_binaries', '')
qemu_logfile = meson.project_build_root() / 'qemu.log'
supports_bios = false

qemu_args = [
    '-cpu', 'max',
    '-smp', '1',
    '-m', '512M',
    '-rtc', 'base=localtime',
    '-serial', 'stdio',
    '-boot', 'order=d,menu=on,splash-time=100',
]

qemu_debug_args = ['-no-reboot', '-no-shutdown', '-d', 'int', '-D', qemu_logfile, '-s', '-S']

qemu_additional_args = ['-M', 'accel=kvm:hvf:whpx:haxm:tcg']

if get_option('kernel_arch') == 'amd64'
    qemu = find_program('qemu-system-x86_64', required: false)
    supports_bios = true

    qemu_args += [
        '-M', 'q35',
    ]

    qemu_debug_args += ['-M', 'smm=off']
else
    error('Unknown Architecture')
endif

if supports_bios
    run_target(
        'run_bios',
        command: [
            qemu, qemu_args, 
            qemu_additional_args, 
            '-cdrom', iso
        ],
        depends: iso,
    )

    run_target(
        'run_bios_debug',
        command: [
            qemu, qemu_args,
            qemu_debug_args,
            '-cdrom', iso
        ],
        depends: iso,
    )
endif

run_target('run_uefi', 
    command: [
        qemu, qemu_args, 
        qemu_additional_args, 
        '-cdrom', iso, 
        '-bios', ovmf_binary
    ],
    depends: iso
)

run_target(
    'run_uefi_debug',
    command: [
        qemu, qemu_args,
        qemu_debug_args,
        '-cdrom', iso,
        '-bios', ovmf_binary,
    ],
    depends: iso,
)
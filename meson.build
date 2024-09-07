project(
    'Phoebe',
    ['c', 'cpp'],
    default_options: ['buildtype=debugoptimized'],
    license: 'MIT',
    meson_version: '>=1.0.0',
    subproject_dir: 'external',
    version: '0.0.1',
)

subdir('meson/compiler')
subdir('meson/compiler/c')
subdir('meson/compiler/cpp')

if get_option('debug')
    add_project_arguments('-DDEBUG', language: ['c', 'cpp'])
endif

link_args = [
    '-Wl,-z,max-page-size=0x1000'
]

arch_spec_args = []

if get_option('kernel_arch') == 'amd64'
    arch_spec_args += [
        '-march=x86-64',
        '-msoft-float',
        '-mno-sse',
        '-mno-sse2',
        '-mno-red-zone',
        '-mcmodel=kernel',
    ]

    link_args += [
        '-Wl,-z,noexecstack',
    ]

    add_global_link_arguments(
        '-Wl,-T,'
        + meson.current_source_dir() / 'scripts/link-kernel.amd64.ld',
        language: ['c', 'cpp'],
    )

    if (cpp_compiler_id == 'clang')
        add_global_arguments('-target', 'x86_64-pc-elf', language: ['c', 'cpp'])
        add_global_link_arguments('-target', 'x86_64-pc-elf', language: ['c', 'cpp'])
    endif
endif

c_compile_flags += arch_spec_args
cpp_compile_flags += arch_spec_args

compile_settings_list = [
    {'lang': 'c', 'compiler': c_compiler, 'flags': c_compile_flags},
    {'lang': 'cpp', 'compiler': cpp_compiler, 'flags': cpp_compile_flags},
]

link_settings_list = [
    {'lang': 'c', 'compiler': c_compiler, 'flags': link_args},
    {'lang': 'cpp', 'compiler': cpp_compiler, 'flags': link_args},
]

add_global_link_arguments('-nostdlib', '-static', language: ['c', 'cpp'])

subdir('meson/compiler/check-and-apply-flags')

# Build Modules
subdir('meson/linker/linker-map')

# Subprojects
limine = subproject(
    'limine',
    default_options: 'arch=' + get_option('kernel_arch'),
    required: true,
)

ovmf = subproject('ovmf_binaries', required: true)
ovmf_binaries = ovmf.get_variable(get_option('kernel_arch'))

dependencies = [
    dependency('limine'),
    dependency('libstdcxx', default_options: 'arch=' + get_option('kernel_arch'))
]

dependencies_includes = []

foreach dependency : dependencies
    dependencies_includes += dependency.partial_dependency(includes: true)
endforeach

# Project Source Tree
clangtidy_files = []

subdir('libc')
subdir('kernel')

# Tooling Modules
subdir('meson/analysis/clang-tidy')
subdir('meson/format')

# Packaging
iso_files = [kernel, files('scripts/limine.conf')]
iso_files += limine.get_variable('limine_binaries')

subdir('meson/packaging/xorriso')
subdir('meson/emulators/qemu')
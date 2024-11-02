libc_include = [include_directories('include')]

libc_sources = files('ctype.c')

subdir('stdio')
subdir('string')
subdir('stdlib')

clangtidy_files += libc_sources

llibc = static_library(
    'llibc',
    libc_sources,
    build_by_default: true,
    include_directories: libc_include,
    dependencies: dependencies,
    install: false,
)
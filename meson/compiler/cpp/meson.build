cpp_compiler = meson.get_compiler('cpp')
cpp_compiler_id = cpp_compiler.get_id()

cpp_warning_flags = [
    '-Wold-style-cast',
    '-Wnon-virtual-dtor',
    '-Wctor-dtor-privacy',
    '-Woverloaded-virtual',
    '-Wnoexcept',
    '-Wstrict-null-sentinel',
    '-Wuseless-cast',
    '-Wzero-as-null-pointer-constant',
    '-Wextra-semi',
    '-Wno-frame-address',
    '-Wno-non-virtual-dtor',
]

cpp_compile_flags = [
    '-fno-rtti',
    '-fno-exceptions',
    '-fno-unwind-tables',
    '-std=c++2c'
]
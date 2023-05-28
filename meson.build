project('cpplox', 'cpp',
  version : '0.1',
  default_options : [
    'warning_level=3',
    'cpp_std=c++17',
    'build.cpp_std=c++17',
    'b_sanitize=address',
    'b_sanitize=memory',
    'b_sanitize=undefined',
    'b_lundef=false'
  ]
)

add_global_arguments('-fstandalone-debug', language : 'cpp')

exe = executable(
  'cpplox', 'main.cpp', 'chunk.cpp',
  'debug.cpp','value.cpp', 'vm.cpp',
  'scanner.cpp','compiler.cpp',
  install: true
)

test('basic', exe)

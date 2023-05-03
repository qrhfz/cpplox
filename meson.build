project('cpplox', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++17',
                     'build.cpp_std=c++17',
                     'werror=true'])

exe = executable(
  'cpplox', 'main.cpp', 'chunk.cpp',
  'debug.cpp','value.cpp', 'vm.cpp',
  'scanner.cpp','compiler.cpp',
  install : true
)

test('basic', exe)

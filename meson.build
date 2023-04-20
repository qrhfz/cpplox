project('cpplox', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++14'])

exe = executable('cpplox', 'cpplox.cpp', 'chunk.cpp','debug.cpp','value.cpp',
  install : true)

test('basic', exe)

project('Chat_cliente-servidor', 'cpp')

# Directorios
src_dir = 'src'
tests_dir = 'tests'
bin_dir = meson.current_build_dir() / 'bin'

# Dependencias de gtest
gtest_dep = dependency('gtest', required: true)
gtest_main_dep = dependency('gtest_main', required: true)

# Crear los ejecutables en la carpeta 'bin'
cliente_executable = executable(bin_dir / 'cliente', src_dir / 'cliente.cpp')
servidor_executable = executable(bin_dir / 'servidor', src_dir / 'servidor.cpp')

# Test
test_executable = executable('test_servidor', tests_dir / 'test_servidor.cpp',
  dependencies: [gtest_dep, gtest_main_dep],
  install: false,
  override_options: ['cpp_std=c++17'])

test('test_servidor', test_executable)

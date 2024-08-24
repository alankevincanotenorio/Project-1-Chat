project('Chat_cliente-servidor', 'cpp')

# Directorios
src_dir = 'src'
tests_dir = 'tests'
bin_dir = meson.current_build_dir() / 'bin'

# Dependencias de gtest
gtest_dep = dependency('gtest', required: true)
gtest_main_dep = dependency('gtest_main', required: true)

# Crear los ejecutables en la carpeta 'bin'
#cliente_executable = executable(bin_dir / 'cliente', src_dir / 'cliente.cpp')
main_cliente_executable = executable(bin_dir / 'main_cliente', src_dir / 'main_cliente.cpp')
main_server_executable = executable(bin_dir / 'main_server', src_dir / 'main_server.cpp')

# Test
test_executable = executable('test_server', tests_dir / 'test_server.cpp',
  dependencies: [gtest_dep, gtest_main_dep],
  install: false,
  override_options: ['cpp_std=c++17'])

test('test_servidor', test_executable)
project('Chat_client-server', 'cpp')

# Directories
src_dir = 'src'
tests_dir = 'tests'
bin_dir = meson.current_build_dir() / 'bin'

# gtest dependencies
gtest_dep = dependency('gtest', required: true)
gtest_main_dep = dependency('gtest_main', required: true)

# Create the executables in the 'bin' folder
mainClient_executable = executable(bin_dir / 'mainClient', src_dir / 'mainClient.cpp')
mainServer_executable = executable(bin_dir / 'mainServer', src_dir / 'mainServer.cpp')

# Test
test_executable = executable('test_server', tests_dir / 'test_server.cpp',
  dependencies: [gtest_dep, gtest_main_dep],
  install: false,
  override_options: ['cpp_std=c++17'])

test('test_server', test_executable)
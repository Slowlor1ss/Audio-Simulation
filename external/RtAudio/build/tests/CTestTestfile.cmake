# CMake generated Testfile for 
# Source directory: D:/Projects/Audio-Simulation/external/RtAudio/tests
# Build directory: D:/Projects/Audio-Simulation/external/RtAudio/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(apinames "D:/Projects/Audio-Simulation/external/RtAudio/build/tests/Debug/apinames.exe")
  set_tests_properties(apinames PROPERTIES  _BACKTRACE_TRIPLES "D:/Projects/Audio-Simulation/external/RtAudio/tests/CMakeLists.txt;32;add_test;D:/Projects/Audio-Simulation/external/RtAudio/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(apinames "D:/Projects/Audio-Simulation/external/RtAudio/build/tests/Release/apinames.exe")
  set_tests_properties(apinames PROPERTIES  _BACKTRACE_TRIPLES "D:/Projects/Audio-Simulation/external/RtAudio/tests/CMakeLists.txt;32;add_test;D:/Projects/Audio-Simulation/external/RtAudio/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(apinames "D:/Projects/Audio-Simulation/external/RtAudio/build/tests/MinSizeRel/apinames.exe")
  set_tests_properties(apinames PROPERTIES  _BACKTRACE_TRIPLES "D:/Projects/Audio-Simulation/external/RtAudio/tests/CMakeLists.txt;32;add_test;D:/Projects/Audio-Simulation/external/RtAudio/tests/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(apinames "D:/Projects/Audio-Simulation/external/RtAudio/build/tests/RelWithDebInfo/apinames.exe")
  set_tests_properties(apinames PROPERTIES  _BACKTRACE_TRIPLES "D:/Projects/Audio-Simulation/external/RtAudio/tests/CMakeLists.txt;32;add_test;D:/Projects/Audio-Simulation/external/RtAudio/tests/CMakeLists.txt;0;")
else()
  add_test(apinames NOT_AVAILABLE)
endif()

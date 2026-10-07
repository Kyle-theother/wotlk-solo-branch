# mod-startergear – korrekte AzerothCore CMakeLists.txt
# Verwendet AC_ADD_SCRIPT statt CollectSourceFiles (deprecated/intern)

AC_ADD_SCRIPT("${CMAKE_CURRENT_SOURCE_DIR}/src/mod_startergear.cpp")
AC_ADD_SCRIPT("${CMAKE_CURRENT_SOURCE_DIR}/src/mod_startergear_loader.cpp")


AC_ADD_SCRIPT("${CMAKE_CURRENT_LIST_DIR}/src/QuickTeleport.cpp")

AC_ADD_SCRIPT_LOADER("QuickTeleport" "${CMAKE_CURRENT_LIST_DIR}/src/loader.h")

AC_ADD_CONFIG_FILE("${CMAKE_CURRENT_LIST_DIR}/conf/quick_teleport.conf.dist")

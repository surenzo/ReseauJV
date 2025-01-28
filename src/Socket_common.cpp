#include "Socket.h"

#ifdef _WIN32
#include "Socket_windows.cpp"
#else
#include "Socket_posix.cpp"
#endif

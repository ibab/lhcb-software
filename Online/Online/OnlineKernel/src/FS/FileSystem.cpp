#include "FS/FileSystem.h"
#include <cerrno>
using namespace FS;
using namespace std;

FileSystem::FileSystem(const string & name) 
{
  m_name = name;
}
#ifdef __linux
#include <sys/statvfs.h>
long FileSystem::freeBytes(void)   {
  struct statvfs buf;
  if (!(statvfs(m_name.c_string(), &buf))) {
    return (buf.f_bsize * buf.f_bavail);
  } else {
    return ::errno;
  }		 	
}
#else
long FileSystem::freeBytes(void)
{
  return 0;
}
#endif


int FileSystem::format(void) 
{
  return 0;
}



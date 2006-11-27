#if 0

#ifndef PLUGINSVC_DIRMANIP_H
#define PLUGINSVC_DIRMANIP_H

#include <climits>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32     /* Windows  */
#include "io.h"
#include "direct.h"
#define S_IRWXU _S_IREAD|_S_IWRITE
#define S_IRGRP 0
#define S_IROTH 0

typedef _finddata_t dirent;
inline const char* dirname(const dirent* e) {  return e->name;  }
inline int mkdir(const char* d, int )       {  return mkdir(d); }

#define S_ISDIR(x)   ((x&_S_IFDIR)==_S_IFDIR)
#define PATH_MAX _MAX_PATH

struct DIR  {
  bool start;
  long handle;
  _finddata_t data;
};
typedef _finddata_t dirent;
inline DIR* opendir(const char* specs)  {
  struct stat buf;
  if (stat(specs, &buf) < 0)
    return 0;
  if (!S_ISDIR(buf.st_mode))
    return 0;
  std::string path = specs;
  path += "/*";
  std::auto_ptr<DIR> dir(new DIR);
  dir->start = true;
  dir->handle = _findfirst(path.c_str(),&dir->data);
  if ( dir->handle != -1 )  {
    return dir.release();
  }
  return 0;
}
inline dirent* readdir(DIR* dir)  {
  if ( dir )  {
    if ( dir->start ) {
      dir->start = false;
      return &dir->data;
    }
    else if ( _findnext(dir->handle,&dir->data)==0 )  {
      return &dir->data;
    }
  }
  return 0;
}
inline int closedir(DIR* dir)  {
  if ( dir )  {
    int sc = _findclose(dir->handle);
    delete dir;
    return sc;
  }
  return EBADF;
}
inline const char* dirname(const char* path) {
  static std::string p;
  std::string tmp = path;
  std::string::size_type idx = tmp.rfind("/");
  if ( idx != std::string::npos ) {
    p = tmp.substr(0,idx);
    return p.c_str();
  } 
  if ( (idx=tmp.rfind("\\")) != std::string::npos ) {
    p = tmp.substr(0,idx);
    return p.c_str();
  } 
  p = "";
  return p.c_str();
}
inline const char* basename(const char* path) {
  static std::string p;
  std::string tmp = path;
  if ( tmp.rfind("/") != std::string::npos ) {
    p = tmp.substr(tmp.rfind("/")+1);
    return p.c_str();
  } 
  p = tmp;
  return p.c_str();
}

#else

#include <unistd.h>
#include <dirent.h>

typedef std::pair<DIR*,const char*> _finddata_t;

inline const char* dirName(_finddata_t& e) {  return e.second; }
inline long _findfirst(const char* spec, _finddata_t* d) {
  struct stat buf;
  if (stat(spec, &buf) < 0)
    return -1;
  if (!S_ISDIR(buf.st_mode))
      return -1;
  d->first = opendir(spec);
  if ( d->first ) {
    dirent* p = readdir(d->first);
    if ( p )   {
      d->second = p->d_name;
      return long(d->first);
    }
    closedir(d->first);
    return -1;
  }
  return -1;
}
inline long _findnext(long, _finddata_t* d) {
  dirent* p = readdir(d->first);
  if ( p ) {
    d->second = p->d_name;
    return 0;
  }
  return long(d->first);
}
inline long _findclose(long hfile) {
  if ( 0 != hfile ) ::closedir((DIR*)hfile);
  return 0;
}
#endif

#endif  

#endif


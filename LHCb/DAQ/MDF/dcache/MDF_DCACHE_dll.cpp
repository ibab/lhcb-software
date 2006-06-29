// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/dcache/MDF_DCACHE_dll.cpp,v 1.3 2006-06-29 19:35:58 frankb Exp $
//	====================================================================
//  MDFIO.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "MDF/PosixIO.h"
#include <cstring>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#ifdef _BUILD
#include "dcap.h"

namespace {
  int mdf_open(const char* fn, int a,unsigned int b)  {  return dc_open(fn,a,b);       }
  int* mdf_errno()                            {  return &dc_errno;             }
  char* mdf_strerror()                        {  return (char*)dc_strerror(dc_errno); }
}
extern "C" EXPORT LHCb::PosixIO* MDF_RFIO()  {
  typedef LHCb::PosixIO _IO;
  static _IO p;
  if ( 0 == p.open )  {
    memset(&p,0,sizeof(p));
    p.unbuffered  = _IO::COMPLETE;
    p.open      = mdf_open;
    p.close     = dc_close;
    p.read      = dc_read;
    p.write     = dc_write;
    p.lseek     = dc_lseek;
    p.lseek64   = dc_lseek64;
    p.access    = dc_access;
    p.unlink    = dc_unlink;
    p.stat      = dc_stat;
    p.stat64    = dc_stat64;

    p.fopen     = dc_fopen;
    p.fclose    = dc_fclose;
    p.fwrite    = dc_fwrite;
    p.fread     = dc_fread;
    p.fseek     = dc_fseeko;
    p.fseek64   = dc_fseeko64;
    p.fstat     = dc_fstat;
    p.fstat64   = dc_fstat64;
    p.ftell     = dc_ftell;
    p.ftell64   = dc_ftello64;

    p.mkdir     = dc_mkdir;
    p.rmdir     = dc_rmdir;
    p.opendir   = dc_opendir;
    p.readdir   = dc_readdir;
    p.closedir  = dc_closedir;
    p.serror    = mdf_strerror;

    p.setopt    = 0;

  #ifdef _WIN32
    p.serrno    = mdf_errno;
    p.ioerrno   = mdf_errno;
  #endif
  }
  return &p;
}
#endif

// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/dcache/MDF_DCACHE_dll.cpp,v 1.2 2006-06-29 18:12:38 frankb Exp $
//	====================================================================
//  MDFIO.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "MDF/PosixIO.h"

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#if 0
#include "dcap.h"
namespace {
  int mdf_open(const char* fn, int a, int b)  {  return dc_open(fn,a,b);       }
  int mdf_lseek(int s, long off, int fl)      {  return dc_lseek64(s,off,fl);  } 
  int* mdf_errno()                            {  return &dc_errno;             }
  char* mdf_strerror()                        {  return dc_strerror(dc_errno); }
}
extern "C" EXPORT LHCb::PosixIO* MDF_RFIO()  {
  static LHCb::PosixIO p;
  if ( 0 == p.open )  {
    p.open      = mdf_open;
    p.close     = dc_close;
    p.read      = dc_read;
    p.write     = dc_write;
    p.lseek     = mdf_lseek;
    p.lseek64   = dc_lseek64;
    p.access    = dc_access;
    p.unlink    = dc_unlink;
    p.stat      = dc_stat;
    p.stat64    = dc_stat64;

    p.fopen     = dc_fopen;
    p.fstat     = dc_fstat;
    p.fstat64   = dc_fstat64;
    p.ftell     = dc_ftell;
    p.ftell64   = dc_ftello64;
    p.setopt    = 0;

    p.mkdir     = dc_mkdir;
    p.opendir   = dc_opendir;
    p.readdir   = dc_readdir64;
    p.closedir  = dc_closedir;
    p.serror    = mdf_strerror;

  #ifdef _WIN32
    p.serrno    = mdf_errno;
    p.ioerrno   = mdf_errno;
  #endif
  }
  return &p;
}
#endif

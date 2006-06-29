// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/gfal/MDF_GFAL_dll.cpp,v 1.1 2006-06-29 17:51:08 frankb Exp $
//	====================================================================
//  MDFIO.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "MDF/PosixIO.h"
#include <cerrno>

#if 0
#include "gfal_api.h"

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

namespace {
  int* mdf_errno()  {  return &errno; }
}

extern "C" EXPORT LHCb::PosixIO* MDF_RFIO()  {
  static LHCb::PosixIO p;
  if ( 0 == p.open )  {
    p.open      = gfal_open;
    p.close     = gfal_close;
    p.read      = gfal_read;
    p.write     = gfal_write;
    p.lseek     = gfal_lseek;
    p.lseek64   = gfal_lseek;
    p.access    = gfal_access;
    p.unlink    = gfal_unlink;
    p.stat      = gfal_stat;
    p.stat64    = gfal_stat64;

    p.fopen     = 0;
    p.fstat     = 0;
    p.fstat64   = 0;
    p.ftell     = 0;
    p.ftell64   = 0;
    p.setopt    = 0;
    p.mkdir     = gfal_mkdir;
    p.opendir   = gfal_opendir;
    p.readdir   = gfal_readdir;
    p.closedir  = gfal_closedir;
    p.serror    = 0;
  #ifdef _WIN32
    p.serrno    = mdf_errno;
    p.ioerrno   = mdf_errno;
  #endif
  }
  return &p;
}
#endif

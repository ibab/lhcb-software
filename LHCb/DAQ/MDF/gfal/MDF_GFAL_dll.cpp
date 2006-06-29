// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/gfal/MDF_GFAL_dll.cpp,v 1.3 2006-06-29 19:35:58 frankb Exp $
//	====================================================================
//  MDFIO.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "MDF/PosixIO.h"
#include <cerrno>
#include <cstring>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#ifdef _BUILD
#include "gfal_api.h"

namespace {
  int* mdf_errno()  {  return &errno; }
}

extern "C" EXPORT LHCb::PosixIO* MDF_RFIO()  {
  typedef LHCb::PosixIO _IO;
  static _IO p;
  if ( 0 == p.open )  {
    memset(&p,0,sizeof(p));
    p.unbuffered  = _IO::COMPLETE;
    p.open      = gfal_open;
    p.close     = gfal_close;
    p.access    = gfal_access;
    p.unlink    = gfal_unlink;
    p.read      = gfal_read;
    p.write     = gfal_write;
    p.lseek     = gfal_lseek;
    p.lseek64   = gfal_lseek64;
    p.stat      = gfal_stat;
    p.stat64    = gfal_stat64;
    p.fstat     = 0;
    p.fstat64   = 0;

    p.buffered = _IO::NONE;
    p.fopen     = 0;
    p.fclose    = 0;
    p.ftell     = 0;
    p.ftell64   = 0;
    p.setopt    = 0;

    p.directory = _IO::COMPLETE;
    p.mkdir     = gfal_mkdir;
    p.rmdir     = gfal_rmdir;
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

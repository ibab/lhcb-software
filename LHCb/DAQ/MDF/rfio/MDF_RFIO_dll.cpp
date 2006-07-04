// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/rfio/MDF_RFIO_dll.cpp,v 1.7 2006-07-04 16:26:39 frankb Exp $
//	====================================================================
//  MDFIO.cpp
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#include "MDF/PosixIO.h"
#include <cstdio>
#include <cstring>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif


extern "C" {
  int   rfio_open(const char *filepath, int flags, unsigned int mode);
  int   rfio_close(int s);
  int   rfio_read(int s, void *ptr, unsigned int size);
  int   rfio_write(int s, const void *ptr, unsigned int size);
  long  rfio_lseek(int s, long offset, int how);
  long long int rfio_lseek64(int s, long long int offset, int how);
  int   rfio_access(const char *filepath, int mode);
  int   rfio_unlink(const char *filepath);
  int   rfio_parse(const char *name, char **host, char **path);
  int   rfio_stat(const char *path, struct stat *statbuf);
  int   rfio_stat64(const char *path, struct stat64 *statbuf);
  int   rfio_fstat(int s, struct stat *statbuf);
  int   rfio_fstat64(int s, struct stat64 *statbuf);

  FILE* rfio_fopen(const char *,const char *);
  int   rfio_fclose(FILE*);
  long  rfio_ftell(FILE*);
  size_t rfio_fwrite(const void *,unsigned int,unsigned int, FILE*);
  size_t rfio_fread(void *,unsigned int,unsigned int, FILE*);
  int   rfio_fseek(FILE *, long int, int);
  int rfio_fseeko64(FILE*, long long int, int);
  long long int rfio_ftello64(FILE*);

  void  rfio_perror(const char *msg);
  char *rfio_serror();
  int   rfiosetopt(int opt, int *pval, int len);

  int   rfio_mkdir(const char *path, unsigned int mode);
  int   rfio_rmdir(const char *path);
  DIR  *rfio_opendir(const char *dirpath);
  int   rfio_closedir(DIR *dirp);
  dirent *rfio_readdir(DIR *dirp);

  // long long int rfio_ftell64(void*);
  // long  rfio_tell(int);
  // long long int rfio_tell64(int);
#ifdef _WIN32
  int  *C__serrno(void);
  int  *C__rfio_errno (void);
#endif
};

extern "C" EXPORT LHCb::PosixIO* MDF_RFIO()  {
  typedef LHCb::PosixIO _IO;
  static _IO p;
  if ( 0 == p.open )  {
    memset(&p,0,sizeof(p));
    p.unbuffered  = _IO::COMPLETE;
    p.open      = rfio_open;
    p.close     = rfio_close;
    p.read      = rfio_read;
    p.write     = rfio_write;
    p.lseek     = rfio_lseek;
    p.lseek64   = rfio_lseek64;
    p.access    = rfio_access;
    p.unlink    = rfio_unlink;
    p.stat      = rfio_stat;
    p.stat64    = rfio_stat64;
    p.fstat     = rfio_fstat;
    p.fstat64   = rfio_fstat64;

    p.buffered  = _IO::PARTIAL;
    p.fopen     = rfio_fopen;
    p.fclose    = rfio_fclose;
    p.fwrite    = rfio_fwrite;
    p.fread     = rfio_fread;
    p.fseek     = rfio_fseek;
    p.fseek64   = 0;//rfio_fseeko64;
    p.ftell     = rfio_ftell;
    p.ftell64   = 0;//rfio_ftello64;

    p.directory = _IO::COMPLETE;
    p.rmdir     = rfio_rmdir;
    p.mkdir     = rfio_mkdir;
    p.opendir   = rfio_opendir;
    p.readdir   = rfio_readdir;
    p.closedir  = rfio_closedir;

    p.serror    = rfio_serror;

    p.setopt    = rfiosetopt;

  #ifdef _WIN32
    p.serrno    = C__serrno;
    p.ioerrno   = C__rfio_errno;
  #endif
  }
  return &p;
}

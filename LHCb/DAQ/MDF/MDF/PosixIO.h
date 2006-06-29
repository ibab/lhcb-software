// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/PosixIO.h,v 1.2 2006-06-29 17:51:07 frankb Exp $
//	====================================================================
//  PosixIO.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef MDF_POSIXIO_H
#define MDF_POSIXIO_H

struct stat;
struct stat64;

/*
 *    LHCb namespace
 */
namespace LHCb    {

  /**@class PosixIO 
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class PosixIO  {
  public:
    PosixIO() : open(0), close(0), read(0), write(0), lseek(0), access(0), stat(0), fstat(0) {}
    int   (*open)     (const char *filepath, int flags, int mode);
    int   (*close)    (int s);
    int   (*read)     (int s, void *ptr, int size);
    int   (*write)    (int s, const void *ptr, int size);
    int   (*lseek)    (int s, int offset, int how);
    int   (*lseek64)  (int s, long long int offset, int how);
    int   (*access)   (const char *filepath, int mode);
    int   (*unlink)   (const char *filepath);
    int   (*stat)     (const char *path, struct stat *statbuf);
    int   (*stat64)   (const char *path, struct stat64 *statbuf);
    FILE* (*fopen)    (const char *, const char *);
    int   (*fstat)    (int s, struct stat *statbuf);
    int   (*fstat64)  (int s, struct stat64 *statbuf);
    long  (*ftell)    (void*);
    long long int (*ftell64)(void*);
    int   (*setopt)   (int opt, int *pval, int len);
    int   (*mkdir)    (const char *path, int mode);
    void *(*opendir)  (const char *dirpath);
    int   (*closedir) (void *dirp);
    void *(*readdir)  (void *dirp);
    char *(*serror)   ();
  #ifdef _WIN32
    int  *(*serrno)   (void);
    int  *(*ioerrno) (void);
  #endif
  };
}       // End namespace LHCb
#endif  // MDF_POSIXIO_H

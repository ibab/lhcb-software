// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/PosixIO.h,v 1.4 2006-06-29 19:35:56 frankb Exp $
//	====================================================================
//  PosixIO.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef MDF_POSIXIO_H
#define MDF_POSIXIO_H

#include <cstdio>
struct stat;
struct stat64;
struct dirent;
#ifdef _WIN32
struct DIR;
#else
#include <dirent.h>
#endif

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
    enum { NONE, PARTIAL, COMPLETE };
    PosixIO() : unbuffered(false), open(0) {}

    int  unbuffered;
    int           (*open)     (const char *filepath, int flags, unsigned int mode);
    int           (*close)    (int s);
    int           (*access)   (const char *filepath, int mode);
    int           (*unlink)   (const char *filepath);
    int           (*read)     (int s, void *ptr, unsigned int size);
    int           (*write)    (int s, const void *ptr, unsigned int size);
    long          (*lseek)    (int s, long offset, int how);
    long long int (*lseek64)  (int s, long long int offset, int how);
    int           (*stat)     (const char *path, struct stat *statbuf);
    int           (*stat64)   (const char *path, struct stat64 *statbuf);
    int           (*fstat)    (int s, struct stat *statbuf);
    int           (*fstat64)  (int s, struct stat64 *statbuf);

    int  buffered;
    FILE*         (*fopen)    (const char *, const char *);
    int           (*fclose)   (FILE*);
    size_t        (*fwrite)   (const void*, unsigned int, unsigned int, FILE*);
    size_t        (*fread)    (void*, unsigned int, unsigned int, FILE*);
    long          (*ftell)    (FILE*);
    long long int (*ftell64)  (FILE*);
    int           (*fseek)    (FILE *, long int, int);
    int           (*fseek64)  (FILE*, long long int, int);

    int  directory;
    int           (*mkdir)    (const char *path, unsigned int mode);
    int           (*rmdir)    (const char *path);
    DIR*          (*opendir)  (const char *dirpath);
    int           (*closedir) (DIR *dirp);
    dirent*       (*readdir)  (DIR *dirp);

    int   (*setopt)   (int opt, int *pval, int len);
    char *(*serror)   ();
  #ifdef _WIN32
    int  *(*serrno)   (void);
    int  *(*ioerrno) (void);
  #endif
  };
}       // End namespace LHCb
#endif  // MDF_POSIXIO_H

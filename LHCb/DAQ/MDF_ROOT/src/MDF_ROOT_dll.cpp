// $Id: MDF_ROOT_dll.cpp,v 1.5 2008-03-03 20:07:38 frankb Exp $
//  ====================================================================
//  MDFIO.cpp
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#include "MDF/PosixIO.h"
#include <cstdio>
#include <cstring>
#ifdef _WIN32
#include <io.h>
#include <sys/stat.h>
static const int S_IRWXU = (S_IREAD|S_IWRITE);
#define EXPORT __declspec(dllexport)
#else
#include <ctype.h>
#include <unistd.h>
#define O_BINARY 0
#define EXPORT
#endif

#include "TUrl.h"
#include "TFile.h"
#include "TSystem.h"
#include <map>
#include <iostream>
namespace {
#if 0

  /// This is no longer needed, since now these TFile properties are accessible
  struct MyFile : public TFile {
    Long64_t offset() const { 
      const TFile* f = this;
      std::cout << "TFile::Read> Bytes read:" << f->GetBytesRead() 
		<< " Size:"             << f->GetSize() 
		<< " Relative offset:"  << (long)f->GetRelOffset()
		<< " Archive offset:"   << (long)this->fArchiveOffset
		<< std::endl;
      return this->GetRelOffset(); 
    }
    Long64_t aoffset() const { return this->fArchiveOffset; }
  };
#endif
  typedef std::map<int,TFile*> FileMap;
  FileMap& fileMap() {
    static FileMap s_fileMap;
    return s_fileMap;
  }
  int root_open(const char *filepath, int flags, unsigned int mode) {
    TFile* f = 0;
    TUrl url(filepath);
    TString opts="filetype=raw", proto, spec;
    opts += url.GetOptions();
    url.SetOptions(opts);
    proto = url.GetProtocol();
    if ( proto == "file" || proto == "http" ) {
      spec = filepath;
      spec += "?filetype=raw";
    }
    else {
      spec = url.GetUrl();
    }
    //std::cout << "URL:" << url.GetUrl() << std::endl;
    //std::cout << "   opts:    " << url.GetOptions() << std::endl;
    //std::cout << "   protocol:" << url.GetProtocol() << std::endl;
    //std::cout << "   specs:   " << (const char*)spec << std::endl;
    if ( (flags&(O_WRONLY|O_CREAT))!=0 && ((mode&S_IWRITE)!= 0) ) {
      f = TFile::Open(spec,"RECREATE","",0);
    }
    else if ( (flags==O_RDONLY || flags==(O_BINARY|O_RDONLY)) && (mode&S_IREAD)!=0 ) {
      f = TFile::Open(spec);
    }
    if ( f && !f->IsZombie() ) {
      int fd = int(fileMap().size()+0xFEED);
      fileMap()[fd] = f;
      return fd;
    }
    return -1;
  }
  int root_close(int fd) {
    FileMap::iterator i = fileMap().find(fd);
    if ( i != fileMap().end() ) {
      TFile* f = (*i).second;
      if ( f ) {
        if ( !f->IsZombie() ) f->Close();
        delete f;
      }
      fileMap().erase(i);
      return 0;
    }
    return -1;
  }
  int root_access(const char *nam, int mode)   {
    return kFALSE==gSystem->AccessPathName(nam, (mode&S_IWRITE) != 0 ? kWritePermission : kReadPermission) ? 0 : -1;
  }
  int root_unlink(const char*) {  return -1; }
  long long int root_lseek64(int fd, long long int offset, int how) {
    FileMap::iterator i = fileMap().find(fd);
    if ( i != fileMap().end() ) {
      TFile* f = (*i).second;
      Long64_t off;
      switch(how) {
      case SEEK_SET:
        f->Seek(offset,TFile::kBeg);
        return f->GetRelOffset();
      case SEEK_CUR:
        f->Seek(offset,TFile::kCur);
        return f->GetRelOffset();
      case SEEK_END:
	off = f->GetRelOffset();
        f->Seek(offset,TFile::kEnd);
        return offset==0 && off==f->GetSize() ? off : f->GetRelOffset();
      }
    }
    return -1;
  }
  long root_lseek(int s, long offset, int how) {
    return (long)root_lseek64(s,offset,how);
  }
  int root_read(int fd, void *ptr, unsigned int size) {
    FileMap::iterator i = fileMap().find(fd);
    if ( i != fileMap().end() ) {
      TFile* f = (*i).second;
      if ( f->GetBytesRead()+size > f->GetSize() ) {
	/*
	std::cout << "TFile::Read> Bytes read:" << f->GetBytesRead() 
		  << " Size:"             << f->GetSize() 
		  << " Relative offset:"  << (long)f->GetRelOffset()
		  << " Archive offset:"   << (long)this->fArchiveOffset
		  << std::endl;
	*/
	return 0;
      }
      if ( f->ReadBuffer((char*)ptr,size)==0 )
        return size;
    }
    return -1;
  }
  int root_write(int fd, const void *ptr, unsigned int size) {
    FileMap::iterator i = fileMap().find(fd);
    if ( i != fileMap().end() ) {
      if ( (*i).second->WriteBuffer((const char*)ptr,size)==0 ) 
        return size;
    }
    return -1;
  }
  int root_stat(const char*   /* path */, struct stat * /*statbuf */) {    return -1;  }
  int root_stat64(const char* /* path */, struct stat64 * /* statbuf */) { return -1; }
  int root_fstat (int /* s */,  struct stat* /* statbuf */) { return -1; }
  int root_fstat64(int /* s */, struct stat64* /* statbuf */) { return -1; }
  char* root_serror() {
    return (char*)gSystem->GetError();
  }
}

extern "C" EXPORT LHCb::PosixIO* MDF_ROOT()  {
  typedef LHCb::PosixIO _IO;
  static _IO p;
  if ( 0 == p.open )  {
    memset(&p,0,sizeof(p));
    p.unbuffered  = _IO::COMPLETE;
    p.open      = root_open;
    p.close     = root_close;
    p.read      = root_read;
    p.write     = root_write;
    p.lseek     = root_lseek;
    p.lseek64   = root_lseek64;
    p.access    = root_access;
    p.unlink    = root_unlink;
    p.stat      = root_stat;
    p.stat64    = root_stat64;
    p.fstat     = root_fstat;
    p.fstat64   = root_fstat64;

    p.buffered  = _IO::NONE;
    p.fopen     = 0;
    p.fclose    = 0;
    p.fwrite    = 0;
    p.fread     = 0;
    p.fseek     = 0;
    p.ftell     = 0;
#ifdef __linux__
    p.fseek64   = 0;
    p.ftell64   = 0;
#else
#ifndef WIN32
    p.fseek64   = 0;
    p.ftell64   = 0;
#endif // WIN32
#endif
    p.directory = _IO::NONE;
    p.rmdir     = 0;
    p.mkdir     = 0;
    p.opendir   = 0;
    p.readdir   = 0;
    p.closedir  = 0;

    p.serror    = root_serror;

    p.setopt    = 0;
#ifdef _WIN32
    p.serrno    = 0;
    p.ioerrno   = 0;
#endif
  }
  return &p;
}

// $Id: DataFile.cpp,v 1.5 2010-10-21 06:46:18 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Author     : M.Frank
//  Created    : 20/09/2010
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/DataFile.cpp,v 1.5 2010-10-21 06:46:18 frankb Exp $

// Framework include files
#include "RTL/rtl.h"
#include "ROMon/DataFile.h"

// C++ include files
#include <fcntl.h>
#include <cstring>
#include <cerrno>

using namespace ROMon;
using namespace std;

#ifdef _WIN32
#include <io.h>
#include <sys/stat.h>
#endif

/// Initializing constructor
DataFile::DataFile(const string& n) : m_fd(0), m_name(n)  {
  ::memset(&m_stbuff,0,sizeof(m_stbuff));
}

/// Default destructor
DataFile::~DataFile() {
  if ( m_fd ) close();
  ::memset(&m_stbuff,0,sizeof(m_stbuff));
}

/// Check if file migrated
bool DataFile::migrated()  const {
  struct stat statbuf;
  if ( m_fd ) {
    if ( 0 == ::stat(name().c_str(),&statbuf) ) {
      bool mig = m_stbuff.st_ino != statbuf.st_ino;
      ::lib_rtl_output(LIB_RTL_VERBOSE,"DataFile::migrated> File:%s %s",
                       name().c_str(),mig ? "was migrated" : "is identical");
      return mig;
    }
  }
  ::lib_rtl_output(LIB_RTL_DEBUG,"DataFile::migrated> File:%s is not open!",name().c_str());  
  return false;
}

/// Open the data file
bool DataFile::open() {
  if ( m_fd ) close();
  m_fd = ::open(m_name.c_str(),O_RDONLY);
  ::memset(&m_stbuff,0,sizeof(m_stbuff));
  m_pointer = 0;
  if ( m_fd != -1 ) {
    if ( 0 == ::fstat(m_fd,&m_stbuff) ) {
      ::lib_rtl_output(LIB_RTL_DEBUG,"Successfully opened file:%s Inode:%u",
                       name().c_str(), m_stbuff.st_ino);
      return true;
    }
    ::close(m_fd);
  }
  m_fd = 0;
  m_pointer = 0;
  return false;
}

/// Close the data file
void DataFile::close() {
  if ( m_fd ) {
    ::close(m_fd);
  }
  ::memset(&m_stbuff,0,sizeof(m_stbuff));
  m_fd = 0;
  m_pointer = 0;
}

/// Move file descriptor to the end of the file
int DataFile::seek_end() {
  if ( !m_fd ) open();
  if ( m_fd ) ::lseek(m_fd,0,SEEK_END);
  return 1;
}

/// Process file netries line by line using functor object
int DataFile::scan(const DataProcessor& functor) {
  if ( !m_fd ) open();
  if ( m_fd ) {
    struct stat stbuff;
    int res = ::fstat(m_fd,&stbuff);
    if ( res < 0 ) {
      ::lib_rtl_output(LIB_RTL_ERROR,"Failed to stat %s",name().c_str());
      return 0;
    }
    if ( stbuff.st_size < m_pointer ) m_pointer = 0;
    size_t bytes = stbuff.st_size-m_pointer;
    if ( bytes > 0 ) {
      int count = 0;
      char* buff = new char[bytes+1];
      for(size_t rd=0; rd<bytes; ) {
        int cnt = ::read(m_fd,buff+rd,bytes-rd);
        if ( cnt > 0 ) rd += cnt;
        else if ( errno == EINTR ) continue;
        else break;
      }
      buff[bytes] = 0;
      for(char *p=buff, *e=buff+bytes; p<e; ) {
        char* q = ::strchr(p,'\n');
        if ( q ) {
          *q = 0;
          ++count;
          if ( !functor(p) ) break;
          p = q+1;
          continue;
        }
        break;
      }
      m_pointer += bytes;
      delete [] buff;
      ::lib_rtl_output(LIB_RTL_VERBOSE,"DataFile::scan> Read %d records from file:%s",count,name().c_str());
    }
    return 1;
  }
  return 0;
}

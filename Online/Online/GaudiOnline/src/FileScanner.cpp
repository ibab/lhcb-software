// $Id: FileScanner.cpp,v 1.7 2009-12-03 19:01:02 frankb Exp $
//====================================================================
//  FileScanner.cpp
//--------------------------------------------------------------------
//
//  Package    : GaudiOnline
//
//  Author     : M.Frank
//====================================================================
#include "GaudiOnline/FileScanner.h"
#include "Event/RawBank.h"
//#include "MBM/Consumer.h"
#include "MBM/mepdef.h"
#include "MDF/RawEventHelpers.h"
#include "MDF/MDFHeader.h"
#include "MDF/MEPEvent.h"
#include "RTL/readdir.h"
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#define O_BINARY 0
#endif

using namespace LHCb;
using namespace std;

static int file_read(int file, char* p, int len) {
  int tmp = 0;
  while (tmp < len)   {
    int sc = ::read(file, p + tmp, len - tmp);
    if (sc > 0)
      tmp += sc;
    else if (sc == 0)
      return 0;
    else return -1;
  }
  return 1;
}

static bool file_write(int file, const void* data, int len)   {
  const char* p = (const char*) data;
  int tmp = len;
  while (tmp > 0)  {
    int sc = ::write(file, p + len - tmp, tmp);
    if (sc > 0)
      tmp -= sc;
    else return false;
  }
  return true;
}

/// Standard Constructor
FileScanner::FileScanner()   {
  m_filePrefix = "Run_";
  m_deleteFiles = false;
}

/// Standard Destructor
FileScanner::~FileScanner()  {
}

/// Read broken hosts file and check if this node should process files
int FileScanner::isBrokenHost()  const   {
  string broken_hosts = m_brokenHostsFile;
  m_comment = "";
  if ( !broken_hosts.empty() ) {
    struct stat file;
    if ( 0 == ::stat(broken_hosts.c_str(),&file) ) {
      const std::string node = RTL::nodeNameShort();
      int   fd   = ::open(broken_hosts.c_str(),O_RDONLY);
      if ( -1 == fd )  {
        m_comment = "Failed to access broken node file:"+broken_hosts+" [Error ignored]";
        return -1;
      }
      char* data = new char[file.st_size+1];
      int rc = file_read(fd,data,file.st_size);
      if ( 1 == rc ) {
        data[file.st_size] = 0;
        for(int i=0; i<file.st_size; ++i)
          data[i] = ::toupper(data[i]);
        for(char* ptr=(char*)node.c_str(); *ptr; ++ptr)
          *ptr = ::toupper(*ptr);
        if ( ::strstr(data,node.c_str()) ) {
          m_comment = "Node is disabled and will not process any deferred files.";
          delete [] data;
          ::close(fd);
          return 1;
        }
      }
      delete [] data;
      ::close(fd);
    }
    else {
      m_comment = "Failed to access broken node file:"+broken_hosts+" [Error ignored]";
      return -1;
    }
  }
  return 0;
}

/// Scan directory for matching files
size_t FileScanner::scanFiles()   {
  m_files.clear();
  m_comment = "";
  DIR* dir = opendir(m_directory.c_str());
  if (dir)  {
    struct dirent *entry;
    bool take_all = (m_allowedRuns.size() > 0 && m_allowedRuns[0]=="*");
    while ((entry = ::readdir(dir)) != 0)    {
      if ( 0 != ::strncmp(entry->d_name,m_filePrefix.c_str(),m_filePrefix.length()) ) {
        continue;
      }
      else if ( !take_all )  {
        bool take_run = false;
        for(vector<string>::const_iterator i=m_allowedRuns.begin(); i!=m_allowedRuns.end(); ++i) {
          if ( ::strstr(entry->d_name,(*i).c_str()) == entry->d_name ) {
            take_run = true;
            break;
          }
        }
        if ( !take_run ) continue;
      }
      m_files.insert(m_directory + "/" + entry->d_name);
    }
    ::closedir(dir);
    return m_files.size();
  }
  const char* err = RTL::errorString();
  m_comment = "Failed to open directory:" + string(err ? err : "????????");
  return 0;
}

/// Open a new data file
int FileScanner::openFile()   {
  m_comment = "";
  while (m_files.size() > 0)  {
    set<string>::iterator i = m_files.begin();
    string fname = *i;
    m_files.erase(i);
    int fd = ::open(fname.c_str(), O_RDONLY | O_BINARY, S_IREAD);
    if ( -1 != fd )   {
      if (m_deleteFiles)   {
        int sc = ::unlink(fname.c_str());
        if (sc != 0)   {
          m_comment = "CANNOT UNLINK file: " + fname + ": " + RTL::errorString();
          //::exit(EBADF);
          ::close(fd);
          return 0;
        }
      }
      m_current = fname;
      m_comment = "Opened file: " + fname + " for deferred HLT processing";
      return fd;
    }
    m_comment = "FAILD to open file: " + fname + " for deferred HLT processing: "
      + RTL::errorString();
  }
  return 0;
}

/// Save remainder of currently read file
int FileScanner::safeRestOfFile(int file_handle)     {
  m_comment = "";
  if (file_handle)  {
    char buffer[10 * 1024];
    int cnt = 0, ret, fd = ::open(m_current.c_str(), O_CREAT | O_BINARY | O_WRONLY, 0777);
    if ( -1 == fd )    {
      m_comment = "CANNOT Create file: " + m_current + ": " + RTL::errorString();
      return -1;
    }
    while ((ret = ::read(file_handle, buffer, sizeof(buffer))) > 0)  {
      if (!file_write(fd, buffer, ret))      {
        m_comment = "CANNOT write file: " + m_current + ": " + RTL::errorString();
      }
      cnt += ret;
    }
    ::snprintf(buffer,sizeof(buffer),"Wrote %d bytes to file:%s fd:%d",cnt,m_current.c_str(),fd);
    m_comment = buffer;
    ::close(fd);
    ::close(file_handle);
    m_current = "";
    file_handle = 0;
    return 0;
  }
  return 0;
}

/// Read one record and filel the proper structures
int FileScanner::readEvent(int file_handle, void* baseAddr)  {
  if ( file_handle != 0 )  {
    int size_buf[3];
    int status = ::file_read(file_handle, (char*)size_buf, sizeof(size_buf));
    if (status <= 0)   {
      ::close(file_handle);
      file_handle = 0;
      m_current = "";
      return END_OF_DATA;
    }
    RawBank* b = (RawBank*)this;
    bool is_mdf   = size_buf[0] == size_buf[1] && size_buf[0] == size_buf[2];
    int  evt_size = size_buf[0];
    int  buf_size = evt_size + (is_mdf ? b->hdrSize() : sizeof(MEPEVENT) + sizeof(int));
    // Careful here: sizeof(int) MUST match me->sizeOf() !
    // The problem is that we do not (yet) have a valid data pointer!
    MBM::EventDesc& dsc = allocateSpace (buf_size);
    if ( dsc.data != 0 )   {
      char*  read_ptr = 0;
      size_t read_len = 0;
      if ( is_mdf ) {
        b = (RawBank*)dsc.data;
        b->setMagic();
        b->setType(RawBank::DAQ);
        b->setSize(MDFHeader::sizeOf(3));
        b->setVersion(DAQ_STATUS_BANK);
        read_ptr = b->begin<char>();
        ::memcpy(read_ptr,size_buf,sizeof(size_buf));
        read_ptr += sizeof(size_buf);
        read_len = evt_size-sizeof(size_buf);
        dsc.len  = evt_size+b->hdrSize();
        dsc.type = EVENT_TYPE_EVENT;
      }
      else {
        static int id = -1;
        MEPEVENT* e = (MEPEVENT*) dsc.data;
        MEPEvent* me = (MEPEvent*) e->data;
        me->setSize(evt_size);
        e->refCount = 1;
        e->evID = ++id;
        e->begin = long(e) - long(baseAddr);
        e->packing = -1;
        e->valid = 1;
        e->magic = mep_magic_pattern();
        for (size_t j = 0; j < MEP_MAX_PACKING; ++j)   {
          e->events[j].begin = 0;
          e->events[j].evID = 0;
          e->events[j].status = EVENT_TYPE_OK;
          e->events[j].signal = 0;
        }
        read_ptr = (char*)me->start();
        ::memcpy(read_ptr,size_buf+1,2*sizeof(size_buf[0]));
        read_ptr += 2*sizeof(size_buf[0]);
        read_len = me->size() - 2*sizeof(int);
        dsc.len  = sizeof(MEPEVENT) + me->sizeOf() + me->size();
        dsc.type = EVENT_TYPE_MEP;
      }
      status = ::file_read(file_handle,read_ptr, read_len);
      if (status <= 0)   {
        ::close(file_handle);
        file_handle = 0;
        m_current = "";
        return END_OF_DATA;
      }
      dsc.mask[0] = ~0x0;
      dsc.mask[1] = ~0x0;
      dsc.mask[2] = ~0x0;
      dsc.mask[3] = ~0x0;
      return EVENT_OK;
    }
  }
  return BAD_FILE_DESC;
}

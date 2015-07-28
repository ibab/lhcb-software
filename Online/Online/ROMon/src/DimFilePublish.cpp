// $Id: DimFilePublish.cpp,v 1.6 2010-10-14 08:15:47 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Readout monitoring in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/src/DimFilePublish.cpp,v 1.6 2010-10-14 08:15:47 frankb Exp $

#include <string>
#include <ctime>

/*
 *  Namespace declaration
 */
namespace ROMon {
  class DimFilePublish {
    int         m_service;
    int         m_delay;
    size_t      m_datalen;
    std::string m_file;
    size_t      m_len;
    void*       m_buff;
    time_t      m_modified;
  private:
    /// Private copy constructor
    DimFilePublish(const DimFilePublish&) {}
    /// Private assignment operator
    DimFilePublish& operator=(const DimFilePublish&) { return *this; }
  public:
    /// Standard constructor
    DimFilePublish(int argc, char** argv);
    /// Default destructor
    virtual ~DimFilePublish();
    /// Publish the collected information
    int monitor();
    /// Read file and return data pointers
    int readFile(void** buf, int* size);

    /// Data callback for DIM
    static void feedData(void* tag, void** buf, int* size, int* first);
  };
}


// C++ include files
#include <stdexcept>
#include <iostream>

// Framework includes
#include "dis.hxx"
#include "RTL/rtl.h"
#include <sys/stat.h>
#ifdef _WIN32
#include <io.h>
#define O_RDONLY _O_RDONLY
#endif
#include <fcntl.h>

using namespace ROMon;
using namespace std;

static void help() {
}

/// Standard constructor
DimFilePublish::DimFilePublish(int argc , char** argv)  
  : m_service(0), m_delay(100000), m_datalen(0), m_len(0), m_buff(0), m_modified(0)  
{
  RTL::CLI cli(argc, argv, help);
  string svc;
  cli.getopt("publish",1,svc);
  cli.getopt("file",1,m_file);
  cli.getopt("delay",1,m_delay);
  if ( !svc.empty() && !m_file.empty() )  {
    m_service = ::dis_add_service((char*)svc.c_str(),(char*)"C",0,0,feedData,(long)this);
    m_buff = ::malloc(1024);
    m_len = 1024;
  }
  else  {
    cout << "Unknown data -- cannot be published." << endl;
    throw runtime_error("Unknown data and unknwon service name -- cannot be published.");
  }
}

/// Default destructor
DimFilePublish::~DimFilePublish() {
  ::dis_remove_service(m_service);
  ::free(m_buff);
}

/// Read file and return data pointers
int DimFilePublish::readFile(void** pptr, int* size)    {
  struct stat buf;
  int sc = ::stat(m_file.c_str(),&buf);
  if ( sc == 0 ) {
    if ( buf.st_mtime > m_modified ) {
      m_datalen = buf.st_size;
      int fid = ::open(m_file.c_str(),O_RDONLY);
      if ( fid != -1 )  {
        if ( m_datalen > m_len ) {
          m_len = m_datalen+1;
          m_buff = ::realloc(m_buff,m_len);
        }
        sc = ::read(fid,m_buff,m_datalen);
        //cout << "Read data from file:" << m_datalen << " bytes. " << buf.st_mtime << " " << m_modified << " " << sc << endl;
        *((char*)m_buff+m_datalen) = 0;
        m_modified = buf.st_mtime;
        *size = m_datalen+1;
        *pptr = m_buff;
        ::close(fid);
        return 1;
      }
      m_datalen = 0;
    }
    //cout << "Publish existing data:" << m_datalen << " bytes. " << buf.st_mtime << " " << m_modified << endl;
    *size = m_datalen;
    *pptr = m_buff;
    return 1;
  }
  cout << "Data on file " << m_file << " Do not exist!" << endl;
  *size = 0;
  *pptr = m_buff;
  return 0;
}

/// Data callback for DIM
void DimFilePublish::feedData(void* tag, void** buf, int* size, int* /* first */) {
  DimFilePublish* h = *(DimFilePublish**)tag;
  h->readFile(buf,size);
}

/// Publish the collected information
int DimFilePublish::monitor() {
  bool run = true;
  while(run) {
    try {
      struct stat buf;
      int sc = ::stat(m_file.c_str(),&buf);
      if ( sc == 0 ) {
        if ( buf.st_mtime > m_modified ) {
          ::dis_update_service(m_service);
        }
      }
      ::lib_rtl_sleep(m_delay);
    }
    catch(exception& e) {
      cout << "DimFilePublish::monitor> Exception:" << e.what() << endl;
    }
    catch(...) {
      cout << "DimFilePublish::monitor> Unhandled exception" << endl;
    }
  }
  return 1;
}

extern "C" int romon_filepublish(int argc, char** argv) {
  string svc = RTL::processName();
  RTL::CLI cli(argc, argv, help);
  cli.getopt("service",1,svc);
  DimFilePublish romon(argc,argv);
  DimServer::start(svc.c_str());
  return romon.monitor();
}


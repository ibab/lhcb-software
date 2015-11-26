// $Id: StreamDescriptor.h,v 1.7 2008-03-03 20:05:04 frankb Exp $
//====================================================================
//  StreamDescriptor.h
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/StreamDescriptor.h,v 1.7 2008-03-03 20:05:04 frankb Exp $
#ifndef LHCb_STREAMDESCRIPTOR_H
#define LHCb_STREAMDESCRIPTOR_H 1

// C++ include files
#include <string>
#include "GaudiKernel/Kernel.h"

namespace Networking {
  struct in_addr;
}
/*
 *  LHCb namespace declaration
 */
namespace LHCb {

  // Forward declarations
  class PosixIO;

  /** @class StreamDescriptor StreamDescriptor.h LHCb/StreamDescriptor.h
    *
    * Description:
    *
    *
    * @author  M.Frank
    * @version 1.0
    */
  class StreamDescriptor {
  public:
    enum Constants  {
      NONE,
      DAQ_BUFFER,
      L1_BUFFER,
      ACCEPTED,
      REJECTED
    };
    /** @class Access
      */
    struct Access  {
      friend class StreamDescriptor;
      int       ioDesc;
      char      type;
      PosixIO*  ioFuncs;
    private:
      /// Fast functions: read buffer into memory
      int   (*m_read)     (const Access& con, void* buffer, int max_len);
      /// Fast functions: write buffer from memory
      bool  (*m_write)    (const Access& con, const void* buffer, int max_len);
      /// Fast functions: seek file location
      long long (*m_seek) (const Access& con, long long offset, int where);
    public:
      Access() : ioDesc(-1), type(0), ioFuncs(0), m_read(0), m_write(0), m_seek(0){}      
      bool write(const void* data, int len)  const      
      { return (*m_write)(*this,data, len);               }
      int  read(void* data, int len)  const             
      { return (*m_read)(*this,data, len);                }
      long long seek(long long offset, int where) const 
      { return (*m_seek)(*this,offset, where);            }
      Access& connect(const std::string& specs)         
      { return *this = StreamDescriptor::connect(specs);  }
      int close()                                       
      { return StreamDescriptor::close(*this);            }
    };

  protected:
    /// Maximal length of data block
    int       m_allocated;
    /// Length of data block
    int       m_length;
    /// Pointer to data block
    char*     m_data;

  public:
    /// Default constructor
    StreamDescriptor();
    /// Standard Destructor
    virtual ~StreamDescriptor();
    /// Check if stream descriptor actually has data
    bool hasData() const      {    return length()>0;  }
    /// Length of data block
    int length() const        {    return m_length;    }
    /// Set length of data block
    void setLength(int len)   {    m_length = len;     }
    /// Maximal allocated length
    int max_length()  const   {    return m_allocated; }
    /// Access to data block
    char* data() const        {    return m_data;      }
    /// Allocate data block
    char* allocate(int len);
    static void getInetConnection(const std::string& con, 
                                  std::string& host, 
                                  Networking::in_addr* ip, 
                                  unsigned short& port);
    static void getFileConnection(const std::string& con, 
                                  std::string& file, 
                                  std::string& proto);
    static Access connect(const std::string& specs);
    static Access bind(const std::string& specs);
    static Access accept(const Access& specs);
    static int close(Access& specs);
    static bool write(const Access& con, const void* data, int len)
    {   return (*con.m_write)(con, data, len);         }
    static int  read(const Access& con, void* data, int len)
    {   return (*con.m_read)(con, data, len);          }
    static long long seek(const Access& con, long long offset, int where)
    {   return (*con.m_seek)(con, offset, where);       }
  };
}
#endif // LHCb_STREAMDESCRIPTOR_H

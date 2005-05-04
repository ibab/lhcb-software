// $Id: StreamDescriptor.h,v 1.4 2005-05-04 17:10:22 frankb Exp $
//====================================================================
//	StreamDescriptor.h
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/StreamDescriptor.h,v 1.4 2005-05-04 17:10:22 frankb Exp $
#ifndef GAUDIONLINE_STREAMDESCRIPTOR_H
#define GAUDIONLINE_STREAMDESCRIPTOR_H 1

// C++ include files
#include <string>

namespace Networking {
  struct in_addr;
}
/*
 *  GaudiOnline namespace declaration
 */
namespace GaudiOnline {

  /** @class StreamDescriptor StreamDescriptor.h GaudiOnline/StreamDescriptor.h
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
      int  ioDesc;
      char type;
    private:
      /// Fast functions: read buffer into memory
      bool (*m_read)     (const Access& con, void* buffer, int max_len);
      /// Fast functions: read buffer length into memory
      bool (*m_read_len) (const Access& con, int& len);
      /// Fast functions: write buffer from memory
      bool (*m_write)    (const Access& con, const void* buffer, int max_len);
      /// Fast functions: set trigger decision
      bool (*m_set_decision)(const Access& con, const void* buffer, int len);
      /// fast functions: send trigger decision
      bool (*m_send_decision)(const Access& con);
    public:
      Access() : ioDesc(-1), m_read(0), m_read_len(0), m_write(0),
                 m_set_decision(0), m_send_decision(0) 
      {
      }      
    };
  protected:
    /// Maximal length of data block
    int       m_allocated;
    /// Length of data block
    int       m_length;
    /// Pointer to data block
    char*     m_data;
    /// Trigger decision
    int       m_decision;
    /// Data type (L1/DAQ)
    int       m_type;
    /// Pointer to current communication structure
    Access*   m_currentAccess;
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
    /// Set data type (L1/DAQ)
    void setType(int typ)     {    m_type = typ;       }
    /// Access to data type
    int  type() const         {    return m_type;      }
    /// Allocate data block
    char* allocate(int len);
    /// Change currenbt access descriptor
    void setCurrentAccess(Access* acc)  { m_currentAccess = acc; }
    /// Send decision
    int sendDecision() const;
    /// Set decision to be sent
    int setDecision(const void* data, int len) const;

    /// Access to datatype (by string)
    static int dataType(const std::string& typ);
    static void getInetConnection(const std::string& con, std::string& host, Networking::in_addr* ip, unsigned short& port);
    static void getFileConnection(const std::string& con, std::string& file);
    static Access connect(const std::string& specs);
    static Access bind(const std::string& specs);
    static Access accept(const Access& specs);
    static int close(Access& specs);
    static bool write(const Access& con, const void* data, int len)
    {   return (*con.m_write)(con, data, len);         }
    static bool read(const Access& con, void* data, int len)
    {   return (*con.m_read)(con, data, len);          }
    static bool readLength(const Access& con, int& len)
    {   return (*con.m_read_len)(con, len);            }
    static bool setDecision(const Access& con, const void* data, int len)
    {   return (*con.m_set_decision)(con, data, len);  }
    static bool sendDecision(const Access& con)
    {   return (*con.m_send_decision)(con);            }
  };
}
#endif // GAUDIONLINE_STREAMDESCRIPTOR_H

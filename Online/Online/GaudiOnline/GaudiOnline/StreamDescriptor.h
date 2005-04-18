// $Id: StreamDescriptor.h,v 1.1.1.1 2005-04-18 15:31:41 frankb Exp $
//====================================================================
//	StreamDescriptor.h
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiOnline/GaudiOnline/StreamDescriptor.h,v 1.1.1.1 2005-04-18 15:31:41 frankb Exp $
#ifndef GAUDIONLINE_STREAMDESCRIPTOR_H
#define GAUDIONLINE_STREAMDESCRIPTOR_H 1

// C++ include files
#include <string>

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
      bool (*m_read)     (Access& con, void* buffer, int max_len);
      /// Fast functions: read buffer length into memory
      bool (*m_read_len) (Access& con, int& len);
      /// Fast functions: write buffer from memory
      bool (*m_write)    (Access& con, const void* buffer, int max_len);
    public:
      Access() : ioDesc(-1), m_read(0), m_write(0), m_read_len(0) {}      
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
    static void getInetConnection(const std::string& con, std::string& host, unsigned int& ip, unsigned short& port);
    static void getFileConnection(const std::string& con, std::string& file);
    static Access connect(const std::string& specs);
    static Access bind(const std::string& specs);
    static Access accept(Access& specs);
    static int close(Access& specs);
    static bool write(Access& con, const void* data, int len)
    {   return (*con.m_write)(con, data, len);         }
    static bool read(Access& con, void* data, int len)
    {   return (*con.m_read)(con, data, len);          }
    static bool readLength(Access& con, int& len)
    {   return (*con.m_read_len)(con, len);            }
  };
}
#endif // GAUDIONLINE_STREAMDESCRIPTOR_H

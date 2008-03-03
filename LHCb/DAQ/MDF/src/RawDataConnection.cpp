// $Id: RawDataConnection.cpp,v 1.5 2008-03-03 20:05:04 frankb Exp $
#include "MDF/RawDataConnection.h"
#include "MDF/StreamDescriptor.h"
#include <iostream>
using namespace LHCb;
using namespace Gaudi;

/// Standard constructor
RawDataConnection::RawDataConnection(const IInterface* owner, const std::string& fname)
: IDataConnection(owner,fname)
{ //               01234567890123456789012345678901234567890
  // Check if FID: A82A3BD8-7ECB-DC11-8DC0-000423D950B0
  if ( fname.length() == 36 && fname[8]=='-'&&fname[13]=='-'&&fname[18]=='-'&&fname[23]=='-' ) {
    m_name = "FID:"+fname;
  }
  m_age = 0;
  m_bind.ioDesc = m_access.ioDesc = 0;
}

/// Standard destructor      
RawDataConnection::~RawDataConnection()   {
}

StatusCode RawDataConnection::connectRead()  {
  m_bind = StreamDescriptor::bind(m_pfn);
  if ( m_bind.ioDesc > 0 )   {
    m_access = StreamDescriptor::accept(m_bind);
    if ( m_access.ioDesc != 0 )  {
      resetAge();
      return StatusCode::SUCCESS;
    }
    StreamDescriptor::close(m_bind);
  }
  return StatusCode::FAILURE;
}

StatusCode RawDataConnection::connectWrite(IoType typ)  {
  switch(typ)  {
    case CREATE:
    case RECREATE:
      resetAge();
      m_access = StreamDescriptor::connect(m_pfn);
      return m_access.ioDesc == 0 ? StatusCode::FAILURE : StatusCode::SUCCESS;
    default:
      break;
  }
  return StatusCode::FAILURE;
}

/// Read raw byte buffer from input stream
StatusCode RawDataConnection::read(void* const data, size_t len)  {
  resetAge();
  int sc = StreamDescriptor::read(m_access,data,len);
  return sc==1 ? StatusCode::SUCCESS : StatusCode(sc);
}

/// Write raw byte buffer to output stream
StatusCode RawDataConnection::write(const void* data, int len)  {
  resetAge();
  return StreamDescriptor::write(m_access,data,len) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

/// Seek on the file described by ioDesc. Arguments as in ::seek()
long long int RawDataConnection::seek(long long int where, int origin)  {
  resetAge();
  return StreamDescriptor::seek(m_access,where,origin);
}

StatusCode RawDataConnection::disconnect()    {
  if ( m_access.ioDesc != 0 )
    StreamDescriptor::close(m_access);
  if ( m_bind.ioDesc != 0 )
    StreamDescriptor::close(m_bind);
  m_bind.ioDesc = m_access.ioDesc = 0;
  return StatusCode::SUCCESS;
}

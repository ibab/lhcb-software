// $Id: RawDataConnection.h,v 1.2 2008-04-10 09:58:42 cattanem Exp $
#ifndef MDF_RAWDATACONNECTION_H
#define MDF_RAWDATACONNECTION_H

// Framework include files
#include "GaudiUtils/IIODataManager.h" // for IDataConnection class definition
#include "MDF/StreamDescriptor.h"

/*
 *  LHCb namespace declaration
 */
namespace LHCb  {

  /** @class RawDataConnection RawDataConnection.h MDF/RawDataConnection.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *  @date    20/10/2007
    */
  class RawDataConnection : virtual public Gaudi::IDataConnection  {
  protected:
    StreamDescriptor::Access m_bind;
    StreamDescriptor::Access m_access;
  public:
    /// Standard constructor
    RawDataConnection(const IInterface* own, const std::string& nam);
    /// Standard destructor      
    virtual ~RawDataConnection();
    /// Check if connected to data source
    virtual bool isConnected() const        {     return m_access.ioDesc != 0; }
    /// Open data stream in read mode
    virtual StatusCode connectRead();
    /// Open data stream in write mode
    virtual StatusCode connectWrite(IoType typ);
    /// Release data stream
    virtual StatusCode disconnect();
    /// Read raw byte buffer from input stream
    virtual StatusCode read(void* const data, size_t len);
    /// Write raw byte buffer to output stream
    virtual StatusCode write(const void* data, int len);
    /// Seek on the file described by ioDesc. Arguments as in ::seek()
    virtual long long int seek(long long int where, int origin);
  };
}         // End namespace Gaudi
#endif    // MDF_RAWDATACONNECTION_H

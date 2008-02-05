// $Id: MDFWriter.h,v 1.10 2008-02-05 16:44:17 frankb Exp $
//  ====================================================================
//  MDFWriter.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef MDF_MDFWRITER_H
#define MDF_MDFWRITER_H

// Framework include files
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/Algorithm.h"
#include "MDF/MDFIO.h"

// Forward declarations
class TMD5;
namespace Gaudi { 
  class IIODataManager; 
  class IDataConnection;
}

/*
 *    LHCb namespace
 */
namespace LHCb    {

  class RawBank;

  /**@class MDFWriter 
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class MDFWriter : public Algorithm, protected MDFIO   {
  public:
    typedef Gaudi::IDataConnection Connection;

  protected:
    /// Reference to file manager service
    Gaudi::IIODataManager* m_ioMgr;
    /// Name of the IO manager service
    std::string   m_ioMgrName;
    /// Stream descriptor (Initializes networking)
    Connection*   m_connection;
    /// Input parameters for connection parameters
    std::string   m_connectParams;
    /// Compression algorithm identifier
    int           m_compress;
    /// Flag to create checksum
    int           m_genChecksum;
    /// Flag to create MD5 checksum
    bool          m_genMD5;
    /// Streambuffer to hold uncompressed data
    StreamBuffer  m_data;
    /// Pointer to checksum object
    TMD5*         m_md5;
    /// Location of the raw banks in the TES
    std::string   m_bankLocation;
    /// Bytes write count
    long long int m_bytesWritten;
    /// Input data type (like MDFIO::m_dataType)
    int           m_inputType;
    /// Internal setup (may not be called from sub-classes!
    void construct();

    /** Allocate space for IO buffer
      * @param[in] ioDesc Output IO descriptor       
      * @param[in] len    Total length of the data buffer
      *
      * @return  Pointer to allocated memory space
      */
    virtual std::pair<char*,int> getDataSpace(void* const /* ioDesc */, size_t len);

    /** Write byte buffer to output stream
      * @param[in] ioDesc Output IO descriptor       
      * @param[in] data   Data buffer to be streamed
      * @param[in] len    Length of the data buffer
      *
      * @return  Status code indicating success or failure.
      */
    virtual StatusCode writeBuffer(void* const ioDesc, const void* data, size_t len);

  public:

    /// Standard algorithm constructor
    MDFWriter(const std::string& name, ISvcLocator* pSvcLocator);

    /// Extended algorithm constructor
    MDFWriter(MDFIO::Writer_t typ, const std::string& name, ISvcLocator* pSvcLocator);

    /// Standard Destructor
    virtual ~MDFWriter();

    /// Algoritm overload: Initialize
    virtual StatusCode initialize();

    /// Algoritm overload: Finalize
    virtual StatusCode finalize();

    // Algoritm overload: Execute procedure
    virtual StatusCode execute();
  };
}      // End namespace LHCb
#endif // MDF_MDFWRITER_H

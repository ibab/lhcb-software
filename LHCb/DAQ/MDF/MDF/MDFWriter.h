// $Id: MDFWriter.h,v 1.4 2007-04-20 12:40:24 cattanem Exp $
//	====================================================================
//  MDFWriter.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef MDF_MDFWRITER_H
#define MDF_MDFWRITER_H

#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/Algorithm.h"
#include "MDF/StreamDescriptor.h"
#include "MDF/MDFIO.h"

class TMD5;

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

  protected:
    typedef LHCb::StreamDescriptor   Descriptor;
    typedef Descriptor::Access       Access;

    /// Stream descriptor (Initializes networking)
    Access        m_connection;
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
    /// Input data type (banks from TES=1, compressed data from address = 2)
    int           m_dataType;

    /// Internal setup (may not be called from sub-classes!
    void construct();

    /** Allocate space for IO buffer
      * @param[in] ioDesc Output IO descriptor       
      * @param[in] len    Total length of the data buffer
      *
      * @return  Pointer to allocated memory space
      */
    virtual std::pair<char*,int> getDataSpace(void* const /* ioDesc */, size_t len)  {
      m_data.reserve(len);
      return std::pair<char*,int>(m_data.data(), m_data.size());
    }

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

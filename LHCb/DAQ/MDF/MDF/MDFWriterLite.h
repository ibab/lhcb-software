// $Id: MDFWriterLite.h,v 1.9 2008-04-10 09:58:42 cattanem Exp $
//  ====================================================================
//  MDFWriterLite.h
//  --------------------------------------------------------------------
//
//  Author    : Sai Suman & Niko Neufeld 
//
//  ====================================================================
#ifndef MDF_MDFWRITERLITE_H
#define MDF_MDFWRITERLITE_H

#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/Algorithm.h"
#include "MDF/StreamDescriptor.h"
#include "MDF/MDFIO.h"

class TMD5;

/*
 *    LHCb namespace
 */
namespace LHCb    {

  /**@class MDFWriterLite 
    *
    *
    * @author:  S. Suman & N. Neufeld
    * @version: 1.0
    */
  class MDFWriterLite : public Algorithm, protected MDFIO   {

  private:
    /// Number of bytes written out to the file so far.
    size_t m_bytesWritten;
    /// Number of events written out to the file so far. 
    size_t m_eventsWritten;
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
    /// Maximum size of each file (after which a new file will be written)
    size_t    m_maxFileSizeKB;
    /// Maximum number of events in a file (after which a new file will be written)
    size_t        m_maxFileEvents;
    /// Location of the raw banks in the TES
    std::string    m_bankLocation;


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
    MDFWriterLite(const std::string& name, ISvcLocator* pSvcLocator);

    /// Extended algorithm constructor
    MDFWriterLite(MDFIO::Writer_t typ, const std::string& name, ISvcLocator* pSvcLocator);

    /// Standard Destructor
    virtual ~MDFWriterLite();

    /// Algoritm overload: Initialize
    virtual StatusCode initialize();

    /// Algoritm overload: Finalize
    virtual StatusCode finalize();

    /// Algoritm overload: Execute procedure
    virtual StatusCode execute();
  };
}      // End namespace LHCb
#endif // MDF_MDFWRITERLITE_H

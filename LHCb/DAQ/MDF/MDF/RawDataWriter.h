// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/RawDataWriter.h,v 1.1 2006-03-17 17:24:47 frankb Exp $
//	====================================================================
//  RawDataWriter.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef MDF_RAWDATAWRITER_H
#define MDF_RAWDATAWRITER_H

#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/Algorithm.h"
#include "MDF/StreamDescriptor.h"

/*
 *    LHCb namespace
 */
namespace LHCb    {

  class MDFHeader;

  /**@class RawDataWriter 
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class RawDataWriter : public Algorithm   {

  protected:
    typedef LHCb::StreamDescriptor   Descriptor;
    typedef Descriptor::Access       Access;

    /// Property: Input parameters for connection parameters
    std::string   m_connectParams;
    /// Property: Compression algorithm identifier
    int           m_compress;
    /// Property: Flag to create checksum
    int           m_genChecksum;

    /// Stream descriptor (Initializes networking)
    Access        m_connection;
    /// Streambuffer to hold uncompressed data
    StreamBuffer  m_data;
    /// Streambuffer to hold compressed data
    StreamBuffer  m_tmp;

    long long int m_bytesWritten;
    int           m_kbytesPerFile;
    int           m_fileNo;
    std::string   m_currStream;

    /// Access MDF record information form the data
    StatusCode getHeaderInfo(DataObject* pObj, int& evType, int& trNumb, unsigned int mask[4]);

  public:

    /// Standard algorithm constructor
    RawDataWriter(const std::string& name, ISvcLocator* pSvcLocator);

    /// Standard Destructor
    virtual ~RawDataWriter()    {}

    /// Initialize
    virtual StatusCode initialize();

    /// Finalize
    virtual StatusCode finalize();

    /// Write data
    StatusCode writeData(const MDFHeader& header, const void* data);

    /// Execute procedure
    virtual StatusCode execute();

    /// Change file after a given number of bytes were written.
    /**   The file name follow the pattern: 
      *     object.Connection = "/disk0/dir1/dir2/run34/Outputfile_%03d.mdf"
      *   Where %FNO gets replaced by a increasing integer of the form 000, 001, 002, ...
      *
      *   @author  M.Frank
      *   @version 1.0
      */
    StatusCode switchOutputFile();

    /// Change output file
    StatusCode switchOutputFile(Access& connection, const std::string& fname);

    /// Write byte buffer to output stream
    /** @param data   [IN]    Data buffer to be streamed
      * @param len    [IN]    Length of the data buffer
      *
      * @return  Status code indicating success or failure.
      */
    StatusCode writeBuffer(const void* data, size_t len);

    /// Write MDF record from serialization buffer
    StatusCode writeRawBuffer(size_t len, int evType, int trNumber, unsigned int trMask[4]);
  };
}      // End namespace LHCb
#endif // MDF_RAWDATAWRITER_H

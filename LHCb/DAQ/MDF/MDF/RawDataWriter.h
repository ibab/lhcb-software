// $Id: RawDataWriter.h,v 1.7 2008-02-05 16:44:18 frankb Exp $
//  ====================================================================
//  RawDataWriter.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef MDF_RAWDATAWRITER_H
#define MDF_RAWDATAWRITER_H

// Framework include files
#include "GaudiKernel/Algorithm.h"
#include "MDF/MDFIO.h"

// Forward declarations
namespace Gaudi {  class IIODataManager;     }

/*
 *    LHCb namespace
 */
namespace LHCb    {

  class MDFHeader;

  /** @class RawDataFile
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class RawDataFile  {
    /// Pointer to checksum object
    void*               m_md5;
    /// Stream descriptor (Initializes networking)
    RawDataConnection*  m_connection;
    long long int       m_bytesWritten;
    std::string         m_name;
    std::string         m_md5Sum;
    unsigned int        m_runNumber;
    unsigned int        m_firstOrbit;
    unsigned int        m_lastOrbit;
    time_t              m_closeStamp;
    unsigned int        m_eventCounter;
    /// Temporary Streambuffer to hold uncompressed data
    StreamBuffer        m_data;
    /// Reference to file manager service
    Gaudi::IIODataManager* m_ioMgr;
    IInterface*         m_owner;
  public:
    /// Initializing constructor
    RawDataFile(Gaudi::IIODataManager* mgr, 
                IInterface*            owner,
                const std::string&     fname,
                bool                   md5,
                unsigned int           run_no,
                unsigned int           orb);
    /// Standard destructor
    virtual ~RawDataFile();
    /// Write byte buffer to output stream
    StatusCode writeBuffer(const void* data, size_t len);
    /// Open file
    StatusCode open();
    /// Close file
    StatusCode close();
    /** Allocate space for IO buffer
      * @param[in] len    Total length of the data buffer
      *
      * @return  Pointer to allocated memory space, length
      */
    std::pair<char*,int> getDataSpace(size_t len);
    /// Set last valid orbit. Only lower orbits are still accepted
    void setLastOrbit(unsigned int orbit);
    const std::string& md5Sum() const       {  return m_md5Sum;       }
    const std::string& name() const         {  return m_name;         }
    long long int bytesWritten() const      {  return m_bytesWritten; }
    unsigned int runNumber() const          {  return m_runNumber;    }
    unsigned int firstOrbit() const         {  return m_firstOrbit;   }
    unsigned int lastOrbit() const          {  return m_lastOrbit;    }
    time_t closeStamp() const               {  return m_closeStamp;   }
    unsigned int& eventCounter()            {  return m_eventCounter; }
  };

  /** @class RawDataWriter 
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class RawDataWriter : public Algorithm, protected MDFIO   {

  protected:
    typedef std::vector<RawDataFile*>  Connections;
    /// Counter of bytes written
    long long int m_bytesWritten;
    /// Number of mega bytes to be written to file
    int           m_MbytesPerFile;
    /// File number counter
    int           m_fileNo;
    /// Connection array
    Connections   m_connections;
    /// Input parameters for connection parameters
    std::string   m_volume, m_stream, m_connect, m_connectParams;
    /// Compression algorithm identifier
    int           m_compress;
    /// Flag to create checksum
    int           m_genChecksum;
    /// Flag to create MD5 checksum
    bool          m_genMD5;
    /// Timeout before really closing the file
    int           m_closeTMO;
    /// Location of the raw banks in the TES
    std::string    m_bankLocation;
    /// Reference to file manager service
    Gaudi::IIODataManager* m_ioMgr;
    /// Name of the IO manager service
    std::string   m_ioMgrName;

  protected:
    /// Access output file according to runnumber and orbit
    RawDataFile*  outputFile(unsigned int run_no, unsigned int orbit);
    /// Submit information to register file to run database
    virtual StatusCode submitRunDbOpenInfo(RawDataFile* f, bool blocking);
    /// Submit information about closed file to run database
    virtual StatusCode submitRunDbCloseInfo(RawDataFile* f, bool blocking);

  public:

    /// Standard algorithm constructor
    RawDataWriter(const std::string& name, ISvcLocator* pSvcLocator);

    /// Standard Destructor
    virtual ~RawDataWriter()    {}

    /// Initialize
    virtual StatusCode initialize();

    /// Finalize
    virtual StatusCode finalize();

    /** Allocate space for IO buffer
      * @param[in] ioDesc    Output IO descriptor       
      * @param[in] len       Total length of the data buffer
      *
      * @return  Pointer to allocated memory space
      */
    virtual std::pair<char*,int> getDataSpace(void* const ioDesc, size_t len);

    /** Write byte buffer to output stream
      * @param[in] data    Data buffer to be streamed
      * @param[in] len     Length of the data buffer
      *
      * @return  Status code indicating success or failure.
      */
    StatusCode writeBuffer(void* const iodesc, const void* data, size_t len);

    /// Algorithm overlay: Execute procedure
    StatusCode execute();
  };
}      // End namespace LHCb
#endif // MDF_RAWDATAWRITER_H

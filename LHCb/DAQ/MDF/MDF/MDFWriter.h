// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/MDFWriter.h,v 1.1 2005-12-20 16:33:38 frankb Exp $
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

/*
 *    LHCb namespace
 */
namespace LHCb    {

  class MDFHeader;

  /**@class MDFWriter 
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class MDFWriter : public Algorithm   {

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
    /// Streambuffer to hold uncompressed data
    StreamBuffer  m_data;
    /// Streambuffer to hold compressed data
    StreamBuffer  m_tmp;

  public:

    /// Standard algorithm constructor
    MDFWriter(const std::string& name, ISvcLocator* pSvcLocator);

    /// Standard Destructor
    virtual ~MDFWriter()    {}

    /// Initialize
    virtual StatusCode initialize();

    /// Finalize
    virtual StatusCode finalize();

    /// Write data
    StatusCode writeData(const MDFHeader& header, const void* data);

    // Execute procedure
    virtual StatusCode execute();
  };
}      // End namespace LHCb
#endif // MDF_MDFWRITER_H

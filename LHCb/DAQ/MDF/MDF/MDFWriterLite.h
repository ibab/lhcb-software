// $Id: MDFWriterLite.h,v 1.11 2010-05-19 06:48:45 kakiba Exp $
//  ====================================================================
//  MDFWriterLite.h
//  --------------------------------------------------------------------
//
//  Author    : Sai Suman & Niko Neufeld 
//
//  ====================================================================
#ifndef MDF_MDFWRITERLITE_H
#define MDF_MDFWRITERLITE_H

#include "MDF/MDFWriter.h"

/*
 *    LHCb namespace
 */
namespace LHCb    {

  /**@class MDFWriterLite 
    *
    * @author:  K. Akiba
    * @author:  S. Suman & N. Neufeld
    * @author   M.Frank
    * @version: 1.1
    */
  class MDFWriterLite : public MDFWriter   {
  protected:
    /// Time stamp of last file open
    time_t m_lastOpen;
    /// Number of events written out to the file so far. 
    size_t m_eventsWritten;
    /// Maximum size of each file (after which a new file will be written) 
    /// will be ignored if m_timeOut != 0
    size_t m_maxFileSizeKB;
    /// Maximum number of events in a file (after which a new file will be written)
    /// will be ignored if m_timeOut != 0
    size_t m_maxFileEvents;
    /// Time stamp of last event written
    time_t m_lastEventWritten;
    /// Timeout value to open a new file (in seconds)
    time_t m_timeOut;
    /// Transform file name in presence of catalogs
    virtual std::string getConnection(const std::string& org_conn);
    
  public:
    /// Standard algorithm constructor
    MDFWriterLite(const std::string& name, ISvcLocator* pSvcLocator);

    /// Standard Destructor
    virtual ~MDFWriterLite();

    /// Algoritihm overload: Execute & initialize procedure
    virtual StatusCode execute();
    virtual StatusCode initialize();
  };
}      // End namespace LHCb
#endif // MDF_MDFWRITERLITE_H

// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RunDatabase/RunDatabase/RunDbDataWriter.h,v 1.2 2006-07-04 17:04:33 frankb Exp $
//	====================================================================
//  RunDbDataWriter.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef MDF_RUNDBDATAWRITER_H
#define MDF_RUNDBDATAWRITER_H

#include "MDF/RawDataWriter.h"

/*
 *    LHCb namespace
 */
namespace LHCb    {

  class RawDataFile;
  class IRunDatabaseWriter;

  /** @class RunDbDataWriter 
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class RunDbDataWriter : public RawDataWriter  {

  protected:
    /// Pointer to run database interface
    IRunDatabaseWriter* m_runDb;
    /// Submit information to register file to run database
    virtual StatusCode submitRunDbOpenInfo(RawDataFile* f, bool blocking);
    /// Submit information about closed file to run database
    virtual StatusCode submitRunDbCloseInfo(RawDataFile* f, bool blocking);

  public:

    /// Standard algorithm constructor
    RunDbDataWriter(const std::string& name, ISvcLocator* pSvcLocator);

    /// Standard Destructor
    virtual ~RunDbDataWriter()    {}

    /// Initialize
    virtual StatusCode initialize();

    /// Finalize
    virtual StatusCode finalize();
  };
}      // End namespace LHCb
#endif // MDF_RUNDBDATAWRITER_H

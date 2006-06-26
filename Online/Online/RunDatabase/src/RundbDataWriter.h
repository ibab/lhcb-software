// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RunDatabase/src/RundbDataWriter.h,v 1.1 2006-06-26 08:48:12 frankb Exp $
//	====================================================================
//  OnlineDataWriter.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef MDF_OnlineDataWriter_H
#define MDF_OnlineDataWriter_H

#include "MDF/RawDataWriter.h"

/*
 *    LHCb namespace
 */
namespace LHCb    {

  class RawDataFile;
  class IRunDatabaseWriter;

  /** @class OnlineDataWriter 
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class OnlineDataWriter : public RawDataWriter  {

  protected:
    /// Pointer to run database interface
    IRunDatabaseWriter* m_runDb;
    /// Submit information to register file to run database
    virtual StatusCode submitRunDbOpenInfo(RawDataFile* f, bool blocking);
    /// Submit information about closed file to run database
    virtual StatusCode submitRunDbCloseInfo(RawDataFile* f, bool blocking);

  public:

    /// Standard algorithm constructor
    OnlineDataWriter(const std::string& name, ISvcLocator* pSvcLocator);

    /// Standard Destructor
    virtual ~OnlineDataWriter()    {}

    /// Initialize
    virtual StatusCode initialize();

    /// Finalize
    virtual StatusCode finalize();
  };
}      // End namespace LHCb
#endif // MDF_OnlineDataWriter_H

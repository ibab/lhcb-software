// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/MIFWriter.h,v 1.4 2007-11-19 19:27:32 frankb Exp $
//	====================================================================
//  MIFWriter.h
//	--------------------------------------------------------------------
//
//	Author    : Markus Frank
//
//	====================================================================
#ifndef MDF_MIFWRITER_H
#define MDF_MIFWRITER_H

#include "MDF/StreamDescriptor.h"
#include "GaudiKernel/Algorithm.h"
#include <map>

namespace Gaudi { 
  class IIODataManager; 
  class IDataConnection;
}

/*
 *    LHCb namespace
 */
namespace LHCb    {

  /**@class MIFWriter 
    *
    *
    * @author:  M.Frank
    * @version: 1.0
    */
  class MIFWriter : public Algorithm   {
  public:
    typedef Gaudi::IDataConnection Connection;

  protected:
    typedef std::map<int,std::string> FidMap;

    FidMap m_fidMap;
    /// Reference to file manager service
    Gaudi::IIODataManager* m_ioMgr;
    /// Name of the IO manager service
    std::string   m_ioMgrName;
    /// Stream descriptor (Initializes networking)
    Connection*   m_connection;
    std::string   m_connectParams;

  public:

    /// Standard algorithm constructor
    MIFWriter(const std::string& name, ISvcLocator* pSvcLocator);

    /// Standard Destructor
    virtual ~MIFWriter();

    /// Initialize
    virtual StatusCode initialize();

    /// Finalize
    virtual StatusCode finalize();

    // Execute procedure
    virtual StatusCode execute();
  };
}
#endif // MDF_MIFWRITER_H

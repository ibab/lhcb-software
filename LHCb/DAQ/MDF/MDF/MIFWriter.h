// $Header: /afs/cern.ch/project/cvs/reps/lhcb/DAQ/MDF/MDF/MIFWriter.h,v 1.2 2006-03-17 17:23:56 frankb Exp $
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
    typedef std::map<std::string, int> FidMap;
    FidMap m_fidMap;
  private:
    typedef LHCb::StreamDescriptor   Descriptor;
    typedef Descriptor::Access       Access;
    /// Stream descriptor (Initializes networking)
    Access        m_connection;
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

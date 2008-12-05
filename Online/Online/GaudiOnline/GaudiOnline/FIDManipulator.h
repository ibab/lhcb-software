// $Id: FIDManipulator.h,v 1.1 2008-12-05 19:28:09 frankb Exp $
// Include files 
//-----------------------------------------------------------------------------
// Implementation file for class : Fileidmanipulator
//
// 2008-03-03 : Albert Puig Navarro (albert.puig@cern.ch)
// 2008-11-20 : Markus Frank
//-----------------------------------------------------------------------------
#ifndef GAUDIONLINE_FIDMANIPULATOR_H
#define GAUDIONLINE_FIDMANIPULATOR_H

#include "GaudiKernel/StatusCode.h"
#include <string>
#include <algorithm>

// Forward declarations
class IDataProviderSvc;
class IMessageSvc;

/*
 *   LHCb namespace
 */
namespace LHCb {
  // Forward declarations
  class RawBank;

  /** @class FIDManipulator FIDManipulator.h
  *  
  *  Note: This is a pure online algorithm and only works for MDF I/O.
  *        Don't even try to use it with POOL etc.
  *
  *  @author Albert Puig Navarro (albert.puig@cern.ch)
  *  @author M.Frank
  *  @date   2008-03-03
  */
  class FIDManipulator {
    IDataProviderSvc* m_dp;
    IMessageSvc*      m_msg;
    /// Property: Location of the raw event in the transient datastore
    std::string       m_location;
    /// Property: input data type
    int               m_type;
    /// Flga for debugging
    bool              m_debug;
    /// Issue error message
    StatusCode error(const std::string& msg);

  public:
    /// Standard constructor
    FIDManipulator(const std::string& loc, int typ, IMessageSvc* m, IDataProviderSvc* d);
    /// Standard destructor
    virtual ~FIDManipulator();
    /// Set debug flag
    void setDebug(bool val) { m_debug = val; }
    /// Add fileID bank
    StatusCode add(int id, const std::string& guid);
    /// Remove fileID bank
    StatusCode remove();
    /// Print FID bank content
    StatusCode print();
    /// Find the FID bank in the raw event structure
    std::pair<LHCb::RawBank*,void*> getBank();
  };
}
#endif  // GAUDIONLINE_FIDMANIPULATOR_H

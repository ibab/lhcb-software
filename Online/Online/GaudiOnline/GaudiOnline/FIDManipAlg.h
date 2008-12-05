// $Id: FIDManipAlg.h,v 1.1 2008-12-05 19:28:09 frankb Exp $
// Include files 
//-----------------------------------------------------------------------------
// Implementation file for class : Fileidmanipulator
//
// 2008-03-03 : Albert Puig Navarro (albert.puig@cern.ch)
// 2008-11-20 : Markus Frank
//-----------------------------------------------------------------------------
#ifndef GAUDIONLINE_FIDMANIPALG_H
#define GAUDIONLINE_FIDMANIPALG_H

// Include files from Gaudi
#include "GaudiKernel/Algorithm.h"

/*
 *   LHCb namespace
 */
namespace LHCb {

  // Forward declarations
  class RawBank;

  /** @class FIDManipAlg FIDManipAlg.h
  *  
  *  Note: This is a pure online algorithm and only works for MDF I/O.
  *        Don't even try to use it with POOL etc.
  *
  *  @author Albert Puig Navarro (albert.puig@cern.ch)
  *  @author M.Frank
  *  @date   2008-03-03
  */
  class FIDManipAlg : public Algorithm {
  public:
    enum Action { ADD=1, REMOVE, DUMP };
  protected:
    /// Property: remove/add file id bank
    int          m_action;
    /// Property: Location of the raw event in the transient datastore
    std::string  m_location;
    /// Property: input data type
    int          m_type;
    /// Property: Debug flag
    int          m_debug;
    /// Property printout frequency
    int          m_printFreq;
    /// Print frequence counter
    int          m_printCnt;

  protected:
  public: 
    /// Standard constructor
    FIDManipAlg(const std::string& nam, ISvcLocator* pSvc);
    /// Standard destructor
    virtual ~FIDManipAlg()  {}
    /// Algorithm execution
    virtual StatusCode initialize();
    /// Algorithm execution
    virtual StatusCode execute();
  };
}
#endif // GAUDIONLINE_FIDMANIPALG_H

// $Id: PreDV.h,v 1.1 2004-03-11 13:02:14 pkoppenb Exp $
#ifndef PREDV_H 
#define PREDV_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

/** @class PreDV PreDV.h
 *  Algorithm to be run before any DVAlgorithm(s)
 *  Prepares container for selection summary objects
 *
 *  @author Hugo Ruiz Perez
 *  @date   2004-03-02
 */
class PreDV : public Algorithm {
public: 
  /// Standard constructor
  PreDV( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PreDV( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  // Keep previous SelResults in 'selections' container when writting DST
  bool m_keepSelHistory;
  
private:

  /// Only warn at first event
  bool m_warned;

};
#endif // PREDV_H

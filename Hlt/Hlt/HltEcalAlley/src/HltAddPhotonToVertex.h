// $Id: HltAddPhotonToVertex.h,v 1.1.1.1 2008-07-16 16:54:04 witekma Exp $
#ifndef HLTADDPHOTONTOVERTEX_H 
#define HLTADDPHOTONTOVERTEX_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
//#include "HltBase/IBiFunctionFactory.h"
//#include "HltFunctions.h"

/** @class HltAddPhotonToVertex HltAddPhotonToVertex.h
 *  
 *
 *  @author Hugo Ruiz
 *  @date   2006-02-21
 */


class HltAddPhotonToVertex : public HltAlgorithm {
 public: 

  typedef IBiFunctionFactory<LHCb::Track,LHCb::Track> IBiTrackFactory;
  
  /// Standard constructor
  HltAddPhotonToVertex( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
    
 private:

};
#endif // HLTADDPHOTONTOVERTEX_H 


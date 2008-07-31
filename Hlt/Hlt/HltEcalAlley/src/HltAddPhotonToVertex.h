// $Id: HltAddPhotonToVertex.h,v 1.2 2008-07-31 11:55:52 graven Exp $
#ifndef HLTADDPHOTONTOVERTEX_H 
#define HLTADDPHOTONTOVERTEX_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"

/** @class HltAddPhotonToVertex HltAddPhotonToVertex.h
 *  
 *
 *  @author Hugo Ruiz
 *  @date   2006-02-21
 */


class HltAddPhotonToVertex : public HltAlgorithm {
 public: 

  /// Standard constructor
  HltAddPhotonToVertex( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
    
 private:
  Hlt::SelectionContainer3<LHCb::RecVertex, LHCb::RecVertex, LHCb::Track>  m_selections;


};
#endif // HLTADDPHOTONTOVERTEX_H 


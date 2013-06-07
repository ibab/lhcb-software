#ifndef CALOCOSMICSTRACKALG_H 
#define CALOCOSMICSTRACKALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
// from LHCb
#include "CaloInterfaces/ICaloCosmicsTrackTool.h"       


/** @class CaloCosmicsTrackAlg CaloCosmicsTrackAlg.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-05-17
 */
class CaloCosmicsTrackAlg : public GaudiTupleAlg {
public: 
  /// Standard constructor
  CaloCosmicsTrackAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloCosmicsTrackAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  ICaloCosmicsTrackTool* m_caloTrack;

  std::string m_trackToolType;
  std::string m_forward;
  std::string m_backward;
  bool m_monitor;
  

};
#endif // CALOCOSMICSTRACKALG_H

// $Id: ResetPVLocation.h,v 1.1 2004-10-27 13:49:59 pkoppenb Exp $
#ifndef RESETPVLOCATION_H 
#define RESETPVLOCATION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// from DaVinci
#include "DaVinciTools/IPVLocator.h" 


/** @class ResetPVLocation ResetPVLocation.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-10-27
 */
class ResetPVLocation : public GaudiAlgorithm {
public: 
  /// Standard constructor
  ResetPVLocation( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ResetPVLocation( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_PVLocation ;  ///< PV location
  IPVLocator* m_PVLocator ;   ///< PV locator
  bool m_reset ; ///< reset to offline
  
};
#endif // RESETPVLOCATION_H

// $Id: CheckPV.h,v 1.1 2004-11-18 07:56:51 pkoppenb Exp $
#ifndef CHECKPV_H 
#define CHECKPV_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "DaVinciTools/DVAlgorithm.h"


/** @class CheckPV CheckPV.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-11-15
 */
class CheckPV : public DVAlgorithm {
public: 
  /// Standard constructor
  CheckPV( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CheckPV( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IPVLocator* m_PVLocator ;   ///< 

};
#endif // CHECKPV_H

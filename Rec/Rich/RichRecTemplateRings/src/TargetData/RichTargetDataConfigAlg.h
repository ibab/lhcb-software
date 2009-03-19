// $Id: RichTargetDataConfigAlg.h,v 1.2 2009-03-19 17:13:31 seaso Exp $
#ifndef RICHTARGETDATACONFIGALG_H 
#define RICHTARGETDATACONFIGALG_H 1

// Include files
// from Gaudi
#include "RichRingRec/RichRingRecAlgBase.h"

// Event
#include "Event/RichRecTrack.h"

//Interfaces
#include "RichRecBase/IRichTrackSelector.h"

/** @class RichTargetDataConfigAlg RichTargetDataConfigAlg.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-05-14
 */
class RichTargetDataConfigAlg : public RichRingRecAlgBase {
public: 
  /// Standard constructor
  RichTargetDataConfigAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichTargetDataConfigAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  StatusCode AcquireTargetRichHitInfo();
  StatusCode AcquireTargetTrackInfo();
  
protected:

private:

 const ITrackSelector * m_trSelector; ///< Track selector
 bool m_selectTracksAboveMomentumThreshold; // select tracks above the
                                             // minimum Cherenkov threshold.

};
#endif // RICHTARGETDATACONFIGALG_H

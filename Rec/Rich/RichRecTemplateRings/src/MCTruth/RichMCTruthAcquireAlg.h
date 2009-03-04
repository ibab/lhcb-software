// $Id: RichMCTruthAcquireAlg.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef MCTRUTH_RICHMCTRUTHACQUIREALG_H
#define MCTRUTH_RICHMCTRUTHACQUIREALG_H 1

// Include files
// from Gaudi

#include "RichRingRec/RichRingRecAlgBase.h"
// Event
#include "Event/RichRecTrack.h"
//Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "MCInterfaces/IRichMCTruthTool.h"
//#include "RichRecBase/IRichRecMCTruthTool.h"
//#include "RichKernel/IRichMCTruthTool.h"
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichSmartIDTool.h"


/** @class RichMCTruthAcquireAlg RichMCTruthAcquireAlg.h MCTruth/RichMCTruthAcquireAlg.h
 *
 *
 *  @author Sajan EASO
 *  @date   2007-06-26
 */
class RichMCTruthAcquireAlg : public RichRingRecAlgBase {
public:
  /// Standard constructor
  RichMCTruthAcquireAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichMCTruthAcquireAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  StatusCode AcquireMCTruthInfo();
  VD   getTrueRingInfo(int rad , VI ahitlist ,const Gaudi::XYZPoint & pdPointLocal,
                       double aMinradius, double aMaxradius );
  StatusCode CompareReconRadiusWithMC();
  StatusCode CompareMassFromRadiators();

  StatusCode StoreRingNtup();

protected:

private:


  /// Pointer to RichRecMCTruthTool interface
  const Rich::Rec::MC::IMCTruthTool * m_richRecMCTruth;
  const Rich::MC::IMCTruthTool * m_mcTool;

  const ITrackSelector * m_trSelector; ///< Track selector

};
#endif // MCTRUTH_RICHMCTRUTHACQUIREALG_H

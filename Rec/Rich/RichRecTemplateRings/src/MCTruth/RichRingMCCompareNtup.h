// $Id: RichRingMCCompareNtup.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef MCTRUTH_RICHRINGMCCOMPARENTUP_H 
#define MCTRUTH_RICHRINGMCCOMPARENTUP_H 1

// Include files
// from Gaudi

#include "RichRingRec/RichRingRecTupleAlgBase.h"
// Event
#include "Event/RichRecTrack.h"
#include "RichRecBase/IRichTrackSelector.h"


/** @class RichRingMCCompareNtup RichRingMCCompareNtup.h MCTruth/RichRingMCCompareNtup.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-29
 */
class RichRingMCCompareNtup : public RichRingRecTupleAlgBase {
public: 
  /// Standard constructor
  RichRingMCCompareNtup( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichRingMCCompareNtup( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  void  RingReconNtup();
  void  RingGlobalReconNtup();
  void RingGlobalReconComparePrint();
  
  
protected:

private:
const ITrackSelector * m_trSelector; ///< Track selector

 

};
#endif // MCTRUTH_RICHRINGMCCOMPARENTUP_H

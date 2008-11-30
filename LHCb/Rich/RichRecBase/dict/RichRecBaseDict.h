#ifndef DICT_RICHRECBASEDICT_H 
#define DICT_RICHRECBASEDICT_H 1

#include "Event/RichRecSegment.h"
#include "Event/RichRecPixel.h"
#include "Event/RichRecTrack.h"
#include "Event/RichRecRing.h"
#include "Event/RichRecPhoton.h"
#include "Event/RichRecStatus.h"

#include "RichRecBase/RichRecPhotonKey.h"
#include "RichRecBase/RichRadCorrLocalPositions.h"
#include "RichRecBase/FastRingFitter.h"

#include "RichRecBase/IRichBaseTrackSelector.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichGasQuartzWindowAbs.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichRecBase/IRichPhotonPredictor.h"
#include "RichRecBase/IRichPhotonSignal.h"
#include "RichRecBase/IRichPixelBackgroundEsti.h"
#include "RichRecBase/IRichPID.h"
#include "RichRecBase/IRichRayleighScatter.h"
#include "RichRecBase/IRichSellmeirFunc.h"
#include "RichRecBase/IRichSignalDetectionEff.h"
#include "RichRecBase/IRichTrSegMaker.h"
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichPhotonReconstruction.h"
#include "RichRecBase/IRichPixelCreator.h"
#include "RichRecBase/IRichRayTraceCherenkovCone.h"
#include "RichRecBase/IRichRecGeomTool.h"
#include "RichRecBase/IRichSegmentCreator.h"
#include "RichRecBase/IRichStatusCreator.h"
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichPhotonEmissionPoint.h"
#include "RichRecBase/IRichPIDPlots.h"
#include "RichRecBase/IRichStereoFitter.h"
#include "RichRecBase/IRichStereoProjection.h"
#include "RichRecBase/IRichIsolatedTrack.h"
#include "RichRecBase/IRichTrackEffectiveRefractiveIndex.h"

// instantiate some templated classes, to get them into the dictionary
namespace 
{
  struct _Instantiations 
  {
    Rich::ObjPtn<Rich::Rec::RadCorrLocalPositions>                 obj_1;
    Rich::ObjPtn<std::vector<LHCb::RichRecTrack*> >                obj_2;
    Rich::ObjPtn<std::vector<LHCb::RichRecPhoton*> >               obj_3;
    Rich::HypoData<LHCb::RichRecRing*>                             obj_4;
    Rich::HypoData<float>                                          obj_5;
    Rich::ObjPtn<LHCb::FastAllocVector<LHCb::RichRecRingOnPixel> > obj_6;
    Rich::ObjPtn<LHCb::FastAllocVector<LHCb::RichRecPhoton*> >     obj_7;
    Rich::ObjPtn<LHCb::FastAllocVector<LHCb::RichRecTrack*> >      obj_8;
  };
}

#endif // DICT_RICHRECBASEDICT_H

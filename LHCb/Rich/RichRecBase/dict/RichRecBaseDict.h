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
    LHCb::RichRecPhoton::Vector                                    obj_0;
    LHCb::RichRecTrack::Vector                                     obj_0a;
    Rich::ObjPtn<Rich::Rec::RadCorrLocalPositions>                 obj_1;
    Rich::ObjPtn<LHCb::RichRecTrack::Vector >                      obj_2;
    Rich::ObjPtn<LHCb::RichRecPhoton::Vector >                     obj_3;
    Rich::HypoData<LHCb::RichRecRing*>                             obj_4;
    Rich::HypoData<float>                                          obj_5;
    LHCb::RichRecPixel::RingsOnPixel                               obj_6;
    LHCb::RichRecPixel::Photons                                    obj_7;
    LHCb::RichRecPixel::Tracks                                     obj_8;
    Rich::ObjPtn<LHCb::RichRecPixel::RingsOnPixel >                obj_9;
    Rich::ObjPtn<LHCb::RichRecPixel::Photons >                    obj_10;
    Rich::ObjPtn<LHCb::RichRecPixel::Tracks >                     obj_11;
    LHCb::RichRecSegments                                         obj_12;
    LHCb::RichRecTracks                                           obj_13;
    LHCb::RichRecPixels                                           obj_14;
    LHCb::RichRecPhotons                                          obj_15;
    LHCb::RichRecRings                                            obj_16;
    LHCb::RichRecTrack::Segments                                  obj_17;
    LHCb::RichRecTrack::Pixels                                    obj_18;
    LHCb::RichRecTrack::Photons                                   obj_19;
    LHCb::RichRecSegment::PDGeomEffs                              obj_20;
    LHCb::RichRecSegment::PDGeomEffsVector                        obj_21;
    LHCb::RichRecSegment::Pixels                                  obj_22;
    LHCb::RichRecSegment::Photons                                 obj_23;
    LHCb::RichRecPixelOnRing::Vector                              obj_24;
  };
}

#endif // DICT_RICHRECBASEDICT_H

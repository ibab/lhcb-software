#ifndef DICT_RICHRECBASEDICT_H 
#define DICT_RICHRECBASEDICT_H 1

#include "RichRecBase/IRichBaseTrackSelector.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"
#include "RichRecBase/IRichGasQuartzWindowAbs.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichRecBase/IRichPhotonPredictor.h"
#include "RichRecBase/IRichPhotonSignal.h"
#include "RichRecBase/IRichRayleighScatter.h"
#include "RichRecBase/IRichSellmeirFunc.h"
#include "RichRecBase/IRichSignalDetectionEff.h"
#include "RichRecBase/IRichTrSegMaker.h"
#include "RichRecBase/IRichTrackSelector.h"
#ifndef WIN32 // These include RichRecSegment.h, which breaks gccxml compilation on Windows
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichPhotonReconstruction.h"
#include "RichRecBase/IRichPixelCreator.h"
#include "RichRecBase/IRichRayTraceCherenkovCone.h"
#include "RichRecBase/IRichRecGeomTool.h"
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichRecBase/IRichSegmentCreator.h"
#include "RichRecBase/IRichStatusCreator.h"
#include "RichRecBase/IRichTrackCreator.h"
#endif // WIN32

#endif // DICT_RICHRECBASEDICT_H


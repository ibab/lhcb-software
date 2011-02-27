#ifndef DICT_RICHKERNELDICT_H 
#define DICT_RICHKERNELDICT_H 1

#ifdef __INTEL_COMPILER       // Disable ICC remark from ROOT
#pragma warning(disable:1572) // Floating-point comparisons are unreliable
#endif

#include "RichKernel/RichPixelCluster.h"
#include "RichKernel/RichGeomPhoton.h"
#include "RichKernel/RichTrackSegment.h"
#include "RichKernel/RichDAQDefinitions.h"
#include "RichKernel/RichDecodedData.h"
#include "RichKernel/RichHypoData.h"
#include "RichKernel/RichObjPtn.h"
#include "RichKernel/RichSmartIDCnv.h"
#include "RichKernel/RichHPDIdentifier.h"
#include "RichKernel/RichDAQL0Header.h"

// Interfaces
#include "RichKernel/IHpdUkL1DisableTool.h"
#include "RichKernel/IRichDetParameters.h"
#include "RichKernel/IRichMirrorSegFinder.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichKernel/IRichPixelClusteringTool.h"
#include "RichKernel/IRichPixelSuppressionTool.h"
#include "RichKernel/IRichRadiatorTool.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichKernel/IRichRawDataFormatTool.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichRefractiveIndex.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichToolRegistry.h"
#include "RichKernel/IRichSnellsLawRefraction.h"
#include "RichKernel/IRichAddBackground.h"
#include "RichKernel/IRichHPDOccupancyTool.h"
#include "RichKernel/IRichGenericHPDAnalysisTool.h"

// instanciate templated classes
namespace 
{
  struct _Instantiations 
  {
    Rich::IAddBackground::HPDBackgrounds         obj_1;
    Rich::IGenericHPDAnalysisTool::Result        obj_2;
    Rich::IGenericHPDAnalysisTool::Results       obj_3;
    Rich::DAQ::Level0ID                          obj_4;
    Rich::DAQ::EventID                           obj_5;
    Rich::DAQ::BXID                              obj_6;
    Rich::DAQ::Level1LogicalID                   obj_7;
    Rich::DAQ::Level1HardwareID                  obj_8;
    Rich::DAQ::L1IngressID                       obj_9;
    Rich::DAQ::L1InputWithinIngress              obj_10;
    Rich::DAQ::Level1Input                       obj_11;
    Rich::DAQ::HPDHardwareID                     obj_12;
    Rich::DAQ::HPDL1InputID                      obj_13;
    Rich::DAQ::HPDCopyNumber                     obj_14;
    Rich::DAQ::Level1CopyNumber                  obj_15;
  };
}

#endif // DICT_RICHKERNELDICT_H


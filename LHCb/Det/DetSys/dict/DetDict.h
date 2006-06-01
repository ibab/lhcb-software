// $Id: DetDict.h,v 1.16 2006-06-01 09:04:11 cattanem Exp $
// ============================================================================
#ifndef DETSYS_DETDICT_H 
#define DETSYS_DETDICT_H 1
// ============================================================================
// Hack to get round gccxml parsing problem (SEAL bug 9704)
// ============================================================================
#ifdef _WIN32
#define LONG_LONG_MAX     0x7fffffffffffffffLL  /*maximum signed __int64 value */
#define LONG_LONG_MIN     0x8000000000000000LL  /*minimum signed __int64 value */
#define ULONG_LONG_MAX    0xffffffffffffffffLL  /*maximum unsigned __int64 value */
#endif
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IUpdateManagerSvc.h"
// ============================================================================
// DetDesc 
// ============================================================================
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/ITransportSvc.h"
// ============================================================================
#include "DetDesc/IAlignment.h"
#include "DetDesc/ICalibration.h"
#include "DetDesc/IConditionInfo.h"
#include "DetDesc/IFastControl.h"
#include "DetDesc/ISlowControl.h"
#include "DetDesc/IReadOut.h"
// ============================================================================
#include "DetDesc/Material.h"
#include "DetDesc/Mixture.h"
#include "DetDesc/Element.h"
#include "DetDesc/Isotope.h"
// ============================================================================
#include "DetDesc/LogVolBase.h"
#include "DetDesc/LVolume.h"
#include "DetDesc/LAssembly.h"
// ============================================================================
#include "DetDesc/Condition.h"
#include "DetDesc/AlignmentCondition.h"
#include "DetDesc/Surface.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/ValidDataObject.h"
#include "DetDesc/ParamValidDataObject.h"
// ============================================================================
#include "DetDesc/SolidBase.h"
#include "DetDesc/SolidBoolean.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidChild.h"
#include "DetDesc/SolidCons.h"
#include "DetDesc/SolidIntersection.h"
#include "DetDesc/SolidPolycone.h"
#include "DetDesc/SolidPolyHedronHelper.h"
#include "DetDesc/SolidSphere.h"
#include "DetDesc/SolidSubtraction.h"
#include "DetDesc/SolidTrap.h"
#include "DetDesc/SolidTrd.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidUnion.h"
// ============================================================================
#include "DetDesc/PVolume.h"
#include "DetDesc/GeometryInfoPlus.h"
#include "DetDesc/AlignmentInfo.h"
#include "DetDesc/CalibrationInfo.h"
#include "DetDesc/ConditionInfo.h"
#include "DetDesc/FastControlInfo.h"
#include "DetDesc/ReadOutInfo.h"
#include "DetDesc/SlowControlInfo.h"
#include "DetDesc/SimpleValidity.h"
// ============================================================================
// VeloDet
// ============================================================================
#include "Kernel/VeloChannelID.h"
#include "VeloDet/DeVelo.h"
#include "VeloDet/DeVeloSensor.h"
#include "VeloDet/DeVeloPhiType.h"
#include "VeloDet/DeVeloRType.h"
// ============================================================================
// CaloDet 
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
#include "CaloDet/DeSubCalorimeter.h"
// ============================================================================
// OTDet 
// ============================================================================
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTQuarter.h"
#include "OTDet/DeOTStation.h"
// ============================================================================
// STDet
// ============================================================================
#include "Kernel/STChannelID.h"
#include "STDet/DeSTDetector.h"
#include "STDet/DeTTDetector.h"
#include "STDet/DeITDetector.h"
#include "STDet/DeSTStation.h"
#include "STDet/DeTTStation.h"
#include "STDet/DeITStation.h"
#include "STDet/DeSTLayer.h"
#include "STDet/DeTTLayer.h"
#include "STDet/DeITLayer.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeTTSector.h"
#include "STDet/DeITSector.h"
#include "STDet/DeITBox.h"
#include "STDet/DeTTHalfModule.h"

// ============================================================================
// RichDet 
// ============================================================================
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"
#include "RichDet/DeRich.h"
#include "RichDet/DeRichHPDPanel.h"
#include "RichDet/DeRichMultiSolidRadiator.h"
#include "RichDet/DeRichRadiator.h"
#include "RichDet/DeRichSingleSolidRadiator.h"
#include "RichDet/DeRichSphMirror.h"
#include "RichDet/RichMirrorSegPosition.h"

// ============================================================================
// MuonDet
// ============================================================================
#include "MuonDet/DeMuonChamber.h"
#include "MuonDet/DeMuonRegion.h" 
#include "MuonDet/DeMuonGasGap.h"   
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/MuonReadoutCond.h"
#include "MuonDet/MuonTSMap.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonStationCabling.h"
// ============================================================================


/** @file 
 *   
 *  Helper file to generate LCG dictionaries for detector description classes
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-03-01
 *  @date   updated 2005-07-07
 */

namespace DetDescPython
{
  //--- Templace instantiations
  struct __Instantiations 
  {
    ILVolume::PVolumes      i1 ;
    ILVolume::ReplicaPath   i2 ;
    ILVolume::PVolumePath   i3 ;
    ILVolume::Interval      i4 ;
    ILVolume::Intersection  i5 ;
    ILVolume::Intersections i6 ;
    ISolid::Ticks           i7 ;
    
    SmartRef<Condition> sref_inst1;
  };
  
};

// ============================================================================
#endif // DETSYS_DETDICT_H
// ============================================================================


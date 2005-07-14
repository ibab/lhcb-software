// $Id: DetDict.h,v 1.3 2005-07-14 11:04:09 jpalac Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.3 $
// ============================================================================
#ifndef BENDERDET_BENDERDETDICT_H 
#define BENDERDET_BENDERDETDICT_H 1
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ITime.h"
#include "GaudiKernel/TimePoint.h"
// ============================================================================
#include "GaudiKernel/ContainedObject.h"
// ============================================================================
// DetDesc 
// ============================================================================
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/ITransportSvc.h"
#include "DetDesc/IUpdateManagerSvc.h"
// ============================================================================
#include "DetDesc/IAlignment.h"
#include "DetDesc/ICalibration.h"
#include "DetDesc/ICondDBAccessSvc.h"
#include "DetDesc/ICondDBCnvSvc.h"
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
#include "DetDesc/../src/Lib/PVolume.h"
#include "DetDesc/../src/Lib/GeometryInfoPlus.h"
#include "DetDesc/../src/Lib/AlignmentInfo.h"
#include "DetDesc/../src/Lib/CalibrationInfo.h"
#include "DetDesc/../src/Lib/ConditionInfo.h"
#include "DetDesc/../src/Lib/FastControlInfo.h"
#include "DetDesc/../src/Lib/ReadOutInfo.h"
#include "DetDesc/../src/Lib/SlowControlInfo.h"
#include "DetDesc/../src/Lib/SimpleValidity.h"
// ============================================================================
// VeloDet
// ============================================================================
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
#include "Kernel/ITChannelID.h"
#include "STDet/DeSTDetector.h"
#include "STDet/STDetectionLayer.h"
#include "STDet/STDetectionStation.h"
#include "STDet/ITLayer.h"
#include "STDet/TTLayer.h"
#include "STDet/STWafer.h"
// ============================================================================
// RichDet 
// ============================================================================
#include "RichDet/DeRich.h"
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichFlatMirror.h"
#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRichHPDPanel.h"
#include "RichDet/DeRich1HPDPanel.h"
#include "RichDet/DeRich2HPDPanel.h"
#include "RichDet/DeRich1CdfHPDPanel.h"
#include "RichDet/DeRichRadiator.h"
#include "RichDet/DeRichMultiSolidRadiator.h"
#include "RichDet/DeRichSingleSolidRadiator.h"
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

namespace GaudiPython 
{
  template <class I> struct Interface 
  {
    Interface() {}
    static I* cast ( IInterface* in) 
    {
      void* out = 0;
      if ( 0 == in        ) { return 0 ; }
      StatusCode sc = in->queryInterface(I::interfaceID(), &out ) ;
      if ( sc.isFailure() ) { return 0 ; }
      //
      return (I*)out;
    }
  };

};

namespace DetDescPython
{
  
  template <class I> 
  struct Interface 
  {
    Interface() {}
    static I* cast ( DataObject* obj ) 
    { return dynamic_cast<I*> ( obj ) ; };
    static I* cast ( ISolid*     obj ) 
    { return dynamic_cast<I*> ( obj ) ; };
    static I* cast ( IInterface* obj ) 
    { return dynamic_cast<I*> ( obj ) ; };
  };
  
  //--- Templace instantiations
  struct __Instantiations 
  {
    std::vector<double>     i0 ;
    ILVolume::PVolumes      i1 ;
    ILVolume::ReplicaPath   i2 ;
    ILVolume::PVolumePath   i3 ;
    ILVolume::Interval      i4 ;
    ILVolume::Intersection  i5 ;
    ILVolume::Intersections i6 ;
    ISolid::Ticks           i7 ;
    
    Interface<IDetectorElement>      a1_1  ;
    Interface<DetectorElement>       a1_2  ;
    Interface<IGeometryInfo>         a2    ;
    Interface<ILVolume>              a3_1  ;
    Interface<LogVolBase>            a3_2  ;
    Interface<LVolume>               a3_3  ;
    Interface<LAssembly>             a3_4  ;
    Interface<IPVolume>              a4    ;
    
    Interface<ISolid>                a5    ;
    Interface<SolidBase>             a5_1  ;
    Interface<SolidBoolean>          a5_2  ;
    Interface<SolidBox>              a5_3  ;
    Interface<SolidChild>            a5_4  ;
    Interface<SolidCons>             a5_5  ;
    Interface<SolidIntersection>     a5_6  ;
    Interface<SolidPolycone>         a5_7  ;
    Interface<SolidPolycone>         a5_8  ;
    Interface<SolidPolyHedronHelper> a5_9  ;
    Interface<SolidSphere>           a5_10 ;
    Interface<SolidSubtraction>      a5_11 ;
    Interface<SolidTrap>             a5_12 ;
    Interface<SolidTrd>              a5_13 ;
    Interface<SolidTubs>             a5_14 ;
    Interface<SolidUnion>            a5_15 ;
    
    Interface<Material>              a6_1  ;
    Interface<Mixture>               a6_2  ;
    Interface<Element>               a6_3  ;
    Interface<Isotope>               a6_4  ;
    //

    GaudiPython::Interface<ITransportSvc>       s1 ;
    GaudiPython::Interface<IUpdateManagerSvc>   s2 ;
    GaudiPython::Interface<ICondDBAccessSvc>    s3 ;
    GaudiPython::Interface<ICondDBCnvSvc>       s4 ;
    
    
  };
  
};


// ============================================================================
// The END 
// ============================================================================
#endif // BENDERDET_BENDERDETDICT_H
// ============================================================================

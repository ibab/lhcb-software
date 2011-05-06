// $Id: DetDescDict.h,v 1.8 2009-12-05 14:25:47 dgolubko Exp $
// ============================================================================
#ifndef DETSYS_DETDICT_H 
#define DETSYS_DETDICT_H 1
// ============================================================================
// DetDesc 
// ============================================================================
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IDetElemFinder.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/ISolid.h"
#include "DetDesc/ITransportSvc.h"
#include "DetDesc/IGeometryErrorSvc.h"
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
#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/GlobalToLocalDelta.h"
// ============================================================================
#include "DetDesc/ParamAsHisto.h"
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
  //--- Template instantiations
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
    std::pair<double,SmartRef<Element> > sref_inst2;
    std::pair<double,SmartRef<Isotope> > sref_inst3;
    SmartRef<AlignmentCondition> sref_inst4;
  };
  
}

#define ParamValidDataObject_template_instantiation_base(Type) \
  template void ParamValidDataObject::addParam(const std::string &, const Type &, const std::string &); \
  template Type &ParamValidDataObject::param< Type >(const std::string &)

#define ParamValidDataObject_template_instantiation_vect(Type) \
  ParamValidDataObject_template_instantiation_base(std::vector< Type >); \
  template std::vector< Type > &ParamValidDataObject::paramVect< Type >(const std::string &)

#define ParamValidDataObject_template_instantiation_map(Key,Type) \
  template void ParamValidDataObject::addParam(const std::string &, const std::map< Key , Type > &, const std::string &); \
  template std::map< Key , Type > &ParamValidDataObject::param<std::map< Key , Type > >(const std::string &)

#define ParamValidDataObject_template_instantiation(Type) \
  ParamValidDataObject_template_instantiation_base(Type); \
  ParamValidDataObject_template_instantiation_vect(Type); \
  ParamValidDataObject_template_instantiation_map(std::string,Type); \
  ParamValidDataObject_template_instantiation_map(int,Type)

ParamValidDataObject_template_instantiation(int);
ParamValidDataObject_template_instantiation(double);
ParamValidDataObject_template_instantiation(std::string);

#ifdef __INTEL_COMPILER
// Disable some ICC warnings in the automatically generated code
#pragma warning(disable:177) // variable declared but never used
#pragma warning(disable:654) // overloaded virtual function only partially overridden
#endif

// ============================================================================
#endif // DETSYS_DETDICT_H
// ============================================================================


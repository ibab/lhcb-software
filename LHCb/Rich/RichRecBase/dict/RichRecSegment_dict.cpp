

//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************



//Include files
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

#include <string>

#include "RichRecSegment_dict.h"

#include "GaudiIntrospection/Introspection.h"


//------------------------------------------------------------------------------
class RichRecSegment_dict
//------------------------------------------------------------------------------
{
public:
  RichRecSegment_dict();
};

//------------------------------------------------------------------------------
static void RichRecSegment_setGeomEfficiency_0(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setGeomEfficiency(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void RichRecSegment_setGeomEfficiencyScat_1(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setGeomEfficiencyScat(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void RichRecSegment_setNEmittedPhotons_2(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setNEmittedPhotons(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void RichRecSegment_setNDetectablePhotons_3(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setNDetectablePhotons(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void RichRecSegment_setNSignalPhotons_4(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setNSignalPhotons(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void RichRecSegment_setNScatteredPhotons_5(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setNScatteredPhotons(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void RichRecSegment_setAverageCKTheta_6(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setAverageCKTheta(*(Rich::ParticleIDType*)argList[0],
          *(float*)argList[1]);
}

//------------------------------------------------------------------------------
static void RichRecSegment_addToGeomEfficiencyPerHPD_7(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->addToGeomEfficiencyPerHPD(*(Rich::ParticleIDType*)argList[0],
          *(int*)argList[1],
          *(float*)argList[2]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_nEmittedPhotons_8const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecSegment*)v)->nEmittedPhotons(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecSegment_nDetectablePhotons_9const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecSegment*)v)->nDetectablePhotons(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecSegment_nSignalPhotons_10const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecSegment*)v)->nSignalPhotons(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecSegment_nScatteredPhotons_11const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecSegment*)v)->nScatteredPhotons(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecSegment_nTotalObservablePhotons_12const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecSegment*)v)->nTotalObservablePhotons(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecSegment_nEmitPhotsPerEnBin_13(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  return &(std::vector<float>&) ((RichRecSegment*)v)->nEmitPhotsPerEnBin(*(Rich::ParticleIDType*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_geomEfficiency_14const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecSegment*)v)->geomEfficiency(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecSegment_geomEfficiencyScat_15const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecSegment*)v)->geomEfficiencyScat(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecSegment_averageCKTheta_16const(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  static float ret;
  ret = ((RichRecSegment*)v)->averageCKTheta(*(Rich::ParticleIDType*)argList[0]);
  return &ret;
}

//------------------------------------------------------------------------------
static void* RichRecSegment_geomEfficiencyPerHPD_17(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  return &(RichMap<int,float>&) ((RichRecSegment*)v)->geomEfficiencyPerHPD(*(Rich::ParticleIDType*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_nEmittedPhotons_18(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecSegment*)v)->nEmittedPhotons();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setNEmittedPhotons_19(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setNEmittedPhotons(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_nDetectablePhotons_20(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecSegment*)v)->nDetectablePhotons();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setNDetectablePhotons_21(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setNDetectablePhotons(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_nSignalPhotons_22(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecSegment*)v)->nSignalPhotons();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setNSignalPhotons_23(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setNSignalPhotons(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_nScatteredPhotons_24(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecSegment*)v)->nScatteredPhotons();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setNScatteredPhotons_25(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setNScatteredPhotons(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_nEmitPhotsPerEnBin_26(void* v)
//------------------------------------------------------------------------------
{
  const std::vector< std::vector<float> >& ret = ((RichRecSegment*)v)->nEmitPhotsPerEnBin();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setNEmitPhotsPerEnBin_27(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setNEmitPhotsPerEnBin(*(std::vector< std::vector<float> >*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_geomEfficiencyPerHPD_28(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<RichMap<int,float> >& ret = ((RichRecSegment*)v)->geomEfficiencyPerHPD();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setGeomEfficiencyPerHPD_29(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setGeomEfficiencyPerHPD(*(std::vector<RichMap<int,float> >*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_geomEfficiency_30(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecSegment*)v)->geomEfficiency();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setGeomEfficiency_31(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setGeomEfficiency(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_averageCKTheta_32(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecSegment*)v)->averageCKTheta();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setAverageCKTheta_33(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setAverageCKTheta(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_trackSegment_34(void* v)
//------------------------------------------------------------------------------
{
  const RichTrackSegment& ret = ((RichRecSegment*)v)->trackSegment();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setTrackSegment_35(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setTrackSegment(*(RichTrackSegment*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_hpdPanelHitPoint_36(void* v)
//------------------------------------------------------------------------------
{
  const HepPoint3D& ret = ((RichRecSegment*)v)->hpdPanelHitPoint();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setHpdPanelHitPoint_37(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setHpdPanelHitPoint(*(HepPoint3D*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_hitDetectorRegions_38(void* v)
//------------------------------------------------------------------------------
{
  static unsigned long ret;
  ret =  ((RichRecSegment*)v)->hitDetectorRegions();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setHitDetectorRegions_39(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setHitDetectorRegions(*(unsigned long*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_geomEfficiencyScat_40(void* v)
//------------------------------------------------------------------------------
{
  const std::vector<float>& ret = ((RichRecSegment*)v)->geomEfficiencyScat();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setGeomEfficiencyScat_41(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setGeomEfficiencyScat(*(std::vector<float>*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_richRecTrack_42(void* v)
//------------------------------------------------------------------------------
{
  RichRecTrack* ret = ((RichRecSegment*)v)->richRecTrack();
  return  (void*)ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setRichRecTrack_43(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setRichRecTrack((RichRecTrack*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_richRecPixels_44(void* v)
//------------------------------------------------------------------------------
{
  const SmartRefVector<RichRecPixel>& ret = ((RichRecSegment*)v)->richRecPixels();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setRichRecPixels_45(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setRichRecPixels(*(SmartRefVector<RichRecPixel>*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecSegment_clearRichRecPixels_46(void* v)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->clearRichRecPixels();
}

//------------------------------------------------------------------------------
static void RichRecSegment_addToRichRecPixels_47(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->addToRichRecPixels((RichRecPixel*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecSegment_removeFromRichRecPixels_48(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->removeFromRichRecPixels((RichRecPixel*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_richRecPhotons_49(void* v)
//------------------------------------------------------------------------------
{
  const SmartRefVector<RichRecPhoton>& ret = ((RichRecSegment*)v)->richRecPhotons();
  return (void*)&ret;
}

//------------------------------------------------------------------------------
static void RichRecSegment_setRichRecPhotons_50(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->setRichRecPhotons(*(SmartRefVector<RichRecPhoton>*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecSegment_clearRichRecPhotons_51(void* v)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->clearRichRecPhotons();
}

//------------------------------------------------------------------------------
static void RichRecSegment_addToRichRecPhotons_52(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->addToRichRecPhotons((RichRecPhoton*)argList[0]);
}

//------------------------------------------------------------------------------
static void RichRecSegment_removeFromRichRecPhotons_53(void* v, std::vector<void*> argList)
//------------------------------------------------------------------------------
{
  ((RichRecSegment*)v)->removeFromRichRecPhotons((RichRecPhoton*)argList[0]);
}

//------------------------------------------------------------------------------
static void* RichRecSegment_constructor_0()
//------------------------------------------------------------------------------
{
  static RichRecSegment* ret = new RichRecSegment();
  return ret;
}

//------------------------------------------------------------------------------
static RichRecSegment_dict instance;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
RichRecSegment_dict::RichRecSegment_dict()
//------------------------------------------------------------------------------
{
  std::vector<std::string> argTypes = std::vector<std::string>();
  MetaClass* metaC = new MetaClass("RichRecSegment",
                "Rich reconstruction object describing a continuous track segment in a single radiator",
                0);

  RichRecSegment* cl = new RichRecSegment();
  metaC->addSuperClass("KeyedObject<int>",
                (((int)cl)-((int)((KeyedObject<int>*)cl))));
  delete cl;

  metaC->addConstructor("Default Constructor",
                RichRecSegment_constructor_0);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("nEmittedPhotons",
                "std::vector<float>",
                "The number of emitted photons expected for each particle hypothesis",
                &((RichRecSegment*)0)->m_nEmittedPhotons,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("nDetectablePhotons",
                "std::vector<float>",
                "The number of detectable emitted photons expected for each particle hypothesis",
                &((RichRecSegment*)0)->m_nDetectablePhotons,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("nSignalPhotons",
                "std::vector<float>",
                "The number of signal photons expected for each particle hypothesis",
                &((RichRecSegment*)0)->m_nSignalPhotons,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("nScatteredPhotons",
                "std::vector<float>",
                "The number of scattered photons expected for each particle hypothesis",
                &((RichRecSegment*)0)->m_nScatteredPhotons,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass(" std::vector<float>",
                "standard container of float",
                sizeof( std::vector<float>),
                0xC1,
                0);
  MetaClass::condCreateClass("std::vector< std::vector<float> >",
                "standard container of  std::vector<float>",
                sizeof(std::vector< std::vector<float> >),
                0xC1,
                0);
  metaC->addField("nEmitPhotsPerEnBin",
                "std::vector< std::vector<float> >",
                "Number of emitted photons per enrgy bin. Internal working object to RichSegmentTool",
                &((RichRecSegment*)0)->m_nEmitPhotsPerEnBin,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<RichMap<int,float> >",
                "standard container of RichMap<int,float>",
                sizeof(std::vector<RichMap<int,float> >),
                0xC1,
                0);
  metaC->addField("geomEfficiencyPerHPD",
                "std::vector<RichMap<int,float> >",
                "The geometrical efficiency for a given hypothesis, binned per HPD",
                &((RichRecSegment*)0)->m_geomEfficiencyPerHPD,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("geomEfficiency",
                "std::vector<float>",
                "Geometrical efficiencies for the each particle hypothesis",
                &((RichRecSegment*)0)->m_geomEfficiency,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("averageCKTheta",
                "std::vector<float>",
                "The average Cherenkov theta angle for given particle ID type",
                &((RichRecSegment*)0)->m_averageCKTheta,
                MetaModifier::setPrivate());

  metaC->addField("trackSegment",
                "RichTrackSegment",
                "Track segment, the passage of a track through a Rich detector",
                &((RichRecSegment*)0)->m_trackSegment,
                MetaModifier::setPrivate());

  metaC->addField("hpdPanelHitPoint",
                "HepPoint3D",
                "Ray traced HPD panel impact point",
                &((RichRecSegment*)0)->m_hpdPanelHitPoint,
                MetaModifier::setPrivate());

  metaC->addField("hitDetectorRegions",
                "unsigned long",
                "Detector regions accessible to this segment",
                &((RichRecSegment*)0)->m_hitDetectorRegions,
                MetaModifier::setPrivate());

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addField("geomEfficiencyScat",
                "std::vector<float>",
                "Geometrical efficiency for scattered component",
                &((RichRecSegment*)0)->m_geomEfficiencyScat,
                MetaModifier::setPrivate());

  metaC->addField("richRecTrack",
                "SmartRef<RichRecTrack>",
                "Smart Reference to associated RichRecTrack",
                &((RichRecSegment*)0)->m_richRecTrack,
                MetaModifier::setPrivate());

  metaC->addField("richRecPixels",
                "SmartRefVector<RichRecPixel>",
                "SmartRefVector of RichRecPixels for which this track has valid photon associations",
                &((RichRecSegment*)0)->m_richRecPixels,
                MetaModifier::setPrivate());

  metaC->addField("richRecPhotons",
                "SmartRefVector<RichRecPhoton>",
                "SmartRef to associated RichRecPhotons",
                &((RichRecSegment*)0)->m_richRecPhotons,
                MetaModifier::setPrivate());

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setGeomEfficiency",
                "Set the geometrical efficiency for a given particle type",
                argTypes,
                RichRecSegment_setGeomEfficiency_0);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setGeomEfficiencyScat",
                "Set the scattered geometrical efficiency for a given particle type",
                argTypes,
                RichRecSegment_setGeomEfficiencyScat_1);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setNEmittedPhotons",
                "Set the number of emitted photons for a given particle type",
                argTypes,
                RichRecSegment_setNEmittedPhotons_2);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setNDetectablePhotons",
                "Set the number of detectable photons for a given particle type",
                argTypes,
                RichRecSegment_setNDetectablePhotons_3);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setNSignalPhotons",
                "Set the number of signal photons for a given particle type",
                argTypes,
                RichRecSegment_setNSignalPhotons_4);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setNScatteredPhotons",
                "Set the number of scattered photons for a given particle type",
                argTypes,
                RichRecSegment_setNScatteredPhotons_5);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("float");
  metaC->addMethod("setAverageCKTheta",
                "Set the average Cherenkov angle for a given particle type",
                argTypes,
                RichRecSegment_setAverageCKTheta_6);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  argTypes.push_back("int");
  argTypes.push_back("float");
  metaC->addMethod("addToGeomEfficiencyPerHPD",
                "Method to create the geometrical efficiencies per HPD",
                argTypes,
                RichRecSegment_addToGeomEfficiencyPerHPD_7);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("nEmittedPhotons",
                "Returns the number of emitted photons for a given particle type",
                "float",
                argTypes,
                RichRecSegment_nEmittedPhotons_8const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("nDetectablePhotons",
                "Returns the number of detectable photons for a given particle type",
                "float",
                argTypes,
                RichRecSegment_nDetectablePhotons_9const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("nSignalPhotons",
                "Returns the number of signal photons for a given particle type",
                "float",
                argTypes,
                RichRecSegment_nSignalPhotons_10const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("nScatteredPhotons",
                "Returns the number of scattered photons for a given particle type",
                "float",
                argTypes,
                RichRecSegment_nScatteredPhotons_11const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("nTotalObservablePhotons",
                "Returns the number of photons for a given particle type",
                "float",
                argTypes,
                RichRecSegment_nTotalObservablePhotons_12const);

  MetaClass::condCreateClass("std::vector<float>&",
                "standard container of float",
                sizeof(std::vector<float>&),
                0xC1,
                0);
  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("nEmitPhotsPerEnBin",
                "Returns the number of emitted photons for a given particle type",
                "std::vector<float>&",
                argTypes,
                RichRecSegment_nEmitPhotsPerEnBin_13);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("geomEfficiency",
                "Get the geometrical efficiency for a given particle type",
                "float",
                argTypes,
                RichRecSegment_geomEfficiency_14const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("geomEfficiencyScat",
                "Get the scattered geometrical efficiency for a given particle type",
                "float",
                argTypes,
                RichRecSegment_geomEfficiencyScat_15const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("averageCKTheta",
                "Get the average Cherenkov angle for a given particle type",
                "float",
                argTypes,
                RichRecSegment_averageCKTheta_16const);

  argTypes.clear();
  argTypes.push_back("Rich::ParticleIDType");
  metaC->addMethod("geomEfficiencyPerHPD",
                "Returns the geometrical efficiency per HPD for given hypothesis",
                "RichMap<int,float>&",
                argTypes,
                RichRecSegment_geomEfficiencyPerHPD_17);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("nEmittedPhotons",
                "The number of emitted photons expected for each particle hypothesis",
                "std::vector<float>",
                RichRecSegment_nEmittedPhotons_18);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setNEmittedPhotons",
                "The number of emitted photons expected for each particle hypothesis",
                argTypes,
                RichRecSegment_setNEmittedPhotons_19);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("nDetectablePhotons",
                "The number of detectable emitted photons expected for each particle hypothesis",
                "std::vector<float>",
                RichRecSegment_nDetectablePhotons_20);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setNDetectablePhotons",
                "The number of detectable emitted photons expected for each particle hypothesis",
                argTypes,
                RichRecSegment_setNDetectablePhotons_21);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("nSignalPhotons",
                "The number of signal photons expected for each particle hypothesis",
                "std::vector<float>",
                RichRecSegment_nSignalPhotons_22);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setNSignalPhotons",
                "The number of signal photons expected for each particle hypothesis",
                argTypes,
                RichRecSegment_setNSignalPhotons_23);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("nScatteredPhotons",
                "The number of scattered photons expected for each particle hypothesis",
                "std::vector<float>",
                RichRecSegment_nScatteredPhotons_24);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setNScatteredPhotons",
                "The number of scattered photons expected for each particle hypothesis",
                argTypes,
                RichRecSegment_setNScatteredPhotons_25);

  MetaClass::condCreateClass(" std::vector<float>",
                "standard container of float",
                sizeof( std::vector<float>),
                0xC1,
                0);
  MetaClass::condCreateClass("std::vector< std::vector<float> >",
                "standard container of  std::vector<float>",
                sizeof(std::vector< std::vector<float> >),
                0xC1,
                0);
  metaC->addMethod("nEmitPhotsPerEnBin",
                "Number of emitted photons per enrgy bin. Internal working object to RichSegmentTool",
                "std::vector< std::vector<float> >",
                RichRecSegment_nEmitPhotsPerEnBin_26);

  argTypes.clear();
  argTypes.push_back("std::vector< std::vector<float> >");
  metaC->addMethod("setNEmitPhotsPerEnBin",
                "Number of emitted photons per enrgy bin. Internal working object to RichSegmentTool",
                argTypes,
                RichRecSegment_setNEmitPhotsPerEnBin_27);

  MetaClass::condCreateClass("std::vector<RichMap<int,float> >",
                "standard container of RichMap<int,float>",
                sizeof(std::vector<RichMap<int,float> >),
                0xC1,
                0);
  metaC->addMethod("geomEfficiencyPerHPD",
                "The geometrical efficiency for a given hypothesis, binned per HPD",
                "std::vector<RichMap<int,float> >",
                RichRecSegment_geomEfficiencyPerHPD_28);

  argTypes.clear();
  argTypes.push_back("std::vector<RichMap<int,float> >");
  metaC->addMethod("setGeomEfficiencyPerHPD",
                "The geometrical efficiency for a given hypothesis, binned per HPD",
                argTypes,
                RichRecSegment_setGeomEfficiencyPerHPD_29);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("geomEfficiency",
                "Geometrical efficiencies for the each particle hypothesis",
                "std::vector<float>",
                RichRecSegment_geomEfficiency_30);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setGeomEfficiency",
                "Geometrical efficiencies for the each particle hypothesis",
                argTypes,
                RichRecSegment_setGeomEfficiency_31);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("averageCKTheta",
                "The average Cherenkov theta angle for given particle ID type",
                "std::vector<float>",
                RichRecSegment_averageCKTheta_32);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setAverageCKTheta",
                "The average Cherenkov theta angle for given particle ID type",
                argTypes,
                RichRecSegment_setAverageCKTheta_33);

  metaC->addMethod("trackSegment",
                "Track segment, the passage of a track through a Rich detector",
                "RichTrackSegment",
                RichRecSegment_trackSegment_34);

  argTypes.clear();
  argTypes.push_back("RichTrackSegment");
  metaC->addMethod("setTrackSegment",
                "Track segment, the passage of a track through a Rich detector",
                argTypes,
                RichRecSegment_setTrackSegment_35);

  metaC->addMethod("hpdPanelHitPoint",
                "Ray traced HPD panel impact point",
                "HepPoint3D",
                RichRecSegment_hpdPanelHitPoint_36);

  argTypes.clear();
  argTypes.push_back("HepPoint3D");
  metaC->addMethod("setHpdPanelHitPoint",
                "Ray traced HPD panel impact point",
                argTypes,
                RichRecSegment_setHpdPanelHitPoint_37);

  metaC->addMethod("hitDetectorRegions",
                "Detector regions accessible to this segment",
                "unsigned long",
                RichRecSegment_hitDetectorRegions_38);

  argTypes.clear();
  argTypes.push_back("unsigned long");
  metaC->addMethod("setHitDetectorRegions",
                "Detector regions accessible to this segment",
                argTypes,
                RichRecSegment_setHitDetectorRegions_39);

  MetaClass::condCreateClass("std::vector<float>",
                "standard container of float",
                sizeof(std::vector<float>),
                0xC1,
                0);
  metaC->addMethod("geomEfficiencyScat",
                "Geometrical efficiency for scattered component",
                "std::vector<float>",
                RichRecSegment_geomEfficiencyScat_40);

  argTypes.clear();
  argTypes.push_back("std::vector<float>");
  metaC->addMethod("setGeomEfficiencyScat",
                "Geometrical efficiency for scattered component",
                argTypes,
                RichRecSegment_setGeomEfficiencyScat_41);

  metaC->addMethod("richRecTrack",
                "Smart Reference to associated RichRecTrack",
                "RichRecTrack",
                RichRecSegment_richRecTrack_42);

  argTypes.clear();
  argTypes.push_back("RichRecTrack");
  metaC->addMethod("setRichRecTrack",
                "Smart Reference to associated RichRecTrack",
                argTypes,
                RichRecSegment_setRichRecTrack_43);

  metaC->addMethod("richRecPixels",
                "SmartRefVector of RichRecPixels for which this track has valid photon associations",
                "SmartRefVector<RichRecPixel>",
                RichRecSegment_richRecPixels_44);

  argTypes.clear();
  argTypes.push_back("RichRecPixel");
  metaC->addMethod("setRichRecPixels",
                "SmartRefVector of RichRecPixels for which this track has valid photon associations",
                argTypes,
                RichRecSegment_setRichRecPixels_45);

  metaC->addMethod("clearRichRecPixels",
                "SmartRefVector of RichRecPixels for which this track has valid photon associations",
                RichRecSegment_clearRichRecPixels_46);

  argTypes.clear();
  argTypes.push_back("RichRecPixel");
  metaC->addMethod("addToRichRecPixels",
                "SmartRefVector of RichRecPixels for which this track has valid photon associations",
                argTypes,
                RichRecSegment_addToRichRecPixels_47);

  argTypes.clear();
  argTypes.push_back("RichRecPixel");
  metaC->addMethod("removeFromRichRecPixels",
                "SmartRefVector of RichRecPixels for which this track has valid photon associations",
                argTypes,
                RichRecSegment_removeFromRichRecPixels_48);

  metaC->addMethod("richRecPhotons",
                "SmartRef to associated RichRecPhotons",
                "SmartRefVector<RichRecPhoton>",
                RichRecSegment_richRecPhotons_49);

  argTypes.clear();
  argTypes.push_back("RichRecPhoton");
  metaC->addMethod("setRichRecPhotons",
                "SmartRef to associated RichRecPhotons",
                argTypes,
                RichRecSegment_setRichRecPhotons_50);

  metaC->addMethod("clearRichRecPhotons",
                "SmartRef to associated RichRecPhotons",
                RichRecSegment_clearRichRecPhotons_51);

  argTypes.clear();
  argTypes.push_back("RichRecPhoton");
  metaC->addMethod("addToRichRecPhotons",
                "SmartRef to associated RichRecPhotons",
                argTypes,
                RichRecSegment_addToRichRecPhotons_52);

  argTypes.clear();
  argTypes.push_back("RichRecPhoton");
  metaC->addMethod("removeFromRichRecPhotons",
                "SmartRef to associated RichRecPhotons",
                argTypes,
                RichRecSegment_removeFromRichRecPhotons_53);

  MetaPropertyList* pl = new MetaPropertyList();
  pl->setProperty("Author", "Chris Jones   (Christopher.Rob.Jones@cern.ch)");
  pl->setProperty("ClassID", "12423");
  metaC->setPropertyList(pl);
}



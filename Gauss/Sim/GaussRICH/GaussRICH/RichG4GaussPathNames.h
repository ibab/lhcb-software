#ifndef GAUSSRICH_RICHG4GAUSSPATHNAMES_H
#define GAUSSRICH_RICHG4GAUSSPATHNAMES_H 1

#include "globals.hh"

#include "RichDet/DeRich.h"

static const G4String RichHpdDemagMatTabPropPath="/dd/Materials/RichMaterialTabProperties/HpdDemagnification";
static const G4String RichHpdDemagPathName = "/dd/Materials/RichMaterialTabProperties/RichHpdDemag";

static const G4String CF4MaterialName       = "/dd/Materials/RichMaterials/CF4";
static const G4String C4F10MaterialName     = "/dd/Materials/RichMaterials/C4F10";
static const G4String R2RadGasMaterialName  = "/dd/Materials/RichMaterials/R2RadiatorGas";
static const G4String R1RadGasMaterialName  = "/dd/Materials/RichMaterials/R1RadiatorGas";
static const G4String C3F8MaterialName     = "/dd/Materials/RichMaterials/C3F08";
static const G4String AgelMaterialName      = "/dd/Materials/RichMaterials/Aerogel";
static const G4String AgelTile0MaterialName = "/dd/Materials/RichMaterials/AerogelT0";
static const G4String AgelTile1MaterialName = "/dd/Materials/RichMaterials/AerogelT1";
static const G4String AgelTile2MaterialName = "/dd/Materials/RichMaterials/AerogelT2";
static const G4String AgelTile3MaterialName = "/dd/Materials/RichMaterials/AerogelT3";
static const G4String AgelTile4MaterialName = "/dd/Materials/RichMaterials/AerogelT4";
static const G4String AgelTile5MaterialName = "/dd/Materials/RichMaterials/AerogelT5";
static const G4String AgelTile6MaterialName = "/dd/Materials/RichMaterials/AerogelT6";
static const G4String AgelTile7MaterialName = "/dd/Materials/RichMaterials/AerogelT7";
static const G4String AgelTile8MaterialName = "/dd/Materials/RichMaterials/AerogelT8";
static const G4String AgelTile9MaterialName = "/dd/Materials/RichMaterials/AerogelT9";
static const G4String AgelTile10MaterialName = "/dd/Materials/RichMaterials/AerogelT10";
static const G4String AgelTile11MaterialName = "/dd/Materials/RichMaterials/AerogelT11";
static const G4String AgelTile12MaterialName = "/dd/Materials/RichMaterials/AerogelT12";
static const G4String AgelTile13MaterialName = "/dd/Materials/RichMaterials/AerogelT13";
static const G4String AgelTile14MaterialName = "/dd/Materials/RichMaterials/AerogelT14";
static const G4String AgelTile15MaterialName = "/dd/Materials/RichMaterials/AerogelT15";
static const G4String AgelSubTileMaterialNamePrefix = "/dd/Materials/RichAerogelSubTileMaterials/AerogelT";
static const G4String RichHpdKovarMaterialName = "/dd/Materials/RichMaterials/Kovar";
static const G4String RichHpdKaptonMaterialName = "/dd/Materials/RichMaterials/HpdKapton";
static const G4String RichHpdChromiumMaterialName = "/dd/Materials/RichMaterials/HpdChromium";


static const G4String FilterGenericMaterialName="/dd/Materials/RichMaterials/Rich1FilterGenericMaterial";
static const G4String FilterD263MaterialName="/dd/Materials/RichMaterials/Rich1FilterD263Material";
static const G4String Rich1GasQWindowMaterialName="/dd/Materials/RichMaterials/Rich1GasWindowQuartz";
static const G4String Rich2GasQWindowMaterialName="/dd/Materials/RichMaterials/Rich2GasWindowQuartz";
static const G4String RichHpdQuartzWindowMaterialName="/dd/Materials/RichMaterials/HpdWindowQuartz";
static const G4String Rich1NitrogenMaterialName="/dd/Materials/RichMaterials/Rich1Nitrogen";
static const G4String Rich2NitrogenMaterialName="/dd/Materials/RichMaterials/Rich2Nitrogen";
static const G4String RichCO2MaterialName="/dd/Materials/RichMaterials/RichCO2" ;
static const G4String RichHpdQeffMatTabPropPath="/dd/Materials/RichMaterialTabProperties/NominalHpdQuantumEff";
static const G4String RichHpdQWRIMatTabPropPath="/dd/Materials/RichMaterialTabProperties/HpdWindowQuartzRIndexPT";
static const G4String RichHpdPCRIMatTabPropPath="/dd/Materials/RichMaterialTabProperties/HpdS20PhCathodeRIndexPT";
static const G4String RichHpdPsfMatTabPropPath="/dd/Materials/RichMaterialTabProperties/HpdPointSpreadFunction";
static const G4String RichHpdPsfPhEnMatTabPropPath = "/dd/Materials/RichMaterialTabProperties/HpdPSFPhotonEnergy";
static const G4String RichHpdPsfRadialMatTabPropPath = "/dd/Materials/RichMaterialTabProperties/HpdPSFRadialDist";
static const G4String RichHpdHVMatTabPropPath="/dd/Materials/RichMaterialTabProperties/HpdHighVoltage";
static const G4String RichHpdHitSmearPath="/dd/Materials/RichMaterialTabProperties/HpdHitSmear";

static const G4String LogVolAgelNameAnalysis= 
                              "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1Aerogel";

static const G4String LogVolAgelSubTileNameAnalysis=
      "/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1AerogelSubTileLogList/lvRich1AerogelSubTile";


static const G4String LogVolC4F10NameAnalysis=
                               "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1SubMaster";
static const G4String LogVolRich1Mirror1MasterNameAnalysis=
                              "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1Mirror1Master";
static const G4String LogVolRich1Mirror1QuadrantNameAnalysis=
                              "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1Mirror1QuadrantModule";
static const G4String LogVolRich1Mirror1NameAnalysis=
                              "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1Mirror1CarbonFibreInnerLayer";
static const G4String LogVolRich1Mirror2NameAnalysis=
                             "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1Mirror2";
static const G4String LogVolRich1Mirror2SegNameAnalysis=
                             "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1Mirror2Seg";

static const G4String LogVolRich1Mirror2MasterNameAnalysis=
                             "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1Mirror2Master";
static const G4String LogVolRich1Mirror2MasterTopNameAnalysis=
                             "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1Mirror2MasterTop";
static const G4String LogVolRich1Mirror2MasterBotNameAnalysis=
                             "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1Mirror2MasterBot";

static const G4String LogVolRich2Mirror1ContainerNameAnalysis=
                        "/dd/Geometry/AfterMagnetRegion/Rich2/lvRich2SphMirrorCont";

static const G4String LogVolRich2Mirror1NameAnalysis=
                              "/dd/Geometry/AfterMagnetRegion/Rich2/lvRich2SphMirror";
static const G4String LogVolRich2Mirror1NameAnalysisSubString="Hex";
static const G4String LogVolRich2Mirror2NameAnalysis=
                              "/dd/Geometry/AfterMagnetRegion/Rich2/lvRich2SecMirrorSeg";
static const G4String LogVolRich2Mirror2ContainerAnalysis=
                              "/dd/Geometry/AfterMagnetRegion/Rich2/lvRich2SecMirrorCont";
static const G4String LogVolRich1MagShNameAnalysis=
                             "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1MagSh" ;
static const G4String LogColRich1GasQWNameAnalysis=
                             "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1GQuartzW";
static const G4String LogVolCF4NameAnalysis=
                             "/dd/Geometry/AfterMagnetRegion/Rich2/lvRich2Gas";
static const G4String LogVolHpdSMasterNameAnalysis =
                     "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichHPDSMaster";
static const G4String LogVolHpdSMasterNameAnalysisStrPrefix =
                      "/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1HPDSMasterLogList";
static const G4String LogVolHpdSMasterNameAnalysisListStrPrefix [] =
                     {"/dd/Geometry/BeforeMagnetRegion/Rich1/RichHPDSMasterLogList/lvRich1HPDSMaster",
                      "/dd/Geometry/BeforeMagnetRegion/Rich1/RichHPDSMasterLogList/lvRich2HPDSMaster"};
static const G4String LogVolHpdQWindowNameAnalysis =
                     "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichHPDQuartzWindow";
static const G4String LogVolSiDetNameAnalysis =
                     "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichHPDSiDet";
static const G4String LogVolSiDetNameAnalysisStrPrefix =
                     "/dd/Geometry/BeforeMagnetRegion/Rich1/RichHPDSiDetLogList/lvRichHPDSiDet";
static const G4String Rich1FilterGenericMatName=
                             "/dd/Materials/RichMaterials/Rich1FilterGenericMaterial" ;
static const G4String Rich1FilterD263MatName=
                             "/dd/Materials/RichMaterials/Rich1FilterD263Material" ;
static const G4String Rich1AerogelMatName=
                             "/dd/Materials/RichMaterials/Aerogel" ;
static const G4String Rich1QuartzMatName=
                             "/dd/Materials/RichMaterials/GasWindowQuartz" ;
static const G4String Rich2QuartzMatName=
                             "/dd/Materials/RichMaterials/GasWindowQuartz" ;
static const G4String RichHpdQWMatName=
                            "/dd/Materials/RichMaterials/HpdWindowQuartz" ;
static const G4String RichHpdPhCathMatName=
                            "/dd/Materials/RichMaterials/HpdS20PhCathode" ;
static const G4String RichAirMatName=
                             "/dd/Materials/RichMaterials/RichAir";
static const G4String Rich1NitrogenMatName=
                          "/dd/Materials/RichMaterials/Rich1Nitrogen";
static const G4String Rich2NitrogenMatName=
                          "/dd/Materials/RichMaterials/Rich2Nitrogen";
static const G4String Rich1C4F10MatName=
                          "/dd/Materials/RichMaterials/C4F10";
static const G4String Rich1RadiatorGasMatName=
                          "/dd/Materials/RichMaterials/R1RadiatorGas";
static const G4String Rich1C3F8MatName=
                          "/dd/Materials/RichMaterials/C3F08";
static const G4String RichHpdVacName=
                       "/dd/Materials/RichMaterials/RichHpdVacuum";
static const G4String Rich2CF4MatName=
                          "/dd/Materials/RichMaterials/CF4";
static const G4String Rich2RadiatorGasMatName=
                          "/dd/Materials/RichMaterials/R2RadiatorGas";

static const G4String Rich1PhysVolGeomName ="/dd/Geometry/LHCb/BeforeMagnetRegion/lvBeforeMagnetRegion#pvRich1Master";
static const G4String Rich2PhysVolGeomName ="/dd/Geometry/LHCb/AfterMagnetRegion/lvAfterMagnetRegion#pvRich2Master";

static const G4String Rich1DeStructurePathName = DeRichLocations::Rich1 ;
static const G4String Rich2DeStructurePathName = DeRichLocations::Rich2 ;

static const G4String RichHpdQwDeSubPathName="/Rich1FirstHpdQW";
static const G4String RichHpdPaCathDeSubPathname="/Rich1FirstHpdPhCathode";
static const G4String RichHpdSiDetSensDetNamePrefix="HpdSiliconSensDet";
static const G4String RichPmtQWindowMaterialName="/dd/Materials/RichMaterials/RichPMTQuartzMaterial";
#endif // GAUSSRICH_RICHG4GAUSSPATHNAMES_H

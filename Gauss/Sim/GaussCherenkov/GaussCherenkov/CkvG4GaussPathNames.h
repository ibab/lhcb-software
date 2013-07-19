#ifndef GAUSSCHERENKOV_CKVG4GAUSSPATHNAMES_H
#define GAUSSCHERENKOV_CKVG4GAUSSPATHNAMES_H 1

#include "globals.hh"

#include "RichDet/DeRich.h"




static const G4String RichPmtQuartzWindowMaterialName="/dd/Materials/RichMaterials/RichPMTQuartzMateral";
static const G4String RichPmtHVMatTabPropPath="/dd/Materials/RichMaterialTabProperties/PmtHighVoltage";

static const G4String RichPmtQeffMatTabPropPath="/dd/Materials/RichMaterialTabProperties/NominalPmtQuantumEff";
static const G4String RichPmtBorosilicateQeffMatTabPropPath="/dd/Materials/RichMaterialTabProperties/NominalCBABorosilicatePmtQuantumEff";
static const G4String RichPmtCBABorosilicateQeffMatTabPropPath="/dd/Materials/RichMaterialTabProperties/NominalCBABorosilicatePmtQuantumEff";
static const G4String RichPmtUBABorosilicateQeffMatTabPropPath="/dd/Materials/RichMaterialTabProperties/NominalUBABorosilicatePmtQuantumEff";
static const G4String RichPmtCBAUVGlassQeffMatTabPropPath=
                          "/dd/Materials/RichMaterialTabProperties/NominalCBAUVGlassPmtQuantumEff";
static const G4String RichPmtUBAUVGlassQeffMatTabPropPath=
                          "/dd/Materials/RichMaterialTabProperties/NominalUBAUVGlassPmtQuantumEff";
static const G4String RichPmtSBAUVGlassQeffMatTabPropPath=
                          "/dd/Materials/RichMaterialTabProperties/SBAUVGlassPmtQuantumEff";
static const G4String RichPmtSBABorosilicateQeffMatTabPropPath=
                          "/dd/Materials/RichMaterialTabProperties/SBABorosilicatePmtQuantumEff";
static const G4String Rich1PhDetSupFramePathSeg = "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1PhDetSupFrameH";
static const G4String Rich1MagShPathLogSeg =  "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1MagShH";
static const G4String Rich1QWPathLogSeg = "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1GQuartzW";
static const G4String Rich1PmtModulePathLogSeg= "/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1PmtModuleLogList/lvRichPMTModuleMaster";


static const G4String RichPmtPsfMatTabPropPath="/dd/Materials/RichMaterialTabProperties/PmtPointSpreadFunction";


static const G4String LogVolRich1PhDetSupFrameH0NameAnalysis = "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1PhDetSupFrameH0";
static const G4String LogVolRich1PhDetSupFrameH1NameAnalysis = "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1PhDetSupFrameH1";
static const G4String LogVolRich1MagShH0NameAnalysis = "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1MagShH0";
static const G4String LogVolRich1MagShH1NameAnalysis = "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRich1MagShH1";

static const G4String LogVolRich2N2Encl0NameAnalysis = "/dd/Geometry/AfterMagnetRegion/Rich2/lvRich2HPDN2Encl0";
static const G4String LogVolRich2HPDPanel0NameAnalysis="/dd/Geometry/AfterMagnetRegion/Rich2/lvRich2HPDPanel0";
static const G4String LogVolRich2N2Encl1NameAnalysis = "/dd/Geometry/AfterMagnetRegion/Rich2/lvRich2HPDN2Encl1";
static const G4String LogVolRich2HPDPanel1NameAnalysis="/dd/Geometry/AfterMagnetRegion/Rich2/lvRich2HPDPanel1";





static const G4String RichPmtModuleSuppressSet3Path="/dd/Materials/RichMaterialTabProperties/Rich1PMTModuleAnnulationSet3";
static const G4String RichPmtModuleSuppressSet4Path="/dd/Materials/RichMaterialTabProperties/Rich1PMTModuleAnnulationSet4";
static const G4String RichPmtModuleSuppressSet5Path="/dd/Materials/RichMaterialTabProperties/Rich1PMTModuleAnnulationSet5";
static const G4String RichPmtModuleSuppressSet6Path="/dd/Materials/RichMaterialTabProperties/Rich1PMTModuleAnnulationSet6";





static const G4String LogVolPmtSMasterNameAnalysis =
                     "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTSMaster";
static const G4String LogVolPmtSMasterNameAnalysisStrPrefix =
                      "/dd/Geometry/BeforeMagnetRegion/Rich1/";
static const G4String LogVolPmtSMasterNameAnalysisListStrPrefix [] =
                     {"/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTSMaster",
                      "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTSMaster"};
static const G4String LogVolPmtQWindowNameAnalysis =
                     "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTQuartz";
static const G4String LogVolPmtAnodeNameAnalysis =
                     "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTAnode";
static const G4String LogVolPmtAnodeNameAnalysisStrPrefix =
                     "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTAnode";

static const G4String LogVolLPmtSMasterNameAnalysis =
                     "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichLPMTSMaster";

static const G4String LogVolPmtLensNameAnalysis =
                     "/dd/Geometry/BeforeMagnetRegion/Rich1/lvRichPMTLens";

static const G4String RichPmtQWMatName=
                            "/dd/Materials/RichMaterials/RichPmtQuartzMaterial" ;
static const G4String RichPmtPhCathMatName=
                            "/dd/Materials/RichMaterials/RichPmtPhCathodeMaterial" ;

static const G4String RichPmtVacName=
                       "/dd/Materials/RichMaterials/RichPmtVacuum";

static const G4String RichPmtQwDeSubPathName="/Rich1FirstPmtQW";
static const G4String RichPmtPhCathDeSubPathname="/Rich1FirstPmtPhCathode";
static const G4String RichLPmtQwDeSubPathName="/Rich1FirstLPmtQW";
static const G4String RichLPmtPhCathDeSubPathname="/Rich1FirstLPmtPhCathode";
static const G4String RichLPmtLensDeSubPathName="/Rich1FirstLPmtLens";

static const G4String RichPmtAnodeSensDetNamePrefix="RichPMTAnodeSensDet";


static const G4String RichAfterMagnetRegionStructurePathName="/dd/Structure/LHCb/AfterMagnetRegion";
static const G4String RichBeforeMagnetRegionStructurePathName="/dd/Structure/LHCb/BeforeMagnetRegion";
static const G4String Rich1ClassicDeStructurePathName = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1";
static const G4String Rich2ClassicDeStructurePathName = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2";
static const G4String SuperRichDeStructurePathName = "/dd/Structure/LHCb/AfterMagnetRegion/SuperRich";
static const G4String SuperRichSystemDeStructurePathName="/dd/Structure/LHCb/AfterMagnetRegion/SuperRich/SuperRichSystem";
static const G4String SuperRichLPmtQwDeSubPathName="/SuperRichFirstLPmtQW";
static const G4String SuperRichLPmtPhCathDeSubPathname="/SuperRichFirstLPmtPhCathode";
static const G4String SuperRichLPmtLensDeSubPathName="/SuperRichFirstLPmtLens";
static const G4String SuperRichPhysVolGeomName = "/dd/Geometry/LHCb/AfterMagnetRegion/lvAfterMagnetRegion#pvSuperRichMaster";

#endif // GAUSSCHERENKOV_CKVG4GAUSSPATHNAMES_H

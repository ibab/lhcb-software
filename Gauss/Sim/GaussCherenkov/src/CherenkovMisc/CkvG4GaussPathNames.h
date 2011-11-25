#ifndef MISC_CKVG4GAUSSPATHNAMES_H
#define MISC_CKVG4GAUSSPATHNAMES_H 1

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


static const G4String RichPmtPsfMatTabPropPath="/dd/Materials/RichMaterialTabProperties/PmtPointSpreadFunction";





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



static const G4String RichPmtQWMatName=
                            "/dd/Materials/RichMaterials/RichPmtQuartzMaterial" ;
static const G4String RichPmtPhCathMatName=
                            "/dd/Materials/RichMaterials/RichPmtPhCathodeMaterial" ;

static const G4String RichPmtVacName=
                       "/dd/Materials/RichMaterials/RichPmtVacuum";

static const G4String RichPmtQwDeSubPathName="/Rich1FirstPmtQW";
static const G4String RichPmtPhCathDeSubPathname="/Rich1FirstPmtPhCathode";
static const G4String RichPmtAnodeSensDetNamePrefix="RichPMTAnodeSensDet";

#endif // MISC_CKVG4GAUSSPATHNAMES_H

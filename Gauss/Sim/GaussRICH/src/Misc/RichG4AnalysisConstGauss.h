#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Step.hh"
static const G4double ZUpsRich1Analysis =975.0*mm; 
static const G4double ZDnsRich1Analysis =2200.0*mm;
static const G4double AgelZBeginAnalysis=1110.0*mm;
static const G4double AgelZEndAnalysis=AgelZBeginAnalysis+50.0*mm;
static const G4double C4F10AllZBeginAnalysis=990.0*mm;
static const G4double C4F10ZBeginAnalysis=AgelZEndAnalysis;
static const G4double C4F10ZEndAnalysis=2065.0*mm;
static const G4double AgelXBeginAnalysis=-398.0*mm;
static const G4double AgelXEndAnalysis=398.0*mm;
static const G4double AgelYBeginAnalysis=-281.0*mm;
static const G4double AgelYEndAnalysis=281.0*mm;
static const G4double Rich1AnalysisXHalfSize = 1.25*m;
static const G4double Rich1AnalysisYHalfSize = 1.80*m;
static const G4double HpdSupPlaneTiltcosy=500;
static const G4double HpdSupPlaneTiltcosz=260;
static const G4double HpdSupPlaneYMid=1190.0*mm;
static const G4double CF4AnalysisUpstrXHalfSize= 3000.0*mm;
static const G4double CF4AnalysisDnstrXHalfSize= 3700.0*mm;
static const G4double CF4AnalysisUpstrYHalfSize= 3250.0*mm;
static const G4double CF4AnalysisDnstrYHalfSize= CF4AnalysisUpstrYHalfSize;
static const G4double HpdSupPlaneTilt=
                      atan(HpdSupPlaneTiltcosy/HpdSupPlaneTiltcosz);
static const G4double Rich1ExitWallBeginZAnalysis=2128.0*mm;
static const G4String CF4MaterialName="/dd/Materials/RichMaterials/CF4";
static const G4String C4F10MaterialName="/dd/Materials/RichMaterials/C4F10";
static const G4String AgelMaterialName="/dd/Materials/RichMaterials/Aerogel";
static const G4String FilterGenericMaterialName="/dd/Materials/RichMaterials/Rich1FilterGenericMaterial";
static const G4String FilterD263MaterialName="/dd/Materials/RichMaterials/Rich1FilterD263Material";
static const G4String Rich1GasQWindowMaterialName="/dd/Materials/RichMaterials/GasWindowQuartz";
static const G4String Rich2GasQWindowMaterialName="/dd/Materials/RichMaterials/GasWindowQuartz";

static const G4String LogVolAgelNameAnalysis= 
                              "/dd/Geometry/Rich1/lvRich1Aerogel";
static const G4String LogVolC4F10NameAnalysis=
                               "/dd/Geometry/Rich1/lvRich1SubMaster";
static const G4String LogVolRich1Mirror1NameAnalysis=
                              "/dd/Geometry/Rich1/lvRich1Mirror1";
static const G4String LogVolRich1Mirror2NameAnalysis=
                             "/dd/Geometry/Rich1/lvRich1Mirror2";
static const G4String LogVolRich2Mirror1NameAnalysis=
                              "/dd/Geometry/Rich2/lvRich2SphMirror";
static const G4String LogVolRich2Mirror2NameAnalysis=
                              "/dd/Geometry/Rich2/lvRich2FlatMirrorSeg";
static const G4String LogVolRich1MagShNameAnalysis=
                             "/dd/Geometry/Rich1/lvRich1MagSh" ;
static const G4String LogColRich1GasQWNameAnalysis=
                             "/dd/Geometry/Rich1/lvRich1GQuartzW";
static const G4String LogVolCF4NameAnalysis=
                             "/dd/Geometry/Rich2/lvRich2Gas";
static const G4String LogVolHpdSMasterNameAnalysis =
                     "/dd/Geometry/Rich1/lvRichHPDSMaster";
static const G4String LogVolHpdQWindowNameAnalysis =
                     "/dd/Geometry/Rich1/lvRichHPDQuartzWindow";
static const G4String LogVolSiDetNameAnalysis =
                     "/dd/Geometry/Rich1/lvRichHPDSiDet";
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
static const G4String RichHpdVacName=
                       "/dd/Materials/RichMaterials/RichHpdVacuum";
static const G4String Rich2CF4MatName=
                          "/dd/Materials/RichMaterials/CF4";


static const G4double PhotMomCnv=1243.125;
static const G4double MeVtoeV=1000000;
static const G4double CF4ZBeginAnalysis=9450.0*mm;
static const G4double CF4ZEndAnalysis=11884.0*mm;
static const G4double ZUpsRich2Analysis= CF4ZBeginAnalysis;
static const G4double ZDnsRich2Analysis=CF4ZEndAnalysis;
static const G4double Rich1FiducialLateralTolerence=5.0*mm;
static const G4double Rich1FiducialBeamHoleTolerence=2.0*mm;
static const G4double Rich1FiducialUpstrBeginX =  
         AgelXBeginAnalysis+Rich1FiducialLateralTolerence;
static const G4double Rich1FiducialUpstrEndX =  
        AgelXEndAnalysis-Rich1FiducialLateralTolerence;
static const G4double Rich1FiducialUpstrBeginY =  
        AgelYBeginAnalysis+Rich1FiducialLateralTolerence;
static const G4double Rich1FiducialUpstrEndY =  
        AgelYEndAnalysis-Rich1FiducialLateralTolerence;
static const G4double Rich1AgelFiducialBeginX =Rich1FiducialUpstrBeginX;
static const G4double Rich1AgelFiducialBeginY =Rich1FiducialUpstrBeginY;
static const G4double Rich1AgelFiducialEndX =Rich1FiducialUpstrEndX;
static const G4double Rich1AgelFiducialEndY =Rich1FiducialUpstrEndY;

static const G4double Rich1FiducialUpstrBeamHoleRad = 
        50.0* mm+Rich1FiducialBeamHoleTolerence ;
static const G4double Rich1AgelFiducialUpstrBeamHoleRad = 
        51.0* mm+Rich1FiducialBeamHoleTolerence ;
static const G4double Rich1FiducialDnstrBeginX =
        -820.0*mm+Rich1FiducialLateralTolerence;
static const G4double Rich1FiducialDnstrEndX = 
        820.0*mm-Rich1FiducialLateralTolerence;
static const G4double Rich1FiducialDnstrBeginY = 
        -550.0*mm+Rich1FiducialLateralTolerence;
static const G4double Rich1FiducialDnstrEndY = 
        550.0*mm-Rich1FiducialLateralTolerence;
static const G4double Rich1FiducialDnstrBeamHoleRad = 
        54.0* mm+Rich1FiducialBeamHoleTolerence;
static const G4double Rich1AgelFiducialDnstrBeamHoleRad = 
        52.0* mm+Rich1FiducialBeamHoleTolerence;
static const G4double  Rich1FiducialZTolerence = 5.0*mm;
static const G4double Rich1FiducialUpstrZ = 
          AgelZBeginAnalysis- Rich1FiducialZTolerence;
static const G4double Rich1FiducialDnstrZ =  
         C4F10ZEndAnalysis+ Rich1FiducialZTolerence ;
static const G4double Rich1AgelFiducialUpstrZ = 
          AgelZBeginAnalysis- Rich1FiducialZTolerence;
static const G4double Rich1AgelFiducialDnstrZ =  
         AgelZEndAnalysis+ Rich1FiducialZTolerence ;

static const G4double Rich1FiducialBeamHoleSlope = 
(Rich1FiducialDnstrBeamHoleRad- Rich1FiducialUpstrBeamHoleRad)/
  (Rich1FiducialDnstrZ-Rich1FiducialUpstrZ);
static const G4double Rich1FiducialXslope = 
 (Rich1FiducialDnstrEndX-Rich1FiducialUpstrEndX)/
(Rich1FiducialDnstrZ-Rich1FiducialUpstrZ);
static const G4double Rich1FiducialYslope =
(Rich1FiducialDnstrEndY-Rich1FiducialUpstrEndY) /
(Rich1FiducialDnstrZ-Rich1FiducialUpstrZ);
static const G4double Rich1AgelFiducialBeamHoleSlope = 
(Rich1AgelFiducialDnstrBeamHoleRad- Rich1AgelFiducialUpstrBeamHoleRad)/
  (Rich1AgelFiducialDnstrZ-Rich1AgelFiducialUpstrZ);

static const G4double Rich2FiducialZTolerence= 5.0*mm;
static const G4double Rich2FiducialLateralTolerence=5.0*mm;
static const G4double Rich2FiducialBeamHoleTolerence=2.0*mm;

static const G4double Rich2FiducialUpstrZ=
     ZUpsRich2Analysis - Rich2FiducialZTolerence;
static const G4double Rich2FiducialDnstrZ =
    ZDnsRich2Analysis + Rich2FiducialZTolerence;
static const G4double Rich2FiducialUpstrBeginX=
     -1.0* CF4AnalysisUpstrXHalfSize+Rich2FiducialLateralTolerence;
static const G4double Rich2FiducialUpstrEndX=
     CF4AnalysisUpstrXHalfSize-Rich2FiducialLateralTolerence;
static const G4double Rich2FiducialUpstrBeginY= 
     -1.0*CF4AnalysisUpstrYHalfSize+Rich2FiducialLateralTolerence;
static const G4double Rich2FiducialUpstrEndY=
      CF4AnalysisUpstrYHalfSize-Rich2FiducialLateralTolerence;
static const G4double Rich2FiducialDnstrBeginX= 
      -1.0*CF4AnalysisDnstrXHalfSize+Rich2FiducialLateralTolerence;
static const G4double Rich2FiducialDnstrEndX= 
       CF4AnalysisDnstrXHalfSize-Rich2FiducialLateralTolerence;
static const G4double Rich2FiducialDnstrBeginY= 
       -1.0*CF4AnalysisDnstrYHalfSize+Rich2FiducialLateralTolerence;
static const G4double Rich2FiducialDnstrEndY=
       CF4AnalysisDnstrYHalfSize-Rich2FiducialLateralTolerence;
static const G4double Rich2FiducialUpstrBeamHoleRad=142.0*mm;
static const G4double Rich2FiducialDnstrBeamHoleRad=167.0*mm;
static const G4double Rich2FiducialBeamHoleSlope =
( Rich2FiducialDnstrBeamHoleRad- Rich2FiducialUpstrBeamHoleRad)/
 (Rich2FiducialDnstrZ- Rich2FiducialUpstrZ) ;
static const G4double Rich2FiducialXslope= 
(Rich2FiducialDnstrEndX -  Rich2FiducialUpstrEndX) /
(Rich2FiducialDnstrZ- Rich2FiducialUpstrZ) ;
static const G4double Rich2FiducialYslope= 0.0;
//static const G4double Rich2FiducialYSlope= 
//(Rich2FiducialDnstrEndY - Rich2FiducialUpstrEndY)/
//(Rich2FiducialDnstrZ- Rich2FiducialUpstrZ) ;

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
static const G4double HpdSupPlaneTiltcosy=500;
static const G4double HpdSupPlaneTiltcosz=260;
static const G4double HpdSupPlaneYMid=1190.0*mm;
static const G4double HpdSupPlaneTilt=
                      atan(HpdSupPlaneTiltcosy/HpdSupPlaneTiltcosz);
static const G4String CF4MaterialName="/dd/Materials/RichMaterials/CF4";
static const G4String C4F10MaterialName="/dd/Materials/RichMaterials/C4F10";
static const G4String AgelMaterialName="/dd/Materials/RichMaterials/Aerogel";
static const G4String FilterGenericMaterialName="/dd/Materials/RichMaterials/Rich1FilterGenericMaterial";
static const G4String FilterD263MaterialName="/dd/Materials/RichMaterials/Rich1FilterD263Material";
static const G4String LogVolAgelNameAnalysis= 
                              "/dd/Geometry/Rich1/lvRich1Aerogel";
static const G4String LogVolC4F10NameAnalysis=
                               "/dd/Geometry/Rich1/lvRich1SubMaster";
static const G4String LogVolRich1Mirror1NameAnalysis=
                              "/dd/Geometry/Rich1/lvRich1Mirror1";
static const G4String LogVolRich1Mirror2NameAnalysis=
                             "/dd/Geometry/Rich1/lvRich1Mirror2";
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

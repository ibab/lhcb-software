// class GaussG4UserLimits  implementation
// (based on CMS code by P.Arce)
// 
#include "GaussG4UserLimits.h"
#include <pair.h>

GaussG4UserLimits::GaussG4UserLimits(const G4String& type) 
:fType(type)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
bool GaussG4UserLimits::GetUserLimitSet( const G4int particleCode, ULtype ultype ) const
{
  bool isSet = false;
  pair< mmiul::const_iterator, mmiul::const_iterator> mmpite = fUserLimitSet.equal_range( particleCode );
  mmiul::const_iterator mmite;
  for(mmite = mmpite.first; mmite != mmpite.second; mmite++ ) {
    if( (*mmite).second == ultype ) {
      isSet = true;
      break;
    }
  }

  return isSet;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
void GaussG4UserLimits::SetUserMinEkine(G4double uekinMin, const G4int particleCode )
{
  fUserLimitSet.insert( mmiul::value_type( particleCode, ULMinEkine ) );
  fMinEkine[particleCode] = uekinMin;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
void GaussG4UserLimits::SetUserMaxTime(G4double utimeMax, const G4int particleCode )
{
  fUserLimitSet.insert( mmiul::value_type( particleCode, ULMaxTime ) );
  fMaxTime[particleCode] =utimeMax;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
void GaussG4UserLimits::SetMaxAllowedStep(G4double ustepMax, const G4int particleCode )
{
  fUserLimitSet.insert( mmiul::value_type( particleCode, ULMaxStepLength ) );
  fMaxStepLength[particleCode] =ustepMax;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
void GaussG4UserLimits::SetUserMaxTrackLength(G4double utrakMax, const G4int particleCode )
{
  fUserLimitSet.insert( mmiul::value_type( particleCode, ULMaxTrackLength ) );
  fMaxTrackLength[particleCode] = utrakMax;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
void GaussG4UserLimits::SetUserMinRange(G4double urangMin, const G4int particleCode )
{
  fUserLimitSet.insert( mmiul::value_type( particleCode, ULMinRange ) );
  fMinRange[particleCode] = urangMin;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
G4double GaussG4UserLimits::GetUserMinEkine(const G4Track& aTrack)
{
  return GetCut( aTrack, fMinEkine, true );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
G4double GaussG4UserLimits::GetUserMaxTime(const G4Track& aTrack)
{
  return GetCut( aTrack, fMaxTime, false );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
G4double GaussG4UserLimits::GetMaxAllowedStep(const G4Track& aTrack)
{
  return GetCut( aTrack, fMaxStepLength, false );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
G4double GaussG4UserLimits::GetUserMaxTrackLength(const G4Track& aTrack)
{
  return GetCut( aTrack, fMaxTrackLength, false );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
G4double GaussG4UserLimits::GetUserMinRange(const G4Track& aTrack)
{
  return GetCut( aTrack, fMinRange, true );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
G4double GaussG4UserLimits::GetCut(const G4Track& aTrack, const mid& partCutMap, const bool cutBelow )
{
  double cutval;
  G4int partCode = aTrack.GetDefinition()->GetPDGEncoding();
  //---- Look if cut exists for this particle
  mid::const_iterator ite = partCutMap.find( partCode );
  if( ite != partCutMap.end() ) {
    cutval = (*ite).second;
  } else {
  //---- Look if cut exists for all particles (!!not used)
    ite = partCutMap.find(0); 
    if( ite != partCutMap.end() ) {
      cutval = (*ite).second;
    } else {
      //if cut does not exists, return maximum value
      if( cutBelow ) {
	cutval = 0.;
      } else { 
	cutval = kInfinity;
      }
    }
  }
  return cutval;

}


string GetULName( ULtype ulType )
{
  string ulName;
  switch (ulType){
  case ULMinEkine:
    ulName = "MinEkineCut";
    break;
  case ULMaxTime: 
    ulName = "MaxTimeCut";
    break;
  case ULMaxStepLength:
    ulName = "MaxStepLengthCut";
    break;
  case ULMaxTrackLength:
    ulName = "MaxTrackLengthCut";
    break;
  case ULMinRange:
    ulName = "MinRangeCut";
    break;
  default:
    cerr << "!!! FATAL ERROR: UserLimitsProcessCreator:SetUserSpecialCuts invalid ULtype " << ulType << endl;
    abort();
  }
  return ulName; 
}

// class GaussG4UserLimits  implementation
// (based on CMS code by P.Arce)
// 
#include "GaussG4UserLimits.h"
#include <map>

GaussG4UserLimits::GaussG4UserLimits(const G4String& type) 
:fType(type)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
bool GaussG4UserLimits::GetUserLimitSet( const G4int particleCode, ULtype ultype ) const
{
  bool isSet = false;
  std::pair< mmiul::const_iterator, mmiul::const_iterator> mmpite = fUserLimitSet.equal_range( particleCode );
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
  if( ite != partCutMap.end() ) 
    {
      cutval = (*ite).second;
    } 
  else 
    {
      //---- Look if cut exists for all particles (pid=0) 
      ite = partCutMap.find(0);    
      
      if( ite != partCutMap.end() ) {
        cutval = (*ite).second;
      } 
      else 
        {
          //if cut does not exists, return maximum value
          if( cutBelow ) {
            cutval = 0.;
          } 
          else 
            { 
              cutval = kInfinity;
            }
        }
    }
  return cutval;
}

/** comparison operator for GaussG4UserLimits class 
 *  @param ul1 the first UL to be compared 
 *  @param ul2 the second UL to be compared 
 *  @return the result of coomaprison operation
 */
bool operator== ( const GaussG4UserLimits& ul1 , 
                  const GaussG4UserLimits& ul2 )
{       
  // the same object 
  if ( &ul1                == &ul2                ) { return true  ; } 
  if (  ul1.fMaxStepLength !=  ul2.fMaxStepLength ) { return false ; }
  if (  ul1.fMaxTime       !=  ul2.fMaxTime       ) { return false ; }
  if (  ul1.fMinEkine      !=  ul2.fMinEkine      ) { return false ; }
  if (  ul1.fMinRange      !=  ul2.fMinRange      ) { return false ; }
  if (  ul1.fUserLimitSet  !=  ul2.fUserLimitSet  ) { return false ; }
  if (  ul1.fType          !=  ul2.fType          ) { return false ; }
  return true ;
}

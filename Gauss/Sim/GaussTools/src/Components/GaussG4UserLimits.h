// class GaussG4UserLimits
// (based on CMS code by P.Arce)
//
#ifndef GaussG4UserLimits_h
#define GaussG4UserLimits_h
#include <string>

enum ULtype {ULMinEkine, ULMaxTime, ULMaxStepLength, ULMaxTrackLength, ULMinRange};

#include "globals.hh"
#include "G4UserLimits.hh" 
#include <map>

class G4Track;
typedef std::map<int,double> mid;
typedef std::multimap<int, ULtype> mmiul;

class GaussG4UserLimits : public G4UserLimits
{
  friend bool operator==( const GaussG4UserLimits& , 
                          const GaussG4UserLimits& );
  friend bool operator!=( const GaussG4UserLimits& , 
                          const GaussG4UserLimits& );
public:  // with description

  GaussG4UserLimits(const G4String& type );
  virtual ~GaussG4UserLimits();

public:  // with description

  const G4String & GetType() const;
  void  SetType(const G4String& type);
  // Set/Get type name for UserLimits.
  
  virtual G4double GetMaxAllowedStep(const G4Track&);  
  virtual G4double GetUserMaxTrackLength(const G4Track&);
  virtual G4double GetUserMaxTime (const G4Track&);
  virtual G4double GetUserMinEkine(const G4Track&);
  virtual G4double GetUserMinRange(const G4Track&);
  
  virtual void SetMaxAllowedStep(G4double ustepMax, const G4int particleCode );
  virtual void SetUserMaxTrackLength(G4double utrakMax, const G4int particleCode );
  virtual void SetUserMaxTime(G4double utimeMax, const G4int particleCode );
  virtual void SetUserMinEkine(G4double uekinMin, const G4int particleCode );
  virtual void SetUserMinRange(G4double urangMin, const G4int particleCode );

  bool GetUserLimitSet( const G4int particleCode, ULtype ultype ) const;

private:
  G4double GetCut(const G4Track& aTrack, const mid& partCutMap, const bool cutBelow );
  
protected:  // with description

  mid fMaxStepLength;     // max allowed Step size in this volume per particle
  mid fMaxTrackLength;    // max total track length per particle
  mid fMaxTime;     // max time per particle
  mid fMinEkine;    // min kinetic energy per particle
  mid fMinRange;    // min remaining range per particle (only for charged particles) 
  // Keep track of which user limits are set, so that only the necessary processes are created. This will avoid unnecessary invokations to processes at each G4Step
  mmiul fUserLimitSet;

  G4String fType;
};

#include "GaussG4UserLimits.icc"

/** comparison operator for GaussG4UserLimits class 
 *  @param ul1 the first UL to be compared 
 *  @param ul2 the second UL to be compared 
 *  @return the result of coomaprison operation
 */
bool        operator== ( const GaussG4UserLimits& ul1 , 
                         const GaussG4UserLimits& ul2 ) ;

/** comparison operator for GaussG4UserLimits class 
 *  @param ul1 the first UL to be compared 
 *  @param ul2 the second UL to be compared 
 *  @return the result of coomaprison operation
 */
inline bool operator!= ( const GaussG4UserLimits& ul1 , 
                         const GaussG4UserLimits& ul2 ) 
{ return !(ul1 == ul2) ; }


#endif

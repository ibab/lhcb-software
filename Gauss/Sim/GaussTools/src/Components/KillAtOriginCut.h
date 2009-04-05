// $Id: KillAtOriginCut.h,v 1.1 2009-04-05 17:40:47 gcorti Exp $
#ifndef GAUSSTOOLS_COMPONENTS_KILLATORIGINCUT_H 
#define GAUSSTOOLS_COMPONENTS_KILLATORIGINCUT_H 1

// Include files
#include "G4ios.hh"
#include "globals.hh"
#include "SpecialCuts.h"

/** @class KillAtOriginCut KillAtOriginCut.h Components/KillAtOriginCut.h
 *  
 *  Special process to kill particles at origin, i.e. they are not
 *  tracked at all.
 *  
 *  @author Gloria CORTI
 *  @date   2009-03-26
 */
namespace GiGa 
{
  
  class KillAtOriginCut : public SpecialCuts {
  public: 
    /// Cut constructor
    KillAtOriginCut(const G4String& processName ="KillAtOriginCut");
    
    virtual ~KillAtOriginCut( ); ///< Destructor

    virtual G4double PostStepGetPhysicalInteractionLength(
                             const G4Track& track,
                             G4double   previousStepSize,
                             G4ForceCondition* condition
                             );
            
    virtual G4VParticleChange* PostStepDoIt( const G4Track&, const G4Step& );

    void printCounters();

  protected:

  private:

    // hide assignment operator as private 
    KillAtOriginCut(KillAtOriginCut&);
    KillAtOriginCut& operator=(const KillAtOriginCut& right);

    /// Counter to monitor how many particle of a given type are killed    
    std::map<std::string, int> m_counters; 

};
  
} // closes namespace

#endif // GAUSSTOOLS_COMPONENTS_KILLATORIGINCUT_H

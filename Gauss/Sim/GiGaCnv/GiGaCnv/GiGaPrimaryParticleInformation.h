// $Id: GiGaPrimaryParticleInformation.h,v 1.1 2004-02-14 08:33:39 robbep Exp $
#ifndef GIGACNV_GIGAPRIMARYPARTICLEINFORMATION_H 
#define GIGACNV_GIGAPRIMARYPARTICLEINFORMATION_H 1

// Include files

#include "G4VUserPrimaryParticleInformation.hh"
#include "G4ios.hh"

/** @class GiGaPrimaryParticleInformation GiGaPrimaryParticleInformation.h 
    GiGaCnv/GiGaPrimaryParticleInformation.h
 *  
 *  Class to hold oscillation information to be passed along with
 *  a G4PrimaryParticle
 *  @author Patrick Robbe
 *  @date   2004-02-06
 */
class GiGaPrimaryParticleInformation : 
  public G4VUserPrimaryParticleInformation {
public:
  /// Standard constructor
  GiGaPrimaryParticleInformation( bool osc ) : 
    G4VUserPrimaryParticleInformation ( ), m_hasOscillated( osc ) { ; }  

  virtual ~GiGaPrimaryParticleInformation( ) {;} ///< Destructor

  bool hasOscillated( ) { return m_hasOscillated ; }

  virtual void Print( ) const { if ( m_hasOscillated ) 
    G4cout << "Particle has oscillated" << G4endl ; 
  }
  

protected:

private:
  bool m_hasOscillated ;
};
#endif // GIGACNV_GIGAPRIMARYPARTICLEINFORMATION_H

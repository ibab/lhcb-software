// $Id: GiGaPrimaryParticleInformation.h,v 1.2 2004-02-20 19:11:59 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GIGACNV_GIGAPRIMARYPARTICLEINFORMATION_H 
#define GIGACNV_GIGAPRIMARYPARTICLEINFORMATION_H 1
// ============================================================================
// Include files
// ============================================================================
// GiGa 
// ============================================================================
#include "GiGa/GiGaUtil.h"
// ============================================================================
// G4 
// ============================================================================
#include "G4VUserPrimaryParticleInformation.hh"
#include "G4ios.hh"
// ============================================================================

/** @class GiGaPrimaryParticleInformation
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
// ============================================================================

// ============================================================================
/** simple function for safe/fast casting
 *  @param info pointer to G4VUserPrimaryParticleInformation
 *  @return result of "fast cast" procedure 
 *  @see GiGaUtil::FastCast
 */
// ============================================================================
inline GiGaPrimaryParticleInformation* 
gigaPrimaryParticleInformation
( G4VUserPrimaryParticleInformation* info ) 
{
  if( 0 == info ) { return 0 ; }
  GiGaUtil::FastCast
    <
    G4VUserPrimaryParticleInformation ,
    GiGaPrimaryParticleInformation    
    >                                 cast;
  return cast( info );
};
// ============================================================================

// ============================================================================
/** simple function for safe/fast casting
 *  @param info pointer to G4VUserPrimaryParticleInformation
 *  @return result of "fast cast" procedure 
 *  @see GiGaUtil::FastCast
 */
// ============================================================================
inline const GiGaPrimaryParticleInformation* 
gigaPrimaryParticleInformation
( const G4VUserPrimaryParticleInformation* info ) 
{
  if( 0 == info ) { return 0 ; }
  GiGaUtil::FastCast
    <
    const G4VUserPrimaryParticleInformation ,
    const GiGaPrimaryParticleInformation
    >                                         cast;
  return cast( info );
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif // GIGACNV_GIGAPRIMARYPARTICLEINFORMATION_H
// ============================================================================


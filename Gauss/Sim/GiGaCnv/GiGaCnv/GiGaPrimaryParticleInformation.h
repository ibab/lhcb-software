// $Id: GiGaPrimaryParticleInformation.h,v 1.6 2008-10-20 12:31:41 robbep Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2006/01/31 10:39:33  gcorti
// adapt to event model
//
// Revision 1.4  2005/01/17 18:14:40  robbep
// Use generator energy instead of Geant4 energy for short lived particles.
//
// Revision 1.3  2004/04/07 15:47:55  gcorti
// signal info, extended collision, new vertex types
//
// Revision 1.2  2004/02/20 19:11:59  ibelyaev
//  upgrade for newer GiGa
// 
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
// Event
#include "Event/HepMCEvent.h"

namespace LHCb {
  class MCParticle ;
}

/** @class GiGaPrimaryParticleInformation
 *  
 *  Class to hold oscillation and signal information to be passed along with
 *  a G4PrimaryParticle
 *  @author Patrick Robbe
 *  @date   2004-02-06
 */
class GiGaPrimaryParticleInformation : 
  public G4VUserPrimaryParticleInformation {
public:
  /// Standard constructors
  /// sets oscillation information
  GiGaPrimaryParticleInformation( bool osc ) : 
    G4VUserPrimaryParticleInformation ( ),
    m_hasOscillated(  osc  ) , 
    m_isSignal     ( false ) ,
    m_signalBarcode( -1    ) ,
    m_pHepMCEvent  (   0   ) , 
    m_mcParticle   (   0   ) { ; }  

  /// Sets link to HepMC Particle
  GiGaPrimaryParticleInformation( bool sig, int code, LHCb::HepMCEvent *event ) :
    G4VUserPrimaryParticleInformation ( ),
    m_hasOscillated ( false ),
    m_isSignal      (  sig  ),
    m_signalBarcode ( code  ),
    m_pHepMCEvent   ( event ),
    m_mcParticle    (   0   ) { ; }

  
  virtual ~GiGaPrimaryParticleInformation( ) {;} ///< Destructor
  
  /// Sets oscillation information
  void setHasOscillated( bool osc ) { m_hasOscillated = osc ; }
  
  /// Sets signal information
  void setIsSignal( bool sig ) { m_isSignal = sig ; }
  
  /// Sets signal barcode
  void setSignalBarcode( int code ) { m_signalBarcode = code ; }

  /// Sets HepMCEvent pointer
  void setHepMCPointer( LHCb::HepMCEvent * event ) { m_pHepMCEvent = event ; }
  
  /// Sets pointer to mother MCParticle
  void setMotherMCParticle( LHCb::MCParticle * mcp ) { m_mcParticle = mcp ; }
  
  /// returns true if the particle has oscillated  
  bool hasOscillated( ) { return m_hasOscillated ; }
  
  /// returns true is the particle is the forced decay particle
  bool isSignal( ) { return m_isSignal ; }
  
  /// returns the barcode of the signal particle
  int  signalBarcode( ) { return m_signalBarcode ; }

  /// returns a pointer to the HepMC Event containing the signal HepMC particle
  LHCb::HepMCEvent * pHepMCEvent ( ) { return m_pHepMCEvent ; }
  
  /// Returns a pointer to the mother MCParticle
  LHCb::MCParticle * motherMCParticle( ) { return m_mcParticle ; }
  
  /// Print function needed in G4VUserPrimaryParticleInformation   
  virtual void Print( ) const { if ( m_hasOscillated ) 
    G4cout << "Particle has oscillated" << G4endl ; 
  }
  
  
protected:

private:
  /// Flag to indicate if particle has oscillated
  bool m_hasOscillated ;
  /// Flag to indicate if particle is the signal particle
  bool m_isSignal ;
  /// Corresponding HepMC Particle barcode (filled for signal particle)
  int m_signalBarcode ;
  /// Pointer to HepMCEvent which the HepMC particle belongs to
  LHCb::HepMCEvent * m_pHepMCEvent ;
  /// Pointer to the MCParticle which is the mother of this G4PrimaryParticle
  LHCb::MCParticle * m_mcParticle ;
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
}


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
}


// ============================================================================
// The END 
// ============================================================================
#endif // GIGACNV_GIGAPRIMARYPARTICLEINFORMATION_H



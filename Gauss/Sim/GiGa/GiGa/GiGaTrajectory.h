// $Id: GiGaTrajectory.h,v 1.24 2008-10-20 10:24:48 robbep Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.23  2005/12/16 19:50:27  gcorti
// adapt to LHCb namespace
//
// Revision 1.22  2004/04/07 15:23:08  gcorti
// transfer signal info
//
// Revision 1.21  2004/02/22 19:01:50  ibelyaev
//  add new data fields to GiGaTrajectoryPoint
//
// Revision 1.20  2004/02/20 18:13:34  ibelyaev
//  major update in GiGaBase and GiGaTrajectory
//
// ============================================================================
#ifndef    GIGA_GIGATRAJECTORY_H
#define    GIGA_GIGATRAJECTORY_H 1
// ============================================================================
/// STL
#include <vector>
/// base "interface" class
#include "G4VTrajectory.hh"
/// GiGa
#include "GiGa/GiGaTrajectoryPoint.h"
///
class G4ParticleDefinition;
class G4Track    ;
class G4Step     ;
class G4VProcess ;
namespace LHCb {
  class HepMCEvent ;
  class MCParticle ;
}

///

/** @class GiGaTrajectory GiGaTrajectory.h GiGa/GiGaTrajectory.h
 *
 *  Customisation of "standard" G4Trajectory class to
 *  incorporate TimeOfFlight of the each point.
 *  Overwise conversion to MCParticle/MCVertex structure is impossible.
 *
 *  @author  Vanya Belyaev
 *  @date    22/02/2001
*/

class GiGaTrajectory: public G4VTrajectory                    ,
                      public std::vector<GiGaTrajectoryPoint*>
{
public:

  /// default (empty) constructor
  GiGaTrajectory (                              );

  /** constructor from the track
   *  @param track pointer to the track
   */
  GiGaTrajectory ( const G4Track*         track );

  /** copy constructor
   *  @param right object to be copied
   */
  GiGaTrajectory ( const GiGaTrajectory & right );

  /// destructor
  virtual ~GiGaTrajectory();

  ///  overloaded new operator
  void* operator new    ( size_t ) ;

  ///  overloaded delete operator
  void  operator delete ( void*  ) ;

  /** clone (virtual constructor) method
   *  @return clone of current trajectory
   */
  virtual GiGaTrajectory* clone() const ;

  /// comparison (needed by G4)
  inline int operator == (const GiGaTrajectory& right) const
  {return ( &right == this );}

  /// accessors - a'la Gaudi and a'la Geant4
  inline const int&                  trackID      () const
  { return m_trackID      ; }
  inline const int&                  parentID     () const
  { return m_parentID     ; }
  inline const CLHEP::HepLorentzVector&     fourMomentum () const
  { return momentum()     ; }
  inline const CLHEP::HepLorentzVector&     momentum     () const
  { return m_4vect        ; }
  inline const CLHEP::HepLorentzVector&     Get4Momentum () const
  { return fourMomentum() ; }
  ///
  bool                               hasOscillated() const
  { return m_hasOscillated ; }
  bool                               isSignal     () const
  { return m_isSignal      ; }
  int                                signalBarcode() const
  { return m_signalBarcode ; }
  LHCb::HepMCEvent *                 pHepMCEvent  () const
  { return m_pHepMCEvent   ; }
  LHCb::MCParticle *                 motherMCParticle() const
  { return m_motherMCParticle ; }

  ///
  G4int                              GetTrackID   () const
  { return trackID ()     ; }
  G4int                              GetParentID  () const
  { return parentID()     ; }

  inline const G4ParticleDefinition* partDef      () const
  { return m_partDef ; }
  ///
  virtual void ShowTrajectory  ( std::ostream& os = std::cout ) const;

  virtual void DrawTrajectory  ( G4int i_mode=0 ) const ;
  virtual void AppendStep      ( const G4Step*  )       ;
  virtual void MergeTrajectory ( G4VTrajectory* )       ;
  ///
  virtual int                 GetPointEntries(          ) const
  { return size() ; }
  virtual G4VTrajectoryPoint* GetPoint       ( int index ) const
  { return point( index ) ; }

  G4VTrajectoryPoint*         point          ( int index ) const
  { return (size_t) index < size() ? *(begin()+index) : 0 ; }


  virtual G4String      GetParticleName    () const ;
  virtual G4double      GetCharge          () const ;
  virtual G4int         GetPDGEncoding     () const ;
  virtual G4ThreeVector GetInitialMomentum () const ;

  /// get the pointer to creator process
  inline  const G4VProcess* creator() const { return m_creator ; }

  /// set the creator process
  inline  void           setCreator( const G4VProcess* value )
  { m_creator = value ; }

  inline void setHasOscillated( bool flag )
  {  m_hasOscillated = flag ;  }

  inline void setIsSignal( bool sig )
  {  m_isSignal = sig ; }

  inline void setSignalBarcode( int code )
  {  m_signalBarcode = code ; }

  inline void setHepMCEvent( LHCb::HepMCEvent * event )
  {  m_pHepMCEvent = event ;  }

  inline void setMotherMCParticle( LHCb::MCParticle * p )
  { m_motherMCParticle = p ; }

  // get the name of the creator process
  const std::string& processname() const ;

  void setProcessName ( const std::string& ) {}

protected:

  /** 'almost' unconditionally append the step
   *  @param step  step to be appended
   *  @return flag
   */
  bool appendStep   ( const G4Step* step ) ;

private:
  /// assignement operator id private
  GiGaTrajectory& operator=( const GiGaTrajectory& );
private:
  ///
  int                          m_trackID       ;
  int                          m_parentID      ;
  const G4ParticleDefinition*  m_partDef       ;
  CLHEP::HepLorentzVector             m_4vect         ;
  const G4VProcess*            m_creator       ;
  bool                         m_hasOscillated ;
  bool                         m_isSignal      ;
  int                          m_signalBarcode ;
  LHCb::HepMCEvent*            m_pHepMCEvent   ;
  LHCb::MCParticle *           m_motherMCParticle ;
  ///
};
// ============================================================================

// ============================================================================
/** @fn  gigaTrajectory
 *  Fast cast of G4VTrajectory interface to concrete GiGa implementation
 *  @param  g4   pointer to G4VTrajectory interface
 *  @return cast (dynamic or static) to GiGaTrajectory
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline GiGaTrajectory* gigaTrajectory( G4VTrajectory* g4 )
{
  GiGaUtil::FastCast<G4VTrajectory,GiGaTrajectory> cast ;
  return cast( g4  );
}
// ============================================================================

// ============================================================================
/** @fn  gigaTrajectory
 *  Fast cast of G4VTrajectory interface to concrete GiGa implementation
 *  @param  g4   pointer to G4VTrajectory interface
 *  @return cast (dynamic or static) to GiGaTrajectory
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline const GiGaTrajectory* gigaTrajectory( const G4VTrajectory* g4 )
{
  GiGaUtil::FastCast<const G4VTrajectory,const GiGaTrajectory> cast ;
  return cast( g4  );
}
// ============================================================================


// ============================================================================
#endif  ///< GIGA_GIGATRAJECTORY_H
// ============================================================================








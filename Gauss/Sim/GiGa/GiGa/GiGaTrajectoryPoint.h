// $Id: GiGaTrajectoryPoint.h,v 1.14 2009-10-20 10:06:33 silviam Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.13  2004/02/22 19:01:51  ibelyaev
//  add new data fields to GiGaTrajectoryPoint
//
// Revision 1.12  2002/12/07 21:36:30  ibelyaev
//  trivial fix
//
// Revision 1.11  2002/12/07 21:05:30  ibelyaev
//  see $GIGAROOT/doc/release.notes 2002-12-07
//
// Revision 1.10  2002/12/07 14:27:51  ibelyaev
//  see $GIGAROOT/cmt/requirements file
//
// ============================================================================
#ifndef     GIGA_GIGATRAJECTORYPOINT_H
#define     GIGA_GIGATRAJECTORYPOINT_H 1
// ============================================================================
// CLHEP
#include "CLHEP/Vector/LorentzVector.h"
// GiGa
#include "GiGa/GiGaUtil.h"
// G4
#include "G4TrajectoryPoint.hh"
#include "G4VTrajectoryPoint.hh"

class G4VProcess ;

/** @class GiGaTrajectoryPoint GiGaTrajectoryPoint.h
 *
 *  Customisation of "standard" G4TrajectoryPoint to incorporate TimeOfFlight
 *  of the point. Overwise conversion to MCVertex class is impossible.
 *
 *  @author  Vanya Belyaev
 *  @date    22/02/2001
 */

class GiGaTrajectoryPoint: public G4TrajectoryPoint
{
public:

  /// default empty constructor
  GiGaTrajectoryPoint ();

  /** constructor from position and time, momentum and process
   *  @param Pos  position
   *  @param Time time
   *  @param Mom  input momentum
   *  @param Proc process
   */
  GiGaTrajectoryPoint
  ( const CLHEP::Hep3Vector&       Pos                       ,
    const double            Time                      ,
    const CLHEP::HepLorentzVector& Mom  = CLHEP::HepLorentzVector() ,
    const G4VProcess*       Proc = 0                  );

  /** constructor from position and time
   *  @param Time time
   *  @param Pos position
   *  @param Mom  input momentum
   *  @param Proc process
   */
  GiGaTrajectoryPoint
  ( const double            Time                      ,
    const CLHEP::Hep3Vector&       Pos                       ,
    const CLHEP::HepLorentzVector& Mom  = CLHEP::HepLorentzVector() ,
    const G4VProcess*       Proc = 0                  );

  /** constructor from 4-vector
   *  @param right 4-vector
   *  @param Mom  input momentum
   *  @param Proc process
   */
  GiGaTrajectoryPoint
  ( const CLHEP::HepLorentzVector& right                     ,
    const CLHEP::HepLorentzVector& Mom  = CLHEP::HepLorentzVector() ,
    const G4VProcess*       Proc = 0                  );

  /** constructor from G4TrajectoryPoint
   *  @param point trajectory point
   *  @param Time time
   *  @param Mom   input momentum
   *  @param Proc  process
   */
  GiGaTrajectoryPoint
  ( const G4TrajectoryPoint& Point                      ,
    const double             Time                       ,
    const CLHEP::HepLorentzVector&  Mom   = CLHEP::HepLorentzVector() ,
    const G4VProcess*        Proc  = 0                  );

  /** constructor from G4VTrajectoryPoint
   *  @param point trajectory point
   *  @param Time time
   *  @param Mom   input momentum
   *  @param Proc  process
   */
  GiGaTrajectoryPoint
  ( const G4VTrajectoryPoint& Point                      ,
    const double              Time                       ,
    const CLHEP::HepLorentzVector&   Mom   = CLHEP::HepLorentzVector() ,
    const G4VProcess*         Proc  = 0                  );

  /** copy constructor
   *  @param right object to be copied
   */
  GiGaTrajectoryPoint ( const GiGaTrajectoryPoint& right );

  /// destructor (virtual)
  virtual ~GiGaTrajectoryPoint();

  /** clone method (virtual constructor)
   *  @return new trajectory point
   */
  GiGaTrajectoryPoint*    clone      () const ;

  /// overloaded "new"
  void* operator new    ( size_t );

  /// overloaded "delete"
  void  operator delete ( void*  );

  /// comparison,needed by G4
  bool operator==( const GiGaTrajectoryPoint& right )
  { return (&right) == this ; } /// ?

  /// get the time
  double    time () const { return m_time   ; }
  /// get the time
  double GetTime () const { return   time() ; }
  /// set the time
  void setTime    ( double Time ) { m_time = Time ; }
  /// get the 4-position
  const CLHEP::HepLorentzVector    fourVector () const
  { return CLHEP::HepLorentzVector ( GetPosition() , GetTime() ) ; };

  /// get the 4-position
  const CLHEP::HepLorentzVector GetFourVector () const { return fourVector ()  ; };

  /// get the actual 'input' 4-momentum of the particle
  const CLHEP::HepLorentzVector&    momentum() const { return m_momentum    ; }

  /// get the actual 'input' 4-momentum of the particle
  const CLHEP::HepLorentzVector& GetMomentum() const { return   momentum () ; }

  /// set new value for input 4-momentum
  void setMomentum ( const CLHEP::HepLorentzVector& value ) {  m_momentum = value   ; }

  /// set new value for input 4-momentum
  void SetMomentum ( const CLHEP::HepLorentzVector& value ) { setMomentum ( value ) ; }

  /** get the process which depermines the position
   *  of the trajectory point(vertex)
   *  for th efitrst point if th etrajectory it is
   *  a "creator" process for the track
   *  otherwise it is process whoch determinies the step
   */
  const G4VProcess*    process() const { return m_process    ; }

  /** get the process which depermines the position
   *  of the trajectory point(vertex)
   *  for th efitrst point if th etrajectory it is
   *  a "creator" process for the track
   *  otherwise it is process whoch determinies the step
   */
  const G4VProcess* GetProcess() const { return   process () ; }

  /// set new valeu of the process
  void setProcess ( const G4VProcess* value ) {  m_process = value   ; }
  /// set new valeu of the process
  void SetProcess ( const G4VProcess* value ) { setProcess ( value ) ; }

private:

  // assigmenet oprator is disabled
  GiGaTrajectoryPoint& operator=
  ( const GiGaTrajectoryPoint& right ) ;

private:
  ///
  double m_time;
  CLHEP::HepLorentzVector  m_momentum ;
  const G4VProcess* m_process  ;
  ///
};
// ============================================================================

// ============================================================================
/** @fn  gigaTrajectoryPoint
 *  Fast cast of G4TrajectoryPoint interface to concrete Gauss implementation
 *  @param  g4   pointer to G4TrajectoryPoint interface
 *  @return cast (dynamic or static) to GaussTrajectory
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline GiGaTrajectoryPoint* gigaTrajectoryPoint( G4TrajectoryPoint* g4 )
{
  GiGaUtil::FastCast<G4TrajectoryPoint,GiGaTrajectoryPoint> cast ;
  return cast( g4 );
}
// ============================================================================

// ============================================================================
/** @fn  gigaTrajectoryPoint
 *  Fast cast of G4VTrajectoryPoint interface to concrete Gauss implementation
 *  @param  g4   pointer to G4TrajectoryPoint interface
 *  @return cast (dynamic or static) to GaussTrajectory
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline GiGaTrajectoryPoint* gigaTrajectoryPoint( G4VTrajectoryPoint* g4 )
{
  GiGaUtil::FastCast<G4VTrajectoryPoint,GiGaTrajectoryPoint> cast ;
  return cast( g4 );
}
// ============================================================================

// ============================================================================
/** @fn  gigaTrajectoryPoint
 *  Fast cast of G4TrajectoryPoint interface to concrete Gauss implementation
 *  @param  g4   pointer to G4TrajectoryPoint interface
 *  @return cast (dynamic or static) to GaussTrajectory
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline const GiGaTrajectoryPoint* gigaTrajectoryPoint
( const G4TrajectoryPoint* g4 )
{
  GiGaUtil::FastCast<const G4TrajectoryPoint,const GiGaTrajectoryPoint> cast ;
  return cast( g4 );
}
// ============================================================================

// ============================================================================
/** @fn  gigaTrajectoryPoint
 *  Fast cast of G4VTrajectoryPoint interface to concrete Gauss implementation
 *  @param  g4   pointer to G4TrajectoryPoint interface
 *  @return cast (dynamic or static) to GaussTrajectory
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline const GiGaTrajectoryPoint* gigaTrajectoryPoint
( const G4VTrajectoryPoint* g4 )
{
  GiGaUtil::FastCast<const G4VTrajectoryPoint,const GiGaTrajectoryPoint> cast ;
  return cast( g4 );
}
// ============================================================================


// ============================================================================
#endif  ///<  GIGA_GIGATRAJECTORYPOINT_H
// ============================================================================













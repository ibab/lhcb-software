// $Id: GiGaTrajectoryPoint.h,v 1.12 2002-12-07 21:36:30 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
  ///
public:
  ///

  /// default empty constructor
  GiGaTrajectoryPoint ();
  
  /** constructor from position and time 
   *  @param Pos position 
   *  @param Time time 
   */
  GiGaTrajectoryPoint ( const Hep3Vector&  Pos   , const double&      Time );

  /** constructor from position and time 
   *  @param Time time 
   *  @param Pos position 
   */
  GiGaTrajectoryPoint ( const double&      Time  , const Hep3Vector&  Pos  );

  /** constructor from 4-vector
   *  @param right 4-vector
   */
  GiGaTrajectoryPoint ( const HepLorentzVector&    right );
  
  /** copy constructor 
   *  @param right object to be copied 
   */
  GiGaTrajectoryPoint ( const GiGaTrajectoryPoint& right ); 
  
  /// destructor (virtual)
  virtual ~GiGaTrajectoryPoint();

  /** clone method (virtual constructor)
   *  @return new trajectory point
   */
  GiGaTrajectoryPoint*    clone      () const 
  { return new GiGaTrajectoryPoint( *this ); }
  
  /// overloaded "new"
  void* operator new    ( size_t );
  
  /// overloaded "delete"
  void  operator delete ( void*  );
  
  /// comparison,needed by G4 
  bool operator==( const GiGaTrajectoryPoint& right ) 
  { return (&right) == this ; } /// ?
  
  ///
  inline const double&           time       () const        
  { return m_time                ; } 
  inline const double&           GetTime    () const        
  { return time()                ; } 
  inline GiGaTrajectoryPoint&    setTime    ( double Time ) 
  { m_time = Time ; return *this ; }  
  inline const HepLorentzVector  fourVector () const        
  { return HepLorentzVector( GetPosition() , GetTime() ) ; };  
  ///
private:
  ///
  double m_time; 
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
};
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
inline const GiGaTrajectoryPoint* gigaTrajectoryPoint
( const G4TrajectoryPoint* g4 )
{
  GiGaUtil::FastCast<const G4TrajectoryPoint,const GiGaTrajectoryPoint> cast ;
  return cast( g4 );
};
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
};
// ============================================================================


// ============================================================================
#endif  ///<  GIGA_GIGATRAJECTORYPOINT_H
// ============================================================================













// $Id: GiGaTrajectoryPoint.h,v 1.10 2002-12-07 14:27:51 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef     GIGA_GIGATRAJECTORYPOINT_H
#define     GIGA_GIGATRAJECTORYPOINT_H 1 
// ============================================================================
// CLHEP 
#include "CLHEP/Vector/LorentzVector.h"
// G4 
#include "G4TrajectoryPoint.hh" 


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
#endif  ///<  GIGA_GIGATRAJECTORYPOINT_H
// ============================================================================













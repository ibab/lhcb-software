// $Id: GiGaTrajectoryPoint.cpp,v 1.5 2002-12-07 14:27:52 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// GiGa
#include "GiGa/GiGaTrajectoryPoint.h"
#include "GiGa/GiGaUtil.h"
// G4 
#include "G4Allocator.hh"

// ============================================================================
/** @file 
 *  Implementation file for class : GiGaTrajectoryPoint
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-12-07 
 */
// ============================================================================

namespace GiGaTrajectoryPointLocal
{
  // ==========================================================================
  /** @var s_Allocator
   *  G4 allocator for trajectory points 
   */
  // ==========================================================================
  G4Allocator<GiGaTrajectoryPoint> s_Allocator;
  // ==========================================================================
  /** @var s_Counter
   *  static instatce counter 
   */
  // ==========================================================================
#ifdef GIGA_DEBUG 
  static GiGaUtil::InstanceCounter<GiGaTrajectoryPoint> s_Counter ;
#endif 
  // ==========================================================================
};
// ============================================================================

// ============================================================================
/// the default(empty) constructor 
// ============================================================================
GiGaTrajectoryPoint::GiGaTrajectoryPoint()
  : G4TrajectoryPoint (   )
  , m_time            ( 0 )
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.increment();
#endif 
};
// ============================================================================

// ============================================================================
/** constructor from position and time 
 *  @param Pos position 
 *  @param Time time 
 */
// ============================================================================
GiGaTrajectoryPoint::GiGaTrajectoryPoint 
( const Hep3Vector&  Pos  , 
  const double&      Time )
  : G4TrajectoryPoint ( Pos  )
  , m_time            ( Time )
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.increment();
#endif 
};
// ============================================================================

// ============================================================================
/** constructor from position and time 
 *  @param Time time 
 *  @param Pos position 
 */
// ============================================================================
GiGaTrajectoryPoint::GiGaTrajectoryPoint 
( const double&      Time  , 
  const Hep3Vector&  Pos  )
  : G4TrajectoryPoint ( Pos  )
  , m_time            ( Time )
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.increment();
#endif 
};
// ============================================================================
  
// ============================================================================
/** constructor from 4-vector
 *  @param right 4-vector
 */
// ============================================================================
GiGaTrajectoryPoint::GiGaTrajectoryPoint   
( const HepLorentzVector&     right )
  : G4TrajectoryPoint ( right     )  
  , m_time            ( right.t() ) 
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.increment();
#endif 
};
// ============================================================================

// ============================================================================
/** copy constructor 
 *  @param right object to be copied 
 */
// ============================================================================
GiGaTrajectoryPoint::GiGaTrajectoryPoint   
( const GiGaTrajectoryPoint& right ) 
  : G4TrajectoryPoint ( right        )  
  , m_time            ( right.time() ) 
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.increment();
#endif 
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaTrajectoryPoint::~GiGaTrajectoryPoint()
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.decrement();
#endif 
};
// ============================================================================

// ============================================================================
/// overloaded new 
// ============================================================================
void* GiGaTrajectoryPoint::operator new(size_t)
{ return (void *) GiGaTrajectoryPointLocal::s_Allocator.MallocSingle(); };

// ============================================================================
/// overloaded delete 
// ============================================================================
void GiGaTrajectoryPoint::operator delete(void *point )
{ 
  GiGaTrajectoryPointLocal::s_Allocator.FreeSingle
    ( (GiGaTrajectoryPoint*) point ); 
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

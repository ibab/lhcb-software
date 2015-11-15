// $Id: GiGaTrajectoryPoint.cpp,v 1.6 2004-02-22 19:01:52 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2002/12/07 14:27:52  ibelyaev
//  see $GIGAROOT/cmt/requirements file
//
// ============================================================================
// Include files
// GiGa
#include "GiGa/GiGaTrajectoryPoint.h"
#include "GiGa/GiGaUtil.h"
// G4
#include "G4Allocator.hh"

using CLHEP::HepLorentzVector;
using CLHEP::Hep3Vector;

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
}
// ============================================================================

// ============================================================================
/// the default(empty) constructor
// ============================================================================
GiGaTrajectoryPoint::GiGaTrajectoryPoint()
  : G4TrajectoryPoint (   )
  , m_time            ( 0 )
  , m_momentum        (   )
  , m_process         ( 0 )
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.increment();
#endif
}
// ============================================================================

// ============================================================================
/** constructor from position and time
 *  @param Pos position
 *  @param Time time
 */
// ============================================================================
GiGaTrajectoryPoint::GiGaTrajectoryPoint
( const Hep3Vector&       Pos  ,
  const double            Time ,
  const HepLorentzVector& Mom  ,
  const G4VProcess*       Proc )
  : G4TrajectoryPoint   ( Pos  )
  , m_time              ( Time )
  , m_momentum          ( Mom  )
  , m_process           ( Proc )
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.increment();
#endif
}
// ============================================================================

// ============================================================================
/** constructor from position and time
 *  @param Time time
 *  @param Pos position
 */
// ============================================================================
GiGaTrajectoryPoint::GiGaTrajectoryPoint
( const double            Time ,
  const Hep3Vector&       Pos  ,
  const HepLorentzVector& Mom  ,
  const G4VProcess*       Proc )
  : G4TrajectoryPoint   ( Pos  )
  , m_time              ( Time )
  , m_momentum          ( Mom  )
  , m_process           ( Proc )
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.increment();
#endif
}
// ============================================================================

// ============================================================================
/** constructor from 4-vector
 *  @param right 4-vector
 */
// ============================================================================
GiGaTrajectoryPoint::GiGaTrajectoryPoint
( const HepLorentzVector& right     ,
  const HepLorentzVector& Mom       ,
  const G4VProcess*       Proc      )
  : G4TrajectoryPoint   ( right     )
  , m_time              ( right.t() )
  , m_momentum          ( Mom       )
  , m_process           ( Proc      )
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.increment();
#endif
}
// ============================================================================

// ============================================================================
/** constructor from G4TrajectoryPoint
 *  @param point trajectory point
 *  @param Time time
 *  @param Mom   input momentum
 *  @param Proc  process
 */
// ============================================================================
GiGaTrajectoryPoint::GiGaTrajectoryPoint
( const G4TrajectoryPoint& Point ,
  const double             Time  ,
  const HepLorentzVector&  Mom   ,
  const G4VProcess*        Proc  )
  : G4TrajectoryPoint    ( Point )
  , m_time               ( Time  )
  , m_momentum           ( Mom   )
  , m_process            ( Proc  )
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.increment();
#endif
}
// ============================================================================

// ============================================================================
/** constructor from G4VTrajectoryPoint
 *  @param point trajectory point
 *  @param Time time
 *  @param Mom   input momentum
 *  @param Proc  process
 */
// ============================================================================
GiGaTrajectoryPoint::GiGaTrajectoryPoint
( const G4VTrajectoryPoint& Point ,
  const double              Time  ,
  const HepLorentzVector&   Mom   ,
  const G4VProcess*         Proc  )
  : G4TrajectoryPoint     ( Point.GetPosition() )
  , m_time                ( Time  )
  , m_momentum            ( Mom   )
  , m_process             ( Proc  )
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.increment();
#endif
}
// ============================================================================


// ============================================================================
/** copy constructor
 *  @param right object to be copied
 */
// ============================================================================
GiGaTrajectoryPoint::GiGaTrajectoryPoint
( const GiGaTrajectoryPoint& right )
  : G4TrajectoryPoint ( right             )
  , m_time            ( right.time     () )
  , m_momentum        ( right.momentum () )
  , m_process         ( right.process  () )
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.increment();
#endif
}
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
GiGaTrajectoryPoint::~GiGaTrajectoryPoint()
{
#ifdef GIGA_DEBUG
  GiGaTrajectoryPointLocal::s_Counter.decrement();
#endif
}
// ============================================================================

// ============================================================================
/** clone method (virtual constructor)
 *  @return new trajectory point
 */
// ============================================================================
GiGaTrajectoryPoint* GiGaTrajectoryPoint::clone      () const
{
  return new GiGaTrajectoryPoint( *this );
}
// ============================================================================

// ============================================================================
/// overloaded new
// ============================================================================
void* GiGaTrajectoryPoint::operator new(size_t)
{
  return (void *) GiGaTrajectoryPointLocal::s_Allocator.MallocSingle();
}

// ============================================================================
/// overloaded delete
// ============================================================================
void GiGaTrajectoryPoint::operator delete(void *point )
{
  GiGaTrajectoryPointLocal::s_Allocator.FreeSingle
    ( (GiGaTrajectoryPoint*) point );
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================

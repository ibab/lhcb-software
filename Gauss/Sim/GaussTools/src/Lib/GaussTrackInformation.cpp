// $Id: GaussTrackInformation.cpp,v 1.1 2002-12-07 21:19:14 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// GiGa 
#include "GiGa/GiGaUtil.h"
// local
#include "GaussTools/GaussTrackInformation.h"
// G4 
#include "G4Allocator.hh"

// ============================================================================
/** @file 
 *  Implementation file for class : GaussTrackInformation
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    23/02/2001
 */
// ============================================================================

// ============================================================================
namespace GaussTrackInformationLocal
{
  // ==========================================================================
  /** @var s_Allocator 
   *  G4 allocator for fast creation of many "small" objects 
   */
  // ==========================================================================
  G4Allocator<GaussTrackInformation> s_Allocator;
  // ==========================================================================
  /** @var s_Counter 
   *  static instance counter 
   */
  // ==========================================================================
#ifdef GIGA_DEBUG
  static GiGaUtil::InstanceCounter<GaussTrackInformation> s_Counter ;
#endif
  // ==========================================================================
};
// ============================================================================

// ============================================================================
/// default (empty) constructor 
// ============================================================================
GaussTrackInformation::GaussTrackInformation()
  : m_appendStep ( false )
  , m_toBeStored ( false )
  , m_createdHit ( false )
  , m_hits       ()  
{
#ifdef GIGA_DEBUG
  GaussTrackInformationLocal::s_Counter.increment() ;
#endif
};
// ============================================================================


// ============================================================================
/// copy constructor 
// ============================================================================
GaussTrackInformation:: GaussTrackInformation 
( const GaussTrackInformation& right              )
  : G4VUserTrackInformation (  right              ) 
  , m_appendStep            (  right.m_appendStep )
  , m_toBeStored            (  right.m_toBeStored )
  , m_createdHit            (  right.m_createdHit )
  , m_hits                  (  right.m_hits       )  
{
#ifdef GIGA_DEBUG
  GaussTrackInformationLocal::s_Counter.increment() ;
#endif
};
// ============================================================================

// ============================================================================
/// clone (virtual constructor)
// ============================================================================
GaussTrackInformation* GaussTrackInformation::clone() const 
{ return new GaussTrackInformation( *this ); }
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GaussTrackInformation::~GaussTrackInformation() 
{
#ifdef GIGA_DEBUG
  GaussTrackInformationLocal::s_Counter.decrement() ;
#endif
};
// ============================================================================

// ============================================================================
/// operator new 
// ============================================================================
void* GaussTrackInformation::operator new    ( size_t )
{ return (void*) GaussTrackInformationLocal::s_Allocator.MallocSingle(); };
// ============================================================================

// ============================================================================
/// operator delete 
// ============================================================================
void GaussTrackInformation::operator delete ( void*  info  )
{ 
  GaussTrackInformationLocal::s_Allocator.FreeSingle
    ( (GaussTrackInformation*) info ); 
};
//=============================================================================

// ============================================================================
// The END 
// ============================================================================

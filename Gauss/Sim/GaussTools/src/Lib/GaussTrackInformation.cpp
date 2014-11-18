// $Id: GaussTrackInformation.cpp,v 1.5 2006-04-12 19:32:08 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2005/10/31 09:29:44  gcorti
// use HepMC for history during tracking
//
// Revision 1.3  2003/07/28 10:26:23  witoldp
// added WorldCuts and DetTrackInfo
//
// Revision 1.2  2003/04/09 12:07:37  witoldp
// added pointer to RICHInfo
//
// Revision 1.1  2002/12/07 21:19:14  ibelyaev
//  few optimization updates
// 
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
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
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
}


// ============================================================================
/// default (empty) constructor 
// ============================================================================
GaussTrackInformation::GaussTrackInformation()
  : m_appendStep(false)
  , m_toBeStored(false)
  , m_createdHit(false)
  , m_directParent(true)
  , m_storeHepMC(false)
  , m_hits()
  , m_detInfo(0)
{
#ifdef GIGA_DEBUG
  GaussTrackInformationLocal::s_Counter.increment() ;
#endif
}



// ============================================================================
/// copy constructor 
// ============================================================================
GaussTrackInformation:: GaussTrackInformation 
( const GaussTrackInformation& right)
  : G4VUserTrackInformation(right) 
  , m_appendStep(right.m_appendStep)
  , m_toBeStored(right.m_toBeStored)
  , m_createdHit(right.m_createdHit)
  , m_directParent(right.m_directParent)
  , m_storeHepMC(right.m_storeHepMC)
  , m_hits(right.m_hits)
  , m_detInfo(right.m_detInfo) 
{
#ifdef GIGA_DEBUG
  GaussTrackInformationLocal::s_Counter.increment() ;
#endif
}


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
  delete m_detInfo;

#ifdef GIGA_DEBUG
  GaussTrackInformationLocal::s_Counter.decrement() ;
#endif
}


// ============================================================================
/// operator new 
// ============================================================================
void* GaussTrackInformation::operator new    ( size_t )
{ return (void*) GaussTrackInformationLocal::s_Allocator.MallocSingle(); }
// ============================================================================

// ============================================================================
/// operator delete 
// ============================================================================
void GaussTrackInformation::operator delete ( void*  info  )
{ 
  GaussTrackInformationLocal::s_Allocator.FreeSingle
    ( (GaussTrackInformation*) info ); 
}


// ============================================================================
// The END 
// ============================================================================

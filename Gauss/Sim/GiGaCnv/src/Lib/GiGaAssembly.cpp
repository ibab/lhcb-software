// $Id: GiGaAssembly.cpp,v 1.2 2002-12-07 14:36:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/01/22 18:24:42  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
// 
// ============================================================================
// Include files
// GiGa
#include "GiGa/GiGaUtil.h"
// local
#include "GiGaCnv/GiGaAssembly.h"
#include "GiGaCnv/GiGaAssemblyStore.h"

// ============================================================================
/** @file GiGaAssembly.cpp
 * 
 * Implementation file for class : GiGaAssembly
 *
 * @date 17/01/2002 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

namespace GiGaAssemblyLocal
{
#ifdef GIGA_DEBUG
  /** @var s_Counter 
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaAssembly> s_Counter;
#endif 
}


// ============================================================================
/** standard constructor
 */
// ============================================================================
GiGaAssembly::GiGaAssembly( const std::string& Name ) 
  : m_name    ( Name ) 
  , m_volumes (      ) 
{
  GiGaAssemblyStore::store()->addAssembly( this );
#ifdef GIGA_DEBUG
  GiGaAssemblyLocal::s_Counter.increment();
#endif
}

// ============================================================================
/** copy constructor 
 *  @param right object to be copied 
 */
// ============================================================================
GiGaAssembly::GiGaAssembly( const GiGaAssembly& right ) 
  : m_name    ( right.m_name    )  
  , m_volumes ( right.m_volumes ) 
{
  GiGaAssemblyStore::store()->addAssembly( this );
#ifdef GIGA_DEBUG
  GiGaAssemblyLocal::s_Counter.increment();
#endif
}
// ============================================================================

// ============================================================================
/** destructor
 */
// ============================================================================
GiGaAssembly::~GiGaAssembly() 
{ 
  m_volumes.clear() ; 
  GiGaAssemblyStore::store()->removeAssembly( this );
#ifdef GIGA_DEBUG
  GiGaAssemblyLocal::s_Counter.decrement () ;
#endif
}

// ============================================================================
/** add "volume" to the assembly. 
 *  @param value the pair to be added 
 *  @param Name  pair name to be appended 
 *  @return status code 
 */
// ============================================================================
StatusCode  GiGaAssembly::addVolume( const GiGaVolumePair&  value ,
                                     const std::string&     Name  ) 
{
  ///
  const GiGaVolume& volume = value.volume() ;
  if( !volume.valid()             ) { return StatusCode::FAILURE ; } // RETURN
  /// logical volume?
  if( 0 != volume.volume()        )
    { 
      m_volumes.push_back( Volume( NickV( volume .volume() , Name ) , 
                                   value.matrix() ) ) ; 
      return StatusCode::SUCCESS ;                                   // RETURN
    }
  /// assembly? 
  const GiGaAssembly* assembly = volume.assembly() ;
  if( 0 == assembly               ) { return StatusCode::FAILURE ; } // RETURN 
  /// 
  for( Volumes::const_iterator vol = assembly->volumes().begin() ;
       assembly->volumes().end() != vol ; ++vol ) 
    {
      m_volumes.push_back(  Volume( NickV( vol->first.first , 
                                           Name + "#" + vol->first.second ) , 
                                    vol->second * value.matrix () ) ) ;
    }
  ///
  return StatusCode::SUCCESS ;
}

// ============================================================================
// End 
// ============================================================================

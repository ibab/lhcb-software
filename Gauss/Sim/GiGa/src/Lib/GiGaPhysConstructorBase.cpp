// $Id: GiGaPhysConstructorBase.cpp,v 1.5 2004-04-20 04:26:06 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2003/04/06 18:49:47  ibelyaev
//  see $GIGAROOT/doc/release.notes
// 
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaPhysConstructorBase.h"
#include "GiGa/GiGaUtil.h"

// ============================================================================
/** @file 
 *  implementation of class GiGaPhysConstructorBase
 *
 *  @author Witek Pokorski   Witold.Pokorski@cern.ch 
 */
// ============================================================================

namespace GiGaPhysConstructorBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaPhysConstructorBase> s_Counter ;
#endif   
};

// ============================================================================
/** standard constructor
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaPhysConstructorBase::GiGaPhysConstructorBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaPhysicsConstructorBase         ( type , name , parent  )
  , m_self                             ( 0 )
{ 
  declareInterface<IGiGaPhysConstructor> (this);
#ifdef GIGA_DEBUG
  GiGaPhysConstructorBaseLocal::s_Counter.increment () ;
#endif
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
GiGaPhysConstructorBase::~GiGaPhysConstructorBase()
{
#ifdef GIGA_DEBUG
  GiGaPhysConstructorBaseLocal::s_Counter.decrement () ;
#endif 
};
// ============================================================================

// ============================================================================
/** initialize the object 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaPhysConstructorBase::initialize ()  
{
  // initialize base class
  StatusCode sc = GiGaPhysicsConstructorBase::initialize (); 
  if( sc.isFailure() )
    { return Error("Could not initialize GiGaPhysicsConstructorBase!", sc); }
  MsgStream log( msgSvc() , name() ); 
  
  // initialise the self pointer 
  m_self = this ;
  //
  return Print("GiGaPhysConstructorBase initialized succesfully" ,
               StatusCode::SUCCESS                        , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
/** finalize initialize the object 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaPhysConstructorBase::finalize   ()  
{ 
  Print("GiGaPhysConstructorBase Finalization" ,
        StatusCode::SUCCESS             , MSG::VERBOSE );
  // finalze the base class 
  return GiGaPhysicsConstructorBase::finalize   (); 
};
// ============================================================================

// ============================================================================
/** accessor to G4VPhysicsConstructor
 *  @see IGiGaPhysConstructor
 *  @see IGiGaPhysicsConstructor
 *  @see G4VPhysicsConstructor
 *  @return pointer to G4VPhysicsConstructor object
 */
// ============================================================================
G4VPhysicsConstructor* GiGaPhysConstructorBase::physicsConstructor ()  const
{
  if( 0 == m_self ) 
    {
      const G4VPhysicsConstructor* aux = this ;
      m_self = const_cast<G4VPhysicsConstructor*> ( aux ) ;
    }
  if( 0 == m_self ) 
    { Error("physicsConstructor(): invalid G4VPhysicsConstructor!") ; }
  return m_self ;
};
// ============================================================================

unsigned long GiGaPhysConstructorBase::release()
{
  unsigned long count = addRef() ;
  if ( count >= 2 )
  {
    AlgTool::release() ;
    return AlgTool::release() ;
  }
  return 0 ;
};

// ============================================================================
// The END 
// ============================================================================

// $Id: GiGaPhysListBase.cpp,v 1.9 2002-12-07 14:27:51 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2002/05/07 12:21:33  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaPhysListBase.h"
#include "GiGa/GiGaUtil.h"

// ============================================================================
/** @file 
 *  implementation of class GiGaPhysListBase
 *
 *  @author Vanya Belyaev 
 */
// ============================================================================

namespace GiGaPhysListBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaPhysListBase> s_Counter ;
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
GiGaPhysListBase::GiGaPhysListBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaBase          ( type , name , parent  )
  , m_DefaultCutValue ( 2.0 * mm    )
{ 
  declareInterface<IGiGaPhysList> (this);
  declareProperty( "Cut" , m_DefaultCutValue ); 
#ifdef GIGA_DEBUG
  GiGaPhysListBaseLocal::s_Counter.increment () ;
#endif
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
GiGaPhysListBase::~GiGaPhysListBase()
{
#ifdef GIGA_DEBUG
  GiGaPhysListBaseLocal::s_Counter.decrement () ;
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
StatusCode GiGaPhysListBase::initialize ()  
{
  /// initialize base class
  StatusCode sc = GiGaBase::initialize (); 
  if( sc.isFailure() )
    { return Error("Could not initialize GiGaBase class!", sc); }
  MsgStream log( msgSvc() , name() ); 
  if( defCut() != GetDefaultCutValue() )
    {
      log << MSG::DEBUG                
          << " Value of Default Cut is changed from " 
          << GetDefaultCutValue() / mm << " [mm] to " 
          << defCut()             / mm << " [mm] " << endreq; 
      SetDefaultCutValue( defCut() );        
    } 
  log << MSG::INFO << " Default Cut Value is set to be " 
      << GetDefaultCutValue() / mm << " [mm]" << endreq; 
  ///
  return Print("GiGaPhysListBase initialized succesfully" ,
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
StatusCode GiGaPhysListBase::finalize   ()  
{ 
  Print("GiGaPhysListBase Finalization" ,
        StatusCode::SUCCESS             , MSG::VERBOSE );
  // finalze the base class 
  return GiGaBase::finalize   (); 
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

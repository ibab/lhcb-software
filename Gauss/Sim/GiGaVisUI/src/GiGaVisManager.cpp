// $Id: GiGaVisManager.cpp,v 1.6 2008-06-26 11:40:34 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "GiGaVisManager.h"

#include "G4VisManager.hh"
///
// Not needing external packages or libraries...
#include "G4ASCIITree.hh"
#include "G4DAWNFILE.hh"
#include "G4RayTracer.hh"
#include "G4VRML1File.hh"
#include "G4VRML2File.hh"
//
#ifdef G4VIS_USE_DAWN
#include "G4FukuiRenderer.hh"
#endif
//
#ifdef G4VIS_USE_OPACS
#include "G4Wo.hh"
#include "G4Xo.hh"
#endif
//
#ifdef G4VIS_USE_OPENGLX
#include "G4OpenGLImmediateX.hh"
#include "G4OpenGLStoredX.hh"
#endif
//
#ifdef G4VIS_USE_OPENGLWIN32
#include "G4OpenGLImmediateWin32.hh"
#include "G4OpenGLStoredWin32.hh"
#endif
//
#ifdef G4VIS_USE_OPENGLXM
#include "G4OpenGLImmediateXm.hh"
#include "G4OpenGLStoredXm.hh"
#endif
//
#ifdef G4VIS_USE_OIX
#include "G4OpenInventorX.hh"
#endif
//
#ifdef G4VIS_USE_OIWIN32
#include "G4OpenInventorWin32.hh"
#endif
//
#ifdef G4VIS_USE_VRML
#include "G4VRML1.hh"
#include "G4VRML2.hh"
#endif
//
namespace GiGaVisManagerLocal 
{
  class GiGaVisMgr: public G4VisManager
  {
  public:
    /// constructor 
    GiGaVisMgr() 
      : G4VisManager() 
    {
      // activate the static pointer 
      G4VVisManager::SetConcreteInstance( this ); 
    };
    
    /// destructor
    virtual ~GiGaVisMgr() {}
    // main method 
    void RegisterGraphicsSystems ()
    {
      // Graphics Systems not needing external packages or libraries...
      RegisterGraphicsSystem (new G4ASCIITree);
      RegisterGraphicsSystem (new G4DAWNFILE);
      RegisterGraphicsSystem (new G4RayTracer);
      RegisterGraphicsSystem (new G4VRML1File);
      RegisterGraphicsSystem (new G4VRML2File);
      
      // Graphics systems needing external packages or libraries...
      
#ifdef G4VIS_USE_DAWN
      RegisterGraphicsSystem (new G4FukuiRenderer);
#endif
      
#ifdef G4VIS_USE_OPACS
      RegisterGraphicsSystem (new G4Wo);
      RegisterGraphicsSystem (new G4Xo);
#endif
      
#ifdef G4VIS_USE_OPENGLX
      RegisterGraphicsSystem (new G4OpenGLImmediateX);
      RegisterGraphicsSystem (new G4OpenGLStoredX);
#endif
      
#ifdef G4VIS_USE_OPENGLWIN32
      RegisterGraphicsSystem (new G4OpenGLImmediateWin32);
      RegisterGraphicsSystem (new G4OpenGLStoredWin32);
#endif
      
#ifdef G4VIS_USE_OPENGLXM
      RegisterGraphicsSystem (new G4OpenGLImmediateXm);
      RegisterGraphicsSystem (new G4OpenGLStoredXm);
#endif
      
#ifdef G4VIS_USE_OIX
      RegisterGraphicsSystem (new G4OpenInventorX);
#endif
      
#ifdef G4VIS_USE_OIWIN32
      RegisterGraphicsSystem (new G4OpenInventorWin32);
#endif
      
#ifdef G4VIS_USE_VRML
      RegisterGraphicsSystem (new G4VRML1);
      RegisterGraphicsSystem (new G4VRML2);
#endif
      
      if (fVerbose > 0) 
        {
          G4cout << "\nYou have successfully chosen to use " 
                 << "the following graphics systems."
                 << G4endl;
          PrintAvailableGraphicsSystems ();
        }
      ///
    };
  };
}



// ============================================================================
/** @file 
 *  Implementation file for class : GiGaVisManager
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-04
 */
// ============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaVisManager )


// ============================================================================
/** standard constructor 
 *  @param type type 
 *  @param name instance name 
 *  @param parent pointer to parent 
 */
// ============================================================================
GiGaVisManager::GiGaVisManager
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : GiGaBase( type, name , parent )
  , m_visMgr ( 0 )
{ 
  declareInterface <IGiGaVisManager> ( this ); 
}


// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
GiGaVisManager::~GiGaVisManager()
{ 
  if( 0 != m_visMgr ) { 
    delete m_visMgr ; 
    m_visMgr = 0 ; 
  } 
}


// ============================================================================
/** get the pointer to G4 visual manager
 *  @see IGiGaVisManager
 *  @return pointer to G4 visual manager 
 */
// ============================================================================
G4VVisManager* GiGaVisManager::visMgr () const 
{
  if( 0 == m_visMgr ) { Warning("G4VVisManager* points to NULL!"); }
  return m_visMgr ;
}


// ============================================================================
/** initialization method 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaVisManager::initialize  ()        
{
  // initialize the base 
  StatusCode sc = GiGaBase::initialize();
  if( sc.isFailure() ) { return Error("Error from GiGaBase",sc); }
  
  G4VisManager* vm = new GiGaVisManagerLocal::GiGaVisMgr();
  vm->Initialize();
  m_visMgr = vm ;
  
  if( 0 == m_visMgr )
    { Warning ( "initialize(): local  G4VVisManager* points to NULL!" ) ; }
  if( 0 == G4VVisManager::GetConcreteInstance() ) 
    { Warning ( "initialize(): static G4VVisManager* points to NULL!" ) ; }
  
  return StatusCode::SUCCESS ;
}


// ============================================================================
/** finalization method 
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaVisManager::finalize    ()
{
  // delete the visualization manager itself 
  Print("finalize(): Delete the virualization manager");
  if( 0 != m_visMgr ) { delete m_visMgr ; m_visMgr = 0 ; }
  // finalize the base class 
  return GiGaBase::finalize ();
}
  


// ============================================================================
// The END 
// ===========================================================================

// $Id: GiGaVisManager.cpp,v 1.1 2002-12-04 21:17:38 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
// from GiGa 
#include "GiGa/GiGaMACROs.h"
// local
#include "GiGaVisManager.h"

#ifdef G4VIS_NONE
#else
#include "G4VisManager.hh"
///
// Not needing external packages or libraries...
#include "G4ASCIITree.hh"
#include "G4DAWNFILE.hh"
#include "G4GAGTree.hh"
#include "G4HepRepFile.hh"
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
    GiGaVisMgr() {}
    /// destructor
    virtual ~GiGaVisMgr() {}
    // main method 
    void RegisterGraphicsSystems ()
    {
      // Graphics Systems not needing external packages or libraries...
      RegisterGraphicsSystem (new G4ASCIITree);
      RegisterGraphicsSystem (new G4GAGTree);
      RegisterGraphicsSystem (new G4DAWNFILE);
      RegisterGraphicsSystem (new G4HepRepFile);
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
};  

#endif  // G4VIS_NONE



// ============================================================================
/** @file 
 *  Implementation file for class : GiGaVisManager
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-04
 */
// ============================================================================

// ============================================================================
/// Declaration of the Tool Factory
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaVisManager ) ;
// ============================================================================


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
{ declareInterface <IGiGaVisManager> ( this ); };
// ============================================================================

// ============================================================================
/// destructor (virtual and protected)
// ============================================================================
GiGaVisManager::~GiGaVisManager()
{ if( 0 != m_visMgr ) { delete m_visMgr ; m_visMgr = 0 ; } };
// ============================================================================

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
};
// ============================================================================

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
  
#ifdef G4VIS_NONE
  Warning("GiGaVisManager was compiled with the flag G4VIS_NONE");
#else 
  G4VisManager* vm = new GiGaVisManagerLocal::GiGaVisMgr();
  vm->Initialize();
  m_visMgr = vm ;
#endif 
  
  if( 0 == m_visMgr ) { Warning("G4VVisManager* points to NULL!"); }
  return StatusCode::SUCCESS ;
};
// ============================================================================

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
  Print("Delete the virualization manager");
  if( 0 != m_visMgr ) { delete m_visMgr ; m_visMgr = 0 ; }
  // finalize the base class 
  return GiGaBase::finalize ();
};
// ============================================================================
  


// ============================================================================
// The END 
// ============================================================================



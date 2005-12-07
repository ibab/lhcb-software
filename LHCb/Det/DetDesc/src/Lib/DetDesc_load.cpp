// $Id: DetDesc_load.cpp,v 1.1 2005-12-07 17:30:33 cattanem Exp $
// Include files
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
// DetDesc
#include "DetDesc/ISolidFactory.h"
// Objects from DetDesc package to be implemented:
#include "DetDesc/LVolume.h"
#include "DetDesc/LAssembly.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/Isotope.h"
#include "DetDesc/Element.h"
#include "DetDesc/Mixture.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/Surface.h"

// ============================================================================
/** @file 
 *  Implementation of Data Object Factories and declaration of ALL 
 *  Factories from DetDesc package 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date xx/xx/xxxx 
 */
// ============================================================================

// ============================================================================
/** @def IMPLEMENT_OBJECT(x) 
 *  Implement Data Object Factory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 06 May 2002 
 */
#define IMPLEMENT_OBJECT(x) \
 static const DataObjectFactory<x>         s_##x##Factory ;\
 const              IFactory& x##Factory = s_##x##Factory ;
// ============================================================================

// ============================================================================
IMPLEMENT_OBJECT( DetectorElement   ) ;
IMPLEMENT_OBJECT( Isotope           ) ;
IMPLEMENT_OBJECT( Mixture           ) ;
IMPLEMENT_OBJECT( Element           ) ;
IMPLEMENT_OBJECT( LVolume           ) ;
IMPLEMENT_OBJECT( LAssembly         ) ;
IMPLEMENT_OBJECT( Surface           ) ;
IMPLEMENT_OBJECT( TabulatedProperty ) ;
// ============================================================================


DECLARE_FACTORY_ENTRIES(DetDesc) {
  
  // data objects 
  DECLARE_OBJECT(    LVolume                 ) ;
  DECLARE_OBJECT(    LAssembly               ) ;
  DECLARE_OBJECT(    DetectorElement         ) ;
  DECLARE_OBJECT(    Isotope                 ) ;
  DECLARE_OBJECT(    Element                 ) ;
  DECLARE_OBJECT(    Mixture                 ) ;
  DECLARE_OBJECT(    TabulatedProperty       ) ;
  DECLARE_OBJECT(    Surface                 ) ;
  
  // solids 
  DECLARE_SOLID(     SolidBox                ) ;
  DECLARE_SOLID(     SolidTubs               ) ;
  DECLARE_SOLID(     SolidCons               ) ;
  DECLARE_SOLID(     SolidSphere             ) ;
  DECLARE_SOLID(     SolidTrd                ) ;
  DECLARE_SOLID(     SolidTrap               ) ;
  DECLARE_SOLID(     SolidPolycone           ) ;
  DECLARE_SOLID(     SolidChild              ) ;
  DECLARE_SOLID(     SolidIntersection       ) ;
  DECLARE_SOLID(     SolidSubtraction        ) ;
  DECLARE_SOLID(     SolidUnion              ) ;
  DECLARE_SOLID(     SolidPolycone           ) ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

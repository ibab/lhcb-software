// $Id: DetDesc_load.cpp,v 1.11 2002-05-26 10:47:14 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2002/05/04 13:13:25  ibelyaev
//  see $DETDESCROOT/doc/release.notes ( 4 May 2002 )
//
// ============================================================================
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
 static const DataObjectFactory<##x##>         s_##x##Factory ;\
 const                 IFactory&##x##Factory = s_##x##Factory ;
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
  
  // services  
  DECLARE_SERVICE(   XmlCnvSvc               ) ;
  DECLARE_SERVICE(   TransportSvc            ) ;
  DECLARE_SERVICE(   XmlParserSvc            ) ;
  
  // converters  
  DECLARE_CONVERTER( XmlCatalogCnv           ) ;
  DECLARE_CONVERTER( XmlConditionCnv         ) ;
  DECLARE_CONVERTER( XmlDetectorElementCnv   ) ;
  DECLARE_CONVERTER( XmlLVolumeCnv           ) ;
  DECLARE_CONVERTER( XmlIsotopeCnv           ) ;
  DECLARE_CONVERTER( XmlElementCnv           ) ;
  DECLARE_CONVERTER( XmlMixtureCnv           ) ;
  DECLARE_CONVERTER( XmlTabulatedPropertyCnv ) ;
  DECLARE_CONVERTER( XmlSurfaceCnv           ) ;
  
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

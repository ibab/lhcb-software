/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
///@{
/** GaudiKernel includes */
#include "GaudiKernel/ICnvFactory.h"
#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/IAlgFactory.h"
#include "GaudiKernel/IObjectFactory.h"
#include "GaudiKernel/ObjectFactory.h"
///@}
/// DetDesc 
#include "DetDesc/ISolidFactory.h"


/// ===========================================================================
/** @file DetDesc_load.cpp 
 * 
 *  Implementation of <Package>_load routine. This routine 
 *  is needed for forcing the linker to load all the components
 *  of the library.. 
 */
/// ===========================================================================


#define DLL_DECL_SERVICE(x)       extern const ISvcFactory& x##Factory; \
                                  x##Factory.addRef();
#define DLL_DECL_CONVERTER(x)     extern const ICnvFactory& x##Factory; \
                                  x##Factory.addRef();
#define DLL_DECL_ALGORITHM(x)     extern const IAlgFactory& x##Factory; \
                                  x##Factory.addRef();
#define DLL_DECL_SOLID(x)         extern const ISolidFactory& x##Factory; \
                                  x##Factory.addRef();

#include "DetDesc/LVolume.h"
_ImplementDataObjectFactory( LVolume           ) ;  

#include "DetDesc/DetectorElement.h"
_ImplementDataObjectFactory( DetectorElement   ) ; 

#include "DetDesc/Isotope.h"
_ImplementDataObjectFactory( Isotope           ) ;  

#include "DetDesc/Element.h"
_ImplementDataObjectFactory( Element           ) ;  

#include "DetDesc/Mixture.h"
_ImplementDataObjectFactory( Mixture           ) ;  

#include "DetDesc/TabulatedProperty.h"
_ImplementDataObjectFactory( TabulatedProperty ) ; 

#include "DetDesc/Surface.h"
_ImplementDataObjectFactory( Surface           ) ; 


void DetDesc_load() 
{
  
  ///@{
  /** Declare services */ 
  DLL_DECL_SERVICE( XmlCnvSvc    );
  DLL_DECL_SERVICE( TransportSvc );
  DLL_DECL_SERVICE( XmlParserSvc );
  ///@}
  
  ///@{
  /** converters */
  DLL_DECL_CONVERTER( XmlCatalogCnv           );
  DLL_DECL_CONVERTER( XmlDetectorElementCnv   );
  DLL_DECL_CONVERTER( XmlLVolumeCnv           );
  DLL_DECL_CONVERTER( XmlIsotopeCnv           );
  DLL_DECL_CONVERTER( XmlElementCnv           );
  DLL_DECL_CONVERTER( XmlMixtureCnv           );
  DLL_DECL_CONVERTER( XmlTabulatedPropertyCnv );
  DLL_DECL_CONVERTER( XmlSurfaceCnv           );
  ///@}

  ///@{
  /** DataObject Factories */
  DLL_DECL_OBJECTFACTORY( LVolume           );
  DLL_DECL_OBJECTFACTORY( DetectorElement   );
  DLL_DECL_OBJECTFACTORY( Isotope           );
  DLL_DECL_OBJECTFACTORY( Element           );
  DLL_DECL_OBJECTFACTORY( Mixture           );
  DLL_DECL_OBJECTFACTORY( TabulatedProperty );
  DLL_DECL_OBJECTFACTORY( Surface           );
  ///@}  

  ///@{
  /** Factories for instantiation of concrete solids */
  DLL_DECL_SOLID( SolidBox           );
  DLL_DECL_SOLID( SolidTubs          );
  DLL_DECL_SOLID( SolidCons          );
  DLL_DECL_SOLID( SolidSphere        );
  DLL_DECL_SOLID( SolidTrd           );
  DLL_DECL_SOLID( SolidTrap          );
  DLL_DECL_SOLID( SolidChild         );
  DLL_DECL_SOLID( SolidIntersection  );
  DLL_DECL_SOLID( SolidSubtraction   );
  DLL_DECL_SOLID( SolidUnion         );
  ///@}
  
};


extern "C" void DetDesc_loadRef() { DetDesc_load(); }








// $Id: CaloDetXmlCnv_load.cpp,v 1.1.1.1 2002-03-28 13:32:34 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#include "GaudiKernel/ICnvFactory.h"
#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/IAlgFactory.h"

// ============================================================================
/** @file   CaloDetXmlCnv_load.cpp
 *
 *  Implementation of "Package"_load routine. This routine 
 *  is needed for forcing the linker to load all the components
 *  of the library.. 
 */
// ============================================================================


#define DLL_DECL_CONVERTER(x)  extern const ICnvFactory& x##Factory;\
                               x##Factory.addRef();

void CaloDetXmlCnv_load() 
{  
  /// Converter factories 
  DLL_DECL_CONVERTER( XmlDeCalorimeterCnv    );
  DLL_DECL_CONVERTER( XmlDeSubCalorimeterCnv );
  
};

extern "C" void CaloDetXmlCnv_loadRef() {  CaloDetXmlCnv_load(); }

// ============================================================================
// The End 
// ============================================================================


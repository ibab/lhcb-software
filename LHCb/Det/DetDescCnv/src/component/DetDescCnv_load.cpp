// $Id: DetDescCnv_load.cpp,v 1.1.1.1 2003-04-23 13:59:46 sponce Exp $
// Include files
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

/**
 *  Declaration of ALL Factories from DetDescCnv package 
 *  @author Sebastien Ponce
 */
DECLARE_FACTORY_ENTRIES(DetDescCnv) {
  
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
  
};


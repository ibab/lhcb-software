// $Id: XmlTools_load.cpp,v 1.1.1.1 2003-04-23 13:38:46 sponce Exp $
// Include files
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

/**
 *  Declaration of ALL Factories from XmlTools package 
 *  @author Sebastien Ponce
 */
DECLARE_FACTORY_ENTRIES(XmlTools) {
  
  // services  
  DECLARE_SERVICE(   XmlCnvSvc               ) ;
  DECLARE_SERVICE(   XmlParserSvc            ) ;
  
};


//$Id: DetCond_load.cpp,v 1.3 2004-12-08 17:19:17 marcocle Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_FACTORY_ENTRIES( DetCond ) {
  DECLARE_SERVICE  ( ConditionsDBCnvSvc );
  // converters  
  DECLARE_CONVERTER( XmlRelyCnv           ) ;
 
}

//$Id: DetCond_load.cpp,v 1.4 2005-02-09 08:30:54 marcocle Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_FACTORY_ENTRIES( DetCond ) {
  // services ========================
  // --------- Database access interface
  DECLARE_SERVICE  ( CondDBAccessSvc );
  // --------- Conversion Service
  DECLARE_SERVICE  ( ConditionsDBCnvSvc );
  // converters ======================
  // --------- Fall-back converter
  DECLARE_CONVERTER( XmlRelyCnv) ;
}

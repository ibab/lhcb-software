//$Id: DetCond_load.cpp,v 1.5 2005-02-09 08:49:29 marcocle Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( DetCond ) {
  // services ========================
  // --------- Database access interface
  DECLARE_SERVICE  ( CondDBAccessSvc );
  // --------- Conversion Service
  DECLARE_SERVICE  ( ConditionsDBCnvSvc );
  // converters ======================
  // --------- Fall-back converter
  DECLARE_CONVERTER( RelyConverter ) ;
};


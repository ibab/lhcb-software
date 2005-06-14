//$Id: DetCond_load.cpp,v 1.6 2005-06-14 13:14:30 marcocle Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( DetCond ) {
  // services ========================
  // --------- Database access interface
  DECLARE_SERVICE  ( CondDBAccessSvc );
  // --------- Conversion Service
  DECLARE_SERVICE  ( CondDBCnvSvc );
  // converters ======================
  // --------- Fall-back converter
  DECLARE_CONVERTER( RelyConverter ) ;
};


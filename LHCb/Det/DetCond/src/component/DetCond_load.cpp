//$Id: DetCond_load.cpp,v 1.9 2006-07-11 18:25:17 marcocle Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( DetCond ) {
  // services ========================
  // --------- Database access service
  DECLARE_SERVICE  ( CondDBAccessSvc );
  // --------- Database dispatcher service
  DECLARE_SERVICE  ( CondDBDispatcherSvc );
  // --------- Conversion Service
  DECLARE_SERVICE  ( CondDBCnvSvc );
  // converters ======================
  // --------- Fall-back converter
  DECLARE_CONVERTER( RelyConverter ) ;
};


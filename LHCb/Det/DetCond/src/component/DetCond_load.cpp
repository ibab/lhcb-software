//$Id: DetCond_load.cpp,v 1.10 2006-07-14 09:27:33 marcocle Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES( DetCond ) {
  // services ========================
  // --------- Database access service
  DECLARE_SERVICE  ( CondDBAccessSvc );
  // --------- Database dispatcher service
  DECLARE_SERVICE  ( CondDBDispatcherSvc );
  // --------- Database layering service
  DECLARE_SERVICE  ( CondDBLayeringSvc );
  // --------- Conversion Service
  DECLARE_SERVICE  ( CondDBCnvSvc );
  // converters ======================
  // --------- Fall-back converter
  DECLARE_CONVERTER( RelyConverter ) ;
};


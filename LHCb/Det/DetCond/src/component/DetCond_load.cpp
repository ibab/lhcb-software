//$Id: DetCond_load.cpp,v 1.7 2005-08-30 14:37:38 marcocle Exp $
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
  // algorithms ======================
  DECLARE_ALGORITHM( PrepareDBCache );
};


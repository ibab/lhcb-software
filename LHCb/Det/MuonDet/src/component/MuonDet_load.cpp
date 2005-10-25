// $Id: MuonDet_load.cpp,v 1.6 2005-10-25 06:56:33 asarti Exp $ 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(MuonDet) {
  
  /// Converter factories 
  DECLARE_CONVERTER( XmlMuonReadoutCondCnv );
  DECLARE_CONVERTER( XmlMuonCablingCnv );
  DECLARE_CONVERTER( XmlMuonODECnv );
  DECLARE_CONVERTER( XmlMuonTSMapCnv );

};


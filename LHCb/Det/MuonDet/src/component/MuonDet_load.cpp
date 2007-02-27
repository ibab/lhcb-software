// $Id: MuonDet_load.cpp,v 1.7 2007-02-27 11:13:58 asatta Exp $ 

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(MuonDet) {
  
  /// Converter factories 
  DECLARE_CONVERTER( XmlMuonReadoutCondCnv );
  DECLARE_CONVERTER( XmlMuonCablingCnv );
  DECLARE_CONVERTER( XmlMuonL1BoardCnv );
  DECLARE_CONVERTER( XmlMuonODECnv );
  DECLARE_CONVERTER( XmlMuonTSMapCnv );

};


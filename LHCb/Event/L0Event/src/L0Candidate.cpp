// $Id: L0Candidate.cpp,v 1.2 2006-01-24 13:14:39 cattanem Exp $
// Include files 

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "Event/L0Candidate.h"
#include <valarray>

//-----------------------------------------------------------------------------
// Auxilliary Implementation file for L0ProcessorsDataContainer Event classes
//
// 10/10/2005 : Olivier Deschamps
//-----------------------------------------------------------------------------

LHCb::L0Candidate::L0Candidate(const std::string& location){
  m_convert=false;
  IDataProviderSvc* evtSvc=0;
  ISvcLocator* locate=Gaudi::svcLocator();
  if( locate->service("EventDataSvc",evtSvc,true).isFailure() ){
    m_status = "EventDataSvc not located";}
  else{
    m_dataContainer = SmartDataPtr<L0ProcessorDatas>(evtSvc,location);
    if( !m_dataContainer ){m_status = "Container does not exist";}
    else{m_status = "OK"; }
  }
}

//=========================================================================
// pick Up the [b1,b2] field in bit words
//=========================================================================
unsigned long LHCb::L0Candidate::pick(const unsigned long word,int b1,int b2){
  unsigned long val = word;
  val >>= b2 ;
  val &= long( pow(2,(b1-b2+1) ) ) - 1  ;
  return val;
}

unsigned long LHCb::L0Candidate::value( const L0Data::Data&  data,
                                        const L0Data::Field&  field) {

  L0ProcessorData* fiber = 0;
  L0ProcessorData* fiber2 = 0;
  
  long caloEtUnit   = 1 ;
  long muonPtUnit   = 1 ;
  if( m_convert ){
    caloEtUnit = 20 ;// ADC to MeV (hardcoded -- should be extracted from ConfDB)
    muonPtUnit = 40 ;// ADC to MeV (hardcoded -- should be extracted from ConfDB)
  }

  switch( data ){
  case L0Data::Electron :
    fiber = m_dataContainer->object(L0DUFiber::CaloElectron);
    if (!fiber){m_status = "CaloElectron fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->lsb() , 8 ,  1  )*caloEtUnit; 
    case L0Data::Address : return pick( fiber->msb() , 14 , 1  ); 
    case L0Data::Status  : return pick( fiber->msb() , 15 , 15 ); 
    case L0Data::BCID    : return pick( fiber->lsb() , 15 , 9  ); 
    default : m_status = "The Electron data do not contain the required field"; return 0 ; 
    }
  case L0Data::Photon :
     fiber = m_dataContainer->object(L0DUFiber::CaloPhoton);
    if (!fiber){m_status = "CaloPhoton fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->lsb() , 8 ,  1  )*caloEtUnit; 
    case L0Data::Address : return pick( fiber->msb() , 14 , 1  ); 
    case L0Data::Status  : return pick( fiber->msb() , 15 , 15 ); 
    case L0Data::BCID    : return pick( fiber->lsb() , 15 , 9  ); 
    default :m_status = "The Photon data do not contain the required field"; return 0 ; 
    }
  case L0Data::Hadron :
     fiber = m_dataContainer->object(L0DUFiber::CaloHadron);
    if (!fiber){m_status = "CaloHadron fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->lsb() , 8 ,  1  )*caloEtUnit; 
    case L0Data::Address : return pick( fiber->msb() , 14 , 1  ); 
    case L0Data::Status  : return pick( fiber->msb() , 15 , 15 ); 
    case L0Data::BCID    : return pick( fiber->lsb() , 15 , 9  ); 
    default :m_status = "The Hadron data do not contain the required field"; return 0 ; 
    }
  case L0Data::Pi0Global :
     fiber = m_dataContainer->object(L0DUFiber::CaloPi0Global);
    if (!fiber){m_status = "CaloPi0Global fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->lsb() , 8 ,  1  )*caloEtUnit; 
    case L0Data::Address : return pick( fiber->msb() , 14 , 1  ); 
    case L0Data::Status  : return pick( fiber->msb() , 15 , 15 ); 
    case L0Data::BCID    : return pick( fiber->lsb() , 15 , 9  ); 
    default :m_status = "The Pi0Global data do not contain the required field"; return 0 ; 
    }
  case L0Data::Pi0Local :
     fiber = m_dataContainer->object(L0DUFiber::CaloPi0Local);
    if (!fiber){m_status = " CaloPi0Local fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->lsb() , 8 ,  1  )*caloEtUnit; 
    case L0Data::Address : return pick( fiber->msb() , 14 , 1  ); 
    case L0Data::Status  : return pick( fiber->msb() , 15 , 15 ); 
    case L0Data::BCID    : return pick( fiber->lsb() , 15 , 9  ); 
    default :m_status = "The Pi0Local data do not contain the required field"; return 0 ; 
    }
  case L0Data::SumEt :
     fiber = m_dataContainer->object(L0DUFiber::CaloSumEt);
    if (!fiber){m_status = "CaloSumEt fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->msb() , 14 ,  1  )*caloEtUnit; 
    case L0Data::Status  : return pick( fiber->msb() , 15 , 15 ); 
    case L0Data::BCID    : return pick( fiber->lsb() , 15 , 9  ); 
    default :m_status = "The SumEt data do not contain the required field"; return 0 ; 
    }
  case L0Data::SpdMult :
     fiber = m_dataContainer->object(L0DUFiber::CaloSpdMult);
    if (!fiber){m_status = "SpdMult fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->msb() , 14 ,  1  ); 
    case L0Data::Status  : return pick( fiber->msb() , 15 , 15 ); 
    case L0Data::BCID    : return pick( fiber->lsb() , 15 , 9  ); 
    default :m_status = "The SpdMult data do not contain the required field"; return 0 ; 
    }
  case L0Data::PuPeak1 :
     fiber = m_dataContainer->object(L0DUFiber::Pu1);
    if (!fiber){m_status = "Pu1 fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->lsb() , 8 ,  1  ); 
    case L0Data::Address : return pick( fiber->msb() , 8 ,  1  ); 
    case L0Data::Status  : return pick( fiber->msb() , 15 , 15 ); 
    case L0Data::BCID    : return pick( fiber->lsb() , 15 , 9  ); 
    default :m_status = "The PuPeak1 data do not contain the required field"; return 0 ; 
    }
  case L0Data::PuPeak2 :
     fiber = m_dataContainer->object(L0DUFiber::Pu2);
    if (!fiber){m_status = "Pu2 fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->lsb() , 8 ,  1  ); 
    case L0Data::Address : return pick( fiber->msb() , 8 ,  1  ); 
    case L0Data::Status  : return pick( fiber->msb() , 15 , 15 ); 
    case L0Data::BCID    : return pick( fiber->lsb() , 15 , 9  ); 
    default :m_status = "The PuPeak2 data do not contain the required field"; return 0 ; 
    }
  case L0Data::PuHits :
     fiber = m_dataContainer->object(L0DUFiber::Pu2);
    if (!fiber){m_status = "Pu2 fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->lsb() , 10 ,  9  ) | ( pick (  fiber->msb() , 14, 9 ) << 2 );
    default :m_status = "The PuHits data do not contain the required field"; return 0 ; 
    }
  case L0Data::Muon1 :
     fiber = m_dataContainer->object(L0DUFiber::MuonCU0);
     fiber2 = m_dataContainer->object(L0DUFiber::MuonSU0);
    if (!fiber){m_status = "MuonCU0 fiber not found"; return 0;}
    if (!fiber2){m_status = "MuonSU0 fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->msb() , 7 ,  1  )*muonPtUnit;
    case L0Data::Sign    : return pick( fiber2->msb() , 7 ,  7  );
    case L0Data::Address : return pick( fiber->msb() , 14 , 8  ) | pick( fiber2->msb() , 6 , 1  ) << 7; 
    case L0Data::Status  : return pick( fiber2->msb() , 15, 12  );
    case L0Data::BCID    : return pick( fiber2->lsb() , 15, 9 ); 
    default :m_status = "The Muon1 data do not contain the required field"; return 0 ; 
    }
  case L0Data::Muon3 :
     fiber = m_dataContainer->object(L0DUFiber::MuonCU1);
     fiber2 = m_dataContainer->object(L0DUFiber::MuonSU1);
    if (!fiber){m_status = " MuonCU1 fiber not found"; return 0;}
    if (!fiber2){m_status = "MuonsU1 fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->msb() , 7 ,  1  )*muonPtUnit;
    case L0Data::Sign    : return pick( fiber2->msb() , 7 ,  7  );
    case L0Data::Address : return pick( fiber->msb() , 14 , 8  ) | pick( fiber2->msb() , 6 , 1  ) << 7; 
    case L0Data::Status  : return pick( fiber2->msb() , 15, 12  );
    case L0Data::BCID    : return pick( fiber2->lsb() , 15, 9 ); 
    default :m_status = "The Muon3 data do not contain the required field"; return 0 ; 
    }
  case L0Data::Muon5 :
     fiber = m_dataContainer->object(L0DUFiber::MuonCU2);
     fiber2 = m_dataContainer->object(L0DUFiber::MuonSU2);
    if (!fiber){m_status = "MuonCU2 fiber not found"; return 0;}
    if (!fiber2){m_status = "MuonSU2 fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->msb() , 7 ,  1  )*muonPtUnit;
    case L0Data::Sign    : return pick( fiber2->msb() , 7 ,  7  );
    case L0Data::Address : return pick( fiber->msb() , 14 , 8  ) | pick( fiber2->msb() , 6 , 1  ) << 7; 
    case L0Data::Status  : return pick( fiber2->msb() , 15, 12  );
    case L0Data::BCID    : return pick( fiber2->lsb() , 15, 9 ); 
    default :m_status = "The Muon5 data do not contain the required field"; return 0 ; 
    }
      case L0Data::Muon7 :
     fiber = m_dataContainer->object(L0DUFiber::MuonCU3);
     fiber2 = m_dataContainer->object(L0DUFiber::MuonSU3);
    if (!fiber){m_status = "MuonCU3 fiber not found"; return 0;}
    if (!fiber2){m_status = "MuonSU3 fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->msb() , 7 ,  1  )*muonPtUnit;
    case L0Data::Sign    : return pick( fiber2->msb() , 7 ,  7  );
    case L0Data::Address : return pick( fiber->msb() , 14 , 8  ) | pick( fiber2->msb() , 6 , 1  ) << 7; 
    case L0Data::Status  : return pick( fiber2->msb() , 15, 12  );
    case L0Data::BCID    : return pick( fiber2->lsb() , 15, 9 ); 
    default :m_status = "The Muon7 data do not contain the required field"; return 0 ; 
    }
  case L0Data::Muon2 :
     fiber = m_dataContainer->object(L0DUFiber::MuonCU0);
     fiber2 = m_dataContainer->object(L0DUFiber::MuonSU0);
    if (!fiber){m_status = "MuonCU0 fiber not found"; return 0;}
    if (!fiber2){m_status = "MuonSU0 required fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->lsb() , 7 ,  1  )*muonPtUnit;
    case L0Data::Sign    : return pick( fiber2->lsb() , 7 ,  7  );
    case L0Data::Address : return pick( fiber->lsb() , 14 , 8  ) | pick( fiber2->lsb() , 6 , 1  ) << 7; 
    case L0Data::Status  : return pick( fiber2->msb() , 15, 12  );
    case L0Data::BCID    : return pick( fiber2->lsb() , 15, 9 ); 
    default :m_status = "The Muon2 data do not contain the required field"; return 0 ; 
    }
  case L0Data::Muon4 :
     fiber = m_dataContainer->object(L0DUFiber::MuonCU1);
     fiber2 = m_dataContainer->object(L0DUFiber::MuonSU1);
    if (!fiber){m_status = "MuonCU1 fiber not found"; return 0;}
    if (!fiber2){m_status = "MuonSU1 fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->lsb() , 7 ,  1  )*muonPtUnit;
    case L0Data::Sign    : return pick( fiber2->lsb() , 7 ,  7  );
    case L0Data::Address : return pick( fiber->lsb() , 14 , 8  ) | pick( fiber2->lsb() , 6 , 1  ) << 7; 
    case L0Data::Status  : return pick( fiber2->msb() , 15, 12  );
    case L0Data::BCID    : return pick( fiber2->lsb() , 15, 9 ); 
    default :m_status = "The Muon4 data do not contain the required field"; return 0 ; 
    }
  case L0Data::Muon6 :
     fiber = m_dataContainer->object(L0DUFiber::MuonCU2);
     fiber2 = m_dataContainer->object(L0DUFiber::MuonSU2);
    if (!fiber){m_status = "MuonCU2 fiber not found"; return 0;}
    if (!fiber2){m_status = "MuonSU2 fiber not found"; return 0;}
    switch( field ){
    case L0Data::Value   : return pick( fiber->lsb() , 7 ,  1  )*muonPtUnit;
    case L0Data::Sign    : return pick( fiber2->lsb() , 7 ,  7  );
    case L0Data::Address : return pick( fiber->lsb() , 14 , 8  ) | pick( fiber2->lsb() , 6 , 1  ) << 7; 
    case L0Data::Status  : return pick( fiber2->msb() , 15, 12  );
    case L0Data::BCID    : return pick( fiber2->lsb() , 15, 9 ); 
    default :m_status = "The Muon6 data do not contain the required field"; return 0 ; 
    }
 case L0Data::Muon8 :
    fiber = m_dataContainer->object(L0DUFiber::MuonCU3);
    fiber2 = m_dataContainer->object(L0DUFiber::MuonSU3);
    if (!fiber){m_status = "MuonCU3 fiber not found"; return 0;}
    if (!fiber2){m_status = "MuonSU3 fiber not found"; return 0;}
   switch( field ){
   case L0Data::Value   : return pick( fiber->lsb() , 7 ,  1  )*muonPtUnit;
   case L0Data::Sign    : return pick( fiber2->lsb() , 7 ,  7  );
   case L0Data::Address : return pick( fiber->lsb() , 14 , 8  ) | pick( fiber2->lsb() , 6 , 1  ) << 7; 
   case L0Data::Status  : return pick( fiber2->msb() , 15, 12  );
   case L0Data::BCID    : return pick( fiber2->lsb() , 15, 9 ); 
   default :m_status = "The Muon8 data do not contain the required field"; return 0 ; 
   }
 default : return 0;
}
return 0;
}

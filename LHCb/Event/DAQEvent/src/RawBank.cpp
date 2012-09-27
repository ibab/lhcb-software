// $Id: RawBank.cpp,v 1.9 2010-01-15 13:20:13 marcin Exp $

// local
#include "Event/RawBank.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RawBank
//
// 2008-02-13 : Marco Cattaneo
//-----------------------------------------------------------------------------

//=============================================================================
std::string LHCb::RawBank::typeName(LHCb::RawBank::BankType e){
  switch ( e ) {
    //
    // Important note:
    // Keep the order exactly as in the RawEvent header file!
    //
  case LHCb::RawBank::L0Calo          : return  "L0Calo";
  case LHCb::RawBank::L0DU            : return  "L0DU";            
  case LHCb::RawBank::PrsE            : return  "PrsE";            
  case LHCb::RawBank::EcalE           : return  "EcalE";           
  case LHCb::RawBank::HcalE           : return  "HcalE";           
  case LHCb::RawBank::PrsTrig         : return  "PrsTrig";         
  case LHCb::RawBank::EcalTrig        : return  "EcalTrig";        
  case LHCb::RawBank::HcalTrig        : return  "HcalTrig";        
  case LHCb::RawBank::Velo            : return  "Velo";            
  case LHCb::RawBank::Rich            : return  "Rich";            
  case LHCb::RawBank::TT              : return  "TT";              
  case LHCb::RawBank::IT              : return  "IT";              
  case LHCb::RawBank::OT              : return  "OT";              
  case LHCb::RawBank::Muon            : return  "Muon";            
  case LHCb::RawBank::L0PU            : return  "L0PU";            
  case LHCb::RawBank::DAQ             : return  "DAQ";             
  case LHCb::RawBank::ODIN            : return  "ODIN";            
  case LHCb::RawBank::HltDecReports   : return  "HltDecReports";    
  case LHCb::RawBank::VeloFull        : return  "VeloFull";        
  case LHCb::RawBank::TTFull          : return  "TTFull";         
  case LHCb::RawBank::ITFull          : return  "ITFull";          
  case LHCb::RawBank::EcalPacked      : return  "EcalPacked";      
  case LHCb::RawBank::HcalPacked      : return  "HcalPacked";      
  case LHCb::RawBank::PrsPacked       : return  "PrsPacked";       
  case LHCb::RawBank::L0Muon          : return  "L0Muon";          
  case LHCb::RawBank::ITError         : return  "ITError";         
  case LHCb::RawBank::TTError         : return  "TTError";  
  case LHCb::RawBank::ITPedestal      : return  "ITPedestal";      
  case LHCb::RawBank::TTPedestal      : return  "TTPedestal";      
  case LHCb::RawBank::VeloError       : return  "VeloError";       
  case LHCb::RawBank::VeloPedestal    : return  "VeloPedestal";    
  case LHCb::RawBank::VeloProcFull    : return  "VeloProcFull";    
  case LHCb::RawBank::OTRaw           : return  "OTRaw";           
  case LHCb::RawBank::OTError         : return  "OTError";         
  case LHCb::RawBank::EcalPackedError : return  "EcalPackedError"; 
  case LHCb::RawBank::HcalPackedError : return  "HcalPackedError"; 
  case LHCb::RawBank::PrsPackedError  : return  "PrsPackedError";  
  case LHCb::RawBank::L0CaloFull      : return  "L0CaloFull";      
  case LHCb::RawBank::L0CaloError     : return  "L0CaloError";     
  case LHCb::RawBank::L0MuonCtrlAll   : return  "L0MuonCtrlAll";   
  case LHCb::RawBank::L0MuonProcCand  : return  "L0MuonProcCand";  
  case LHCb::RawBank::L0MuonProcData  : return  "L0MuonProcData";  
  case LHCb::RawBank::L0MuonRaw       : return  "L0MuonRaw";       
  case LHCb::RawBank::L0MuonError     : return  "L0MuonError";     
  case LHCb::RawBank::GaudiSerialize  : return  "GaudiSerialize";  
  case LHCb::RawBank::GaudiHeader     : return  "GaudiHeader";     
  case LHCb::RawBank::TTProcFull      : return  "TTProcFull";      
  case LHCb::RawBank::ITProcFull      : return  "ITProcFull";      
  case LHCb::RawBank::TAEHeader       : return  "TAEHeader";       
  case LHCb::RawBank::MuonFull        : return  "MuonFull";        
  case LHCb::RawBank::MuonError       : return  "MuonError";
  case LHCb::RawBank::TestDet         : return  "TestDet";
  case LHCb::RawBank::L0DUError       : return  "L0DUError";
  case LHCb::RawBank::HltRoutingBits  : return  "HltRoutingBits";
  case LHCb::RawBank::HltSelReports   : return  "HltSelReports";
  case LHCb::RawBank::HltVertexReports: return  "HltVertexReports";
  case LHCb::RawBank::HltLumiSummary  : return  "HltLumiSummary";
  case LHCb::RawBank::L0PUFull        : return  "L0PUFull";
  case LHCb::RawBank::L0PUError       : return  "L0PUError";
  case LHCb::RawBank::DstBank         : return  "DstBank";
  case LHCb::RawBank::DstData         : return  "DstData";
  case LHCb::RawBank::DstAddress      : return  "DstAddress";    
  case LHCb::RawBank::FileID          : return  "FileID";
  case LHCb::RawBank::VP              : return  "VP";
  case LHCb::RawBank::FTCluster       : return  "FTCluster";  
  case LHCb::RawBank::VL              : return  "VL"; 
  case LHCb::RawBank::UT              : return  "UT";
  case LHCb::RawBank::UTFull          : return  "UTFull";
  case LHCb::RawBank::UTError         : return  "UTError";
  case LHCb::RawBank::UTPedestal      : return  "UTPedestal";
  default                             : return  "Undefined_name";
  };
}  

//=============================================================================

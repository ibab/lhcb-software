// $Id: CaloReadoutTool.cpp,v 1.7 2007-02-22 23:40:49 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "CaloDAQ/CaloReadoutTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloReadoutTool
//
// base class for Calo readout tools 
// (CaloEnergyFromRaw, CaloTriggerAdcsFromRaw and CaloTriggerBitsFromRaw)
//
// 2007-02-01 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloReadoutTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloReadoutTool::CaloReadoutTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<CaloReadoutTool>(this);

  declareProperty( "DetectorName"   , m_detectorName );
  declareProperty( "PackedIsDefault", m_packedIsDefault = false);
  m_getRaw = true;
}
//=============================================================================
// Destructor
//=============================================================================
CaloReadoutTool::~CaloReadoutTool() {} 



//=========================================================================
//  Get required CaloBanks (short or packed format) - Fill m_banks
//=========================================================================
StatusCode CaloReadoutTool::getCaloBanksFromRaw( ) {

  LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( rootOnTES() + LHCb::RawEventLocation::Default );
  m_banks = 0;
  if( !m_packedIsDefault){
    debug() << "Banks of short type are requested as default" << endreq;
    m_banks= &rawEvt->banks(  m_shortType );
  }else{
    debug() << "Banks of paked type are requested as default" << endreq;
    m_banks= &rawEvt->banks(  m_packedType );
  }
  
  
  if ( 0 == m_banks || 0 == m_banks->size() ) {
    if( !m_packedIsDefault){      
      debug()<< " Requested banks of short type has not been found ... try packed type" << endreq;
      m_banks = &rawEvt->banks( m_packedType );
    }else{
      debug()<< " Requested banks of packed type has not been found ... try short type" << endreq;
      m_banks = &rawEvt->banks( m_shortType );
    }    

    if ( 0 == m_banks || 0 == m_banks->size() ){
      error() << "None of short and packed banks has been found " << endreq;
    return StatusCode::FAILURE;
    }else{
      if( !m_packedIsDefault){      
        debug()<< " Requested banks of packed type has been found" << endreq;
      }else{
        debug()<< " Requested banks of short type has found" << endreq;
      }
    }
  }else{
    if( !m_packedIsDefault){      
      debug()<< " Requested banks of short type has been found" << endreq;
    }else{
      debug()<< " Requested banks of packed type has found" << endreq;
    }
  }
  return StatusCode::SUCCESS;
}



//========================
//  Check FE-Cards is PIN
//========================
void CaloReadoutTool::checkCards(int nCards, std::vector<int> feCards ){
  debug() << nCards-feCards.size() << "FE-Cards have been read among the " << nCards << " expected"<< endreq; 
  if( 0 != feCards.size() ){
    for(unsigned int iFe = 0 ; iFe <  feCards.size();++iFe){ 
     debug() << " Unread FE-Cards : " << m_calo->cardCode( feCards[iFe] ) 
             << "  - Is it a PinDiode readout FE-Card ? " << m_calo->isPinCard( feCards[iFe] ) << endreq;
     if ( !m_calo->isPinCard( feCards[iFe] ) )
       warning() << " The standard FE-Card " << m_calo->cardCode( feCards[iFe] )  
                 << " expected in TELL1 bank has not been read !!"<< endreq;
    }    
  }
}




//========================
//  Check FE-Cards is PIN
//========================
int CaloReadoutTool::findCardbyCode(std::vector<int> feCards , int code){
  for(unsigned int iFe = 0 ; iFe <  feCards.size();++iFe){ 
    if( code == m_calo->cardCode( feCards[iFe] ) ){
      debug() <<" FE-Card [code : " << code 
              << "] has been found with (num : " << feCards[iFe] <<")  in condDB" << endreq;
      return iFe;
      break;
    }        
  }
  error() << " FE-Card [code : " << code << "] has not been found" << endreq;
  return -1;
}    

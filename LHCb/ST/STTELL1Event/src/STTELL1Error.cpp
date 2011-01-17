
#include "Event/STTELL1Error.h"
#include <bitset>
#include <map>

//-----------------------------------------------------------------------------
// Implementation file for class : STTELL1Error
//
// 2008-06-24 : M Needham
//-----------------------------------------------------------------------------

std::ostream& LHCb::STTELL1Error::fillStream(std::ostream& s) const
{

  s  << "Tell1 Sent " << sentWords() << " "  << " words " << std::endl; 

  if (ErrorBankLength() == 0x14){
    s << "PP FPGA has error info :"  << std::endl ;
    s  << "(Event Info) Chip Addr (dec): " << ChipAddr()  << std::endl;
    s  << " |Sync RAM Full (bits): " << std::bitset<6>(SyncRAMFull()) << std::endl;
    s  << " |TLK Link Loss (bits): (bits) " <<  std::bitset<6>(tlkLnkLoss()) << std::endl;
    s  << "(Event Info) |Sync Evt Size Err. (bits): " <<  std::bitset<6>(SyncEvtSizeError()) << std::endl;
    s  << " |Opt. Link Disable (bits): " <<  std::bitset<6>(OptLnkDisable()) << std::endl;  
    s  << " |Opt. Link NoEvent (bits): " <<  std::bitset<6>(OptLnkNoEvt()) << std::endl;
    if( SyncRAMFull() != 0) s << "Sync RAM Full ERROR! " 
 					       << ", Value: " << std::bitset<6>(SyncRAMFull()) << std::endl; 
    if( tlkLnkLoss() != 0) s << "TLK Link loss ERROR! " 
					     << ", Value: " << std::bitset<6>(tlkLnkLoss()) << std::endl;
    if( SyncEvtSizeError() != 0) s << "Sync Event size ERROR !" 
					     << ", Value: " << std::bitset<6>(SyncEvtSizeError()) << std::endl;
    if( OptLnkNoEvt() != 0)  s<< "Optical Link no Event ERROR! " 
				   << ", Value: " <<  std::bitset<6>(OptLnkNoEvt()) << std::endl ;
 

    s << "(Event Info) Reserved bit (bits): " << std::bitset<1>(R2())
      << " |PCN Error (bits):" << std::bitset<1>(pcnError())
      << " |Optical Link no clock (bits): " << std::bitset<6>(OptLnkNoClock()) << std::endl;

    // word 6 --> pseudo error
    s << "(Event Info) |Header Pseudo Err. (bits): " << std::bitset<24>(HeaderPseudoError()) << std::endl;

    s << "(Event Info) Beetle3 PCN (dec): " <<  pcnBeetle3()
      << " |Beetle2 PCN (dec): " <<  pcnBeetle2()
      << " |Beetle1 PCN (dec): " <<  pcnBeetle1()
      << " |Beetle0 PCN (dec): " <<  pcnBeetle0()  << std::endl;


    s << "(Event Info) Reserved bits (hex): " << std::hex << R3()
      << " |Beetle5 PCN (dec): " <<  std::dec << pcnBeetle5()
      << " |Beetle4 PCN (dec): " <<  pcnBeetle4() << std::endl;

    s << "(Event Info) Reserved bits (hex): " <<  std::hex << R4()
      << " |I Headers: |Beet.5 (dec): " << std::dec  << iHeaderBeetle5()
      << " |Beet.4: " <<  iHeaderBeetle4()
      << " |Beet.3: " <<  iHeaderBeetle3()
      << " |Beet.2: " <<  iHeaderBeetle2()
      << " |Beet.1: " <<  iHeaderBeetle1()
      << " |Beet.0: " <<  iHeaderBeetle0() <<std::endl;
  }

  // optional nsz and pedestal bank words
  if(hasNZS() == true){
    s << "NSZ bank length: " << nzsBankLength()  << std::endl;       
  }
  if (hasPed() == true) {
    s << "Pedestal bank length: " << PedBankLength() << std::endl;
  }

  if (ErrorBankLength() == 0x14 ){
    s << "Summary info of errors" << std::endl;
    const unsigned pcn = pcnVote();
    s << "majority PCN vote: " << pcn << std::endl;
    for (unsigned int iLink = 0u; iLink < nBeetle ; ++iLink){
      for (unsigned int iPort = 0u; iPort < nPort ; ++iPort){
        s << "Beetle:"  << iLink << " Port "<<  iPort << "Status: " <<linkInfo(iLink, iPort , pcn) << std::endl;
      } // port
    } // link
  }
  return s << "################################################" << std::endl;
}

bool LHCb::STTELL1Error::badLink ( const unsigned int beetle,
                                   const unsigned int port, 
                                   const unsigned int testpcn ) const{

  return (linkInfo(beetle,port,testpcn) == STTELL1Error::kNone ? false : true); 
}

bool  LHCb::STTELL1Error::addLinkInfo (const unsigned int key,  
                                   const LHCb::STTELL1Error::FailureMode& mode ) { 
  return m_badLinks.insert( key , mode ).second ;
}


double LHCb::STTELL1Error::fractionOK(const unsigned int pcn) const{
  unsigned int goodLinks = 0u;
  for (unsigned int iLink = 0u; iLink < nBeetle ; ++iLink){
    for (unsigned int iPort = 0u; iPort < nPort ; ++iPort){
      if (!badLink(iLink,iPort,pcn)) ++goodLinks;
    } // port
  }  // link
  return goodLinks/double(totalNumberOfPorts); 
} 

LHCb::STTELL1Error::FailureMode LHCb::STTELL1Error::linkInfo(const unsigned int beetle, 
                                                             const unsigned int port, 
                                                             const unsigned int testpcn) const
{

  // if link is not in error we have nothhing to do
  if (hasErrorInfo() == false) return STTELL1Error::kNone;
 
  const unsigned int key = linkID(beetle,port);
  LHCb::STTELL1Error::FailureInfo::iterator i = m_badLinks.find( key ) ;
  if (m_badLinks.end() !=i) { 
    return i->second;
  }

  // we have to look at the pcn
  const unsigned int pcn = findPCN(beetle);
  return (pcn != testpcn ? STTELL1Error::kWrongPCN : STTELL1Error::kNone); 
}

unsigned int LHCb::STTELL1Error::findPCN(const unsigned int beetle) const{

  // get the pcn for a given beetle
    switch (beetle) {
      case 0: return pcnBeetle0();
      case 1: return pcnBeetle1();
      case 2: return pcnBeetle2();
      case 3: return pcnBeetle3();
      case 4: return pcnBeetle4();
      case 5: return pcnBeetle5();
      default : return 200u; // 
    }
}


void LHCb::STTELL1Error::fillErrorInfo() {

  for (unsigned int iLink = 0; iLink < nBeetle ; ++iLink){
    if ( !correctPatterns() )
      {
        flagBadLinks(iLink,kCorruptedBank);
      }
    else if ( OptLnkDisable() >> iLink & 1 ) {
      flagBadLinks(iLink,kOptLinkDisabled);      
    }   
    else  if ( tlkLnkLoss() >> iLink & 1 ) {
      flagBadLinks(iLink, kTlkLinkLoss);      
    }   
    else if (OptLnkNoEvt() >> iLink & 1) {
      flagBadLinks(iLink, kOptLinkNoEvent);
     } 
    else if (SyncRAMFull() >> iLink & 1) {
      flagBadLinks(iLink, kSyncRAMFull);
    }
    else if (SyncEvtSizeError() >> iLink & 1) {
      flagBadLinks(iLink, kSyncEvtSize);
    }
    else if (OptLnkNoClock() >> iLink & 1){
      flagBadLinks(iLink, kOptLinkNoClock);
    }
    else {
      // pseudo error = 1 word per port
      for (unsigned int iPort = 0 ; iPort < nPort; ++iPort ){
	const unsigned int link = linkID(iLink,iPort);
        if ( HeaderPseudoError() >> link & 1) addLinkInfo(link, kPseudoHeader); 
      } // iport 
    } // if
  } // iOLink  

}

bool LHCb::STTELL1Error::correctPatterns() const
{
  if ( ! ((m_word4 & 0xFF) == 0) && (((m_word4 & 0xFF00) / 0x100) == 0x8e) )
    return false;
  
  if ( ! ((m_word4 & 0xFF) == 0) && (((m_word4 & 0xFF00) / 0x100) == 0x8e) )
    return false;

  if ( ! ((m_word11 & 0xFF) == 2) && (((m_word11 & 0xFF00) / 0x100) == 0x8e) )
    return false;

  if (hasNZS())
  {
    if ( ! ((m_word12 & 0xFF) == 3) && (((m_word12 & 0xFF00) / 0x100) == 0x8e) )
      return false;
  }
  

  if (hasPed())
  {
    if ( ! ((m_word13 & 0xFF) == 4) && (((m_word13 & 0xFF00) / 0x100) == 0x8e) )
      return false;
  }
  

  return true;
    
  /*
  bool isOK(true);

  // testing word4 Error bank
  isOK  *= ((m_word4 & 0xFF) == 0) * (((m_word4 & 0xFF00) / 0x100) == 0x8e);

  // testing word10 Cluster bank
  isOK *= ((m_word10 & 0xFF) == 1) * (((m_word10 & 0xFF00) / 0x100) == 0x8e);

  // testing word11 ADC bank
  isOK *= ((m_word11 & 0xFF) == 2) * (((m_word11 & 0xFF00) / 0x100) == 0x8e);

  if (hasNZS())
    // testing word12 NZS bank
    isOK *= ((m_word12 & 0xFF) == 3) * (((m_word12 & 0xFF00) / 0x100) == 0x8e);
  

  if (hasPed())
    // testing word13 Pedestal bank
    isOK *= ((m_word13 & 0xFF) == 4) * (((m_word13 & 0xFF00) / 0x100) == 0x8e);

  return isOK;
  */
}


unsigned int LHCb::STTELL1Error::pcnVote() const{

  std::map<unsigned int, unsigned int> pcns;
  for (unsigned int iLink = 0; iLink < nBeetle ; ++iLink){
    unsigned int iPort = 0;
    bool good = true;
    while (iPort < nPort && good){
      const unsigned int key = linkID(iLink,iPort);       
      LHCb::STTELL1Error::FailureInfo::iterator i = m_badLinks.find( key ) ;
      if (i != m_badLinks.end()) good = false; 
      ++iPort;
    }  // port
    if (good == true){
      // we are allowed to vote [the US system]
      pcns[findPCN(iLink)] += 1;     
    }
  }  // link

  // find the best
  unsigned int majorityVote = 200;
  unsigned int maxValue = 0;
  std::map<unsigned int, unsigned int>::iterator iter = pcns.begin();
  for (; iter != pcns.end() ; ++iter){
    if (iter->second > maxValue){
      majorityVote = iter->first;
      maxValue = iter->second;
    }
  } // iter

  return majorityVote;
}


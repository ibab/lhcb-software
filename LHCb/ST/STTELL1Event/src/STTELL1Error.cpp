
#include "Event/STTELL1Error.h"
#include <bitset>

//-----------------------------------------------------------------------------
// Implementation file for class : STTELL1Error
//
// 2008-06-24 : M Needham
//-----------------------------------------------------------------------------

std::ostream& LHCb::STTELL1Error::fillStream(std::ostream& s) const
{
  s  << "################################################" << std::endl;
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
 
  return s << "################################################" << std::endl;
}

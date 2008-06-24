
#include "Event/STTELL1Error.h"
#include <bitset>

//-----------------------------------------------------------------------------
// Implementation file for class : STChannelID
//
// 2007-07-11 : Chris Jones
//-----------------------------------------------------------------------------

std::ostream& LHCb::STTELL1Error::fillStream(std::ostream& s) const
{
  s  << "################################################" << std::endl;
  s  << "Tell1 Sent " << sentWords() << " "  << " words " << std::endl; 
  s  << "(Event Info) Chip Addr (dec): " << (m_word4 & 0xc0000000)/0x40000000  << std::endl;
  s  << " |Sync RAM Full (bits): " << std::bitset<6>((m_word4 & 0x3f000000)/0x1000000) << std::endl;
  s  << " |TLK Link Loss (bits): (bits) " <<  std::bitset<6>((m_word4 & 0x00fc0000)/0x40000) << std::endl;
  s  << "(Event Info) |Sync Evt Size Err. (bits): " <<  std::bitset<6>((m_word4 & 0x0003f000)/0x1000 ) << std::endl;
  s  << " |Opt. Link Disable (bits): " <<  std::bitset<6>((m_word4 & 0x00000fc0)/0x40) << std::endl;  
  s  << " |Opt. Link NoEvent (bits): " <<  std::bitset<6>((m_word4 & 0x0000003f)) << std::endl;
  if( (m_word4 & 0x3f000000)/0x1000000 != 0) s << "Sync RAM Full ERROR! " 
					       << ", Value: " << std::bitset<6>((m_word4 & 0x3f000000)/0x1000000) << std::endl; 
  if( (m_word4 & 0x00fc0000)/0x40000 != 0) s << "TLK Link loss ERROR! " 
					     << ", Value: " << std::bitset<6>((m_word4 & 0x00fc0000)/0x40000) << std::endl;
  if( (m_word4 & 0x0003f000)/0x1000  != 0) s << "Sync Event size ERROR !" 
					     << ", Value: " << std::bitset<6>((m_word4 & 0x0003f000)/0x1000) << std::endl;
  if( m_word4 & 0x0000003f != 0)  s<< "Optical Link no Event ERROR! " 
				   << ", Value: " <<  std::bitset<6>(m_word4 & 0x0000003f) << std::endl ;

  if (ErrorBankLength() == 0x14){
    // print out the error bank information
    s << "PP FPGA has error info :"  << std::endl ;
    s << "(Event Info) Reserved bit (bits): " << std::bitset<1>((m_word5 & 0x80000000)/0x80000000)
      << " |PCN Error (bits):" << std::bitset<1>((m_word5 & 0x40000000)/0x40000000)
      << " |Optical Link no clock (bits): " << std::bitset<6>((m_word5 & 0x3f000000)/0x1000000) << std::endl;

    // word 6 --> pseudo error
    s << "(Event Info) |Header Pseudo Err. (bits): " << std::bitset<24>(m_word6 & 0x00ffffff) << std::endl;

    s << "(Event Info) Beetle3 PCN (dec): " <<  (m_word7 & 0xff000000)/1000000
      << " |Beetle2 PCN (dec): " <<  (m_word7 & 0x00ff0000)/10000
      << " |Beetle1 PCN (dec): " <<  (m_word7 & 0x0000ff00)/100
      << " |Beetle0 PCN (dec): " <<  (m_word7 & 0x000000ff)  << std::endl;


    s << "(Event Info) Reserved bits (hex): " << std::hex << (m_word8 & 0xffff0000)/0x00010000
      << " |Beetle5 PCN (dec): " <<  std::dec << (m_word8 & 0x0000ff00)/100
      << " |Beetle4 PCN (dec): " <<  (m_word8 & 0x000000ff) << std::endl;

    s << "(Event Info) Reserved bits (hex): " <<  std::hex <<(m_word9 & 0xff000000)/1000000
      << " |I Headers: |Beet.5 (dec): " << std::dec  << (m_word9 & 0x00f00000)/0x100000
      << " |Beet.4: " << (m_word9 & 0x000f0000)/0x10000
      << " |Beet.3: " <<  (m_word9 & 0x0000f000)/0x1000
      << " |Beet.2: " <<   (m_word9 & 0x00000f00)/0x100
      << " |Beet.1: " << (m_word9 & 0x000000f0)/0x10
      << " |Beet.0: " <<   (m_word9 & 0x0000000f) <<std::endl;
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

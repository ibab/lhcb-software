
#include "Event/STTELL1EventInfo.h"
#include <map>

//-----------------------------------------------------------------------------
// Implementation file for class : STTELL1EventInfo
//
// 2008-07-17 : J Luisier
//-----------------------------------------------------------------------------

unsigned int LHCb::STTELL1EventInfo::findPCN(const unsigned int beetle) const{

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

    return 200u;
}

unsigned int LHCb::STTELL1EventInfo::findIHeader(const unsigned int beetle) const{  // get the pcn for a given beetle
    switch (beetle) {
      case 0: return iHeaderBeetle0();
      case 1: return iHeaderBeetle1();
      case 2: return iHeaderBeetle2();
      case 3: return iHeaderBeetle3();
      case 4: return iHeaderBeetle4();
      case 5: return iHeaderBeetle5();
      default : return 200u; // 
    }

    return 200u;
}


unsigned int LHCb::STTELL1EventInfo::pcnVote() const{
  
  std::map<unsigned int, unsigned int> pcns;
  for (unsigned int iLink = 0; iLink < nBeetle ; ++iLink)
    pcns[findPCN(iLink)] += 1;     
   
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

bool LHCb::STTELL1EventInfo::pcnConsistency() const
{
  unsigned int pcnv(pcnVote());

  if ( pcnv == pcnBeetle0() && pcnv == pcnBeetle1() &&
       pcnv == pcnBeetle2() && pcnv == pcnBeetle3() &&
       pcnv == pcnBeetle4() && pcnv == pcnBeetle5() )
    return true;
  else
    return false;

  /*
    bool isConsistent(true);

    isConsistent *= (pcnv == pcnBeetle0());
    isConsistent *= (pcnv == pcnBeetle1());
    isConsistent *= (pcnv == pcnBeetle2());
    isConsistent *= (pcnv == pcnBeetle3());
    isConsistent *= (pcnv == pcnBeetle4());
    isConsistent *= (pcnv == pcnBeetle5());

    return isConsistent;
  */
}

bool LHCb::STTELL1EventInfo::linkEnabled(const unsigned int iLink) const{
  return  (OptLnkDisable() >> iLink & 1) ;
} 

// fillstream method
std::ostream& LHCb::STTELL1EventInfo::fillStream(std::ostream& s) const{

  s << "Print Event info " <<  ChipAddr() << std::endl;
  for (unsigned int i = 0 ; i < nBeetle; ++i){
    s << "Beetle: " << i << " PCN "  << findPCN(i)   
    <<  " IHeader " << findIHeader(i) << std::endl; 
  } //i

  return s;

}



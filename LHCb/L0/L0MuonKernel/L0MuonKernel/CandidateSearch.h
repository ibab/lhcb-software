#ifndef CANDIDATESEARCH_H     
#define CANDIDATESEARCH_H

/* class CandidateSearch CandidateSearch L0MuonKernel/CandidateSearch.h
    
  Class containing methods to search candidate hits for a given seed in 
  the CandidateTower

  author Luisanna Tocco
  date 4 July 2003

*/

#include <vector>
#include <boost/dynamic_bitset.hpp>
#include "GaudiKernel/MsgStream.h"

namespace L0Muon {

class CandidateSearch {

 public: 

  CandidateSearch();
  //CandidateSearch(std::pair<int, int> & sd);
  ~CandidateSearch();

  typedef boost::dynamic_bitset<> nbitset ; 
  void searchInSta(int sta, boost::dynamic_bitset<> & bits);
  void anyBitsInSta(int sta, boost::dynamic_bitset<> & bits);
  int makeExtrapolation(MsgStream * log );
  bool CandidateFound();
  bool hitFoundInSta(int sta) { return m_found[sta];}
  void setCandidateAddrs(std::pair<int, int> & sd); 
  nbitset getCandidateAddrs();
  nbitset getAddrs(int sta) ;

  int getHitPos(int sta){ return m_hitpos[sta];}
  
  void resetBits();
  
  
 private: 
  int m_extraM1[11];
  bool m_found[5], m_isFull;
  int m_hitpos[5];
  int m_offset, m_offsetM1 ;
  nbitset m_addrM3, m_addrM2, m_addrM1, m_addr;
  
} ; 

};
#endif











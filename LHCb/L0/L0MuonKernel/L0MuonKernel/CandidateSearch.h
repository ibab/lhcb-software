#ifndef CANDIDATESEARCH_H     
#define CANDIDATESEARCH_H

/** @class CandidateSearch CandidateSearch.h L0MuonKernel/CandidateSearch.h
    
  Class containing methods for searching candidates

  @author Luisanna Tocco
  @date 4 July 2003

*/

#include <vector>
#include <boost/dynamic_bitset.hpp>
//#include "GaudiKernel/MsgStream.h"

namespace L0Muon {

class CandidateSearch {

 public: 

  /// Constructor
  CandidateSearch();

  /// Destructor
  ~CandidateSearch();

  /// Useful type
  typedef boost::dynamic_bitset<> nbitset ; 
  
  /** Search for an hit in the candidate tower
   
      @param sta  : station
      @param bits : bit set of the candidate tower
  */
  void searchInSta(int sta, boost::dynamic_bitset<> & bits);
 
  /// Search for any bit
  void anyBitsInSta(int sta, boost::dynamic_bitset<> & bits);

  /// Make the extrapolation to M1
  int makeExtrapolation();

  /// true if a candidate is found
  bool CandidateFound();

  bool usefulEvent();

  /// true if an hit is found in station sta
  bool hitFoundInSta(int sta) { return m_found[sta];}

  /** Set the addresses for candidates:
      7 bits for (absolute) address in M3 
      4 bits for (relative) address in M2 (M3-M2) 
      4 bits for (relative) address in M1 with respect
      the field of interest
    
      @param sd : seed 
  */
  void setCandidateAddrs(std::pair<int, int> & sd); 

  /// Get the addresses of candidates
  nbitset getCandidateAddrs();

  /// Get the address in station sta
  nbitset getAddrs(int sta) ;

  /// Return the hit position in station sta
  int getHitPos(int sta);
  
  /// Reset the bits
  void resetBits();
  
  /// flag for searching without M1
  void ignoreM1(bool & ignoreM1) { m_ignoreM1 = ignoreM1; }
  
    

 private: 
  int m_extraM1[11];
  bool m_found[5], m_isFull, m_ignoreM1;
  int m_hitpos[5];
  int m_offset, m_offsetM1 ;
  nbitset m_addrM3, m_addrM2, m_addrM1, m_addr;
  
} ; 

};
#endif











#ifndef CANDIDATETOWER_H     
#define CANDIDATETOWER_H  

/* class CandidateTower CandidateTower L0MuonKernel/CandidateTower.h

   Class representing a L0Muon Trigger representation
   of data for a given seed in M3 

   author  Luisanna Tocco
   date  4 July 2003
*/

#include <vector>
#include <utility>  // For std::pair
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/Unit.h"
#include "GaudiKernel/MsgStream.h"

namespace L0Muon {

class CandidateTower {
 
 public:

  CandidateTower(); 
  CandidateTower(std::pair<int, int> seedindex); 
  ~CandidateTower();

  void setSeed(std::pair<int, int> &seedindex);
  std::pair<int, int> getSeed() { return m_seed ;}
  
  void setFoi(int sta, int xfoi, int yfoi);
  std::pair<int, int> getFoi(int sta); 
  
  //void setBit(int sta, int xFoi, int yFoi);
  void setBit(int sta, std::vector<boost::dynamic_bitset<> > table,
              int maxXFoi, int maxYFoi,
              int offset);  

  void setOrderedBit(int sta, std::vector<boost::dynamic_bitset<> > table,
              int maxXFoi, int maxYFoi,
              int offset);  


  //boost::dynamic_bitset<> getBit(int sta);
  boost::dynamic_bitset<> getBit();
  //void reset(int sta);
  void reset();
  void drawBits(int sta, MsgStream * log);

  

  typedef boost::dynamic_bitset<> nbits;
  typedef std::pair<int, int> seedind ; 

  void setPadIndex(int sta, int maxXfoi, int maxYFoi, int offset, int ind);
  void setOrderedPadIndex(int sta, 
                          int maxXfoi, int maxYFoi, int offset, int ind, 
                          MsgStream * log);
  seedind getPadIndex(int sta) { return m_xyindex[sta];}
  
  
    

 private:

  nbits m_bits;
  seedind m_seed ;
  seedind m_xyindex[5];
  bool m_flag;
  int m_xFoi[5], m_yFoi[5];
  
  
};

};

#endif









#ifndef CANDIDATETOWER_H     
#define CANDIDATETOWER_H  

/** @class CandidateTower CandidateTower.h L0MuonKernel/CandidateTower.h

   Class containing methods for searching candidates

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

  /// Default constructor
  CandidateTower(); 

  /// Construct a candidate tower for a given seed
  CandidateTower(std::pair<int, int> seedindex);

  /// Destructor 
  ~CandidateTower();

  /// Set the seed in M3
  void setSeed(std::pair<int, int> &seedindex);

  /// Get the seed 
  std::pair<int, int> getSeed() { return m_seed ;}
  
  /// Set the Foi for searching candidates
  void setFoi(int sta, int xfoi, int yfoi);

  /// Get the Foi for searching candidates
  std::pair<int, int> getFoi(int sta); 
  
  /** Extract bits in foi for searching candidates
      For M2 and M1 bits are ordered
      following the hadware order for searching.
      For M4 and M5  bits are not ordered.

  */
  void setBit(int sta, std::vector<boost::dynamic_bitset<> > table,
              int maxXFoi, int maxYFoi,
              int offset);  
  
  /** Extract bits in foi for searching candidates.
      Bits are ordered in all the stations
  */
  void setOrderedBit(int sta, std::vector<boost::dynamic_bitset<> > table,
              int maxXFoi, int maxYFoi,
              int offset);  


  /// Return extracted bits
  boost::dynamic_bitset<> getBit();
 
  /// Reset bits
  void reset(); 

  /// Draw bits
  void drawBits(int sta, MsgStream & log);

  

  typedef boost::dynamic_bitset<> nbits;
  typedef std::pair<int, int> seedind ; 

  /**
     set a index (x,y) corresponding the position 
     of the pad in the tower
   */
  void setPadIndex(int sta, int maxXfoi, int maxYFoi, int offset, int ind);

  /**
     set a index (x,y) corresponding the position 
     of the pad (in the ordered bit set ) 
     in the tower
   */
  void setOrderedPadIndex(int sta, 
                          int maxXfoi, int maxYFoi, int offset, int ind);

  /// Return the pad position in the tower
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









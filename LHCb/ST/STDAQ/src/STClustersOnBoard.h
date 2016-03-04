#ifndef  _STClustersOnBoard_H
#define _STClustersOnBoard_H

/** @class STClustersOnBoard STClustersOnBoard.h
 *
 *  Helper class for keeping track of clusters...
 *
 *  @author M Needham
 *  @date   2004-08-01
 */

#include <vector>
#include <utility>
#include <algorithm>
#include <array>

#include "Kernel/STDAQDefinitions.h"

class STClustersOnBoard {

public:

 STClustersOnBoard(unsigned int nMax);

 typedef std::pair<LHCb::STCluster*, unsigned int> boardPair; 
 typedef std::vector<boardPair> ClusterVector;

 ~STClustersOnBoard();

 void addCluster(LHCb::STCluster* aCluster);  

 ClusterVector clusters() const;

 bool inOverflow() const; 

 bool inOverflow(const unsigned int ppx) const;

 void clear();

private: 

 unsigned int m_maxClustersPerPPx;
 mutable ClusterVector m_clusCont;
 std::array<unsigned int,4> m_ppxCount;
 
 class Less_by_Channel
    : public std::binary_function<boardPair,boardPair,bool>
  {
  public:
                                                                                
    /** compare the channel of one object with the
     *  channel of another object
     *  @param obj1   first  object
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    inline bool operator() (boardPair obj1 , boardPair obj2 ) const
    {
      return obj1.second < obj2.second ;
    }
  };

};

inline STClustersOnBoard::STClustersOnBoard(unsigned int nMax):
m_maxClustersPerPPx(nMax){
  // constructer
  m_clusCont.reserve(200);
  clear();
}

inline STClustersOnBoard::~STClustersOnBoard(){
  //destructer
}

inline STClustersOnBoard::ClusterVector STClustersOnBoard::clusters() const{
  std::sort(m_clusCont.begin(),m_clusCont.end(),Less_by_Channel());
  return m_clusCont;
}

inline void STClustersOnBoard::addCluster(LHCb::STCluster* aCluster){

  const unsigned int daqChan = aCluster->tell1Channel();
  const unsigned int ppx = daqChan/STDAQ::nStripPerPPx;
  if (inOverflow(ppx) == false){
    m_clusCont.push_back(std::make_pair(aCluster, daqChan));
    ++m_ppxCount[ppx];
  }
  else {
    // data went into the void
  }
}

inline bool STClustersOnBoard::inOverflow(const unsigned int ppx) const {
  return m_ppxCount[ppx]>= m_maxClustersPerPPx ? true :false;
}

inline bool STClustersOnBoard::inOverflow() const{
  
  auto iter = m_ppxCount.begin();
  while ((iter != m_ppxCount.end()) && (*iter < m_maxClustersPerPPx )){
    ++iter;
  }
  return (iter != m_ppxCount.end() ? true : false);
}

inline void STClustersOnBoard::clear(){
  m_clusCont.clear();
  for (int i = 0; i <4; ++i) m_ppxCount[i] = 0;
}

#endif // ClustersOnBoard

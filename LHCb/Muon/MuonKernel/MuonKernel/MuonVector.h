// $Id: MuonVector.h,v 1.3 2002-03-04 16:51:04 atsareg Exp $

#ifndef MUONKERNEL_MUONVECTOR_H
#define MUONKERNEL_MUONVECTOR_H     1

#include <vector>
#include <algorithm>

/** @class MuonVector MuonVector.h MuonKernel/MuonVector.h 
   
   This is a prototype of a possible MuonVector templated class
   to keep MuonTiles and derived objects with easy access based
   on MuonTileID
   
   @author A.Tsaregorodtsev
   @date 19 December 2002
*/

#include "MuonKernel/MuonTileID.h"
#include "MuonKernel/MuonLayout.h"

template <class Tile> 
class MuonVector : public std::vector<Tile> {

public:

  // Default constructor
  MuonVector() {m_default = Tile();}
  ~MuonVector() {}
  
  /// operator[] which takes MuonTileID as an index
  Tile& operator[] (const MuonTileID& id) {
    std::vector<Tile>::iterator it = std::find(begin(), end(), id);
    if(it != end()) {
      return *it;
    } else {
      return m_default;
    }  
  }
  /// operator[] which takes int as an index. This operator
  /// was screened from std::vector<MuonTile> implementation
  Tile& operator[] (int ind){
    return std::vector<Tile>::operator[](ind);
  }
  
  MuonVector<Tile> subset(const MuonTileID& id) {
  
    MuonVector<Tile> result;
    MuonLayout idLayout = id.layout();
    std::vector<Tile>::iterator it;
    for(it=begin();it!=end();it++) {
      MuonTileID container = (*it).id().containerID(idLayout);
      if(container == id) {      
        result.push_back(*it);
      }
    }
    return result;
  }
  
private:
  Tile m_default;

};

#endif      // MUONKERNEL_MUONVECTOR_H  

// $Id: MuonVector.h,v 1.1.1.1 2002-01-28 08:48:24 atsareg Exp $

#ifndef MUONKERNEL_MUONVECTOR_H
#define MUONKERNEL_MUONVECTOR_H     1

#include <vector>
#include <algorithm>

/** @class MuonVector MuonVector.h MuonKernel/MuonVector.h 
   
   This is a prototype of a possible MuonVector templated class
   to keep MuonTiles and derived objects with easy access based
   on MuonSystemID
   
   @author A.Tsaregorodtsev
   @date 19 December 2002
*/

#include "MuonKernel/MuonSystemID.h"
#include "MuonKernel/MuonLayout.h"

template <class MuonTile> 
class MuonVector : public std::vector<MuonTile> {

public:

  // Default constructor
  MuonVector() {m_default = MuonTile();}
  ~MuonVector() {}
  
  /// operator[] which takes MuonSystemID as an index
  MuonTile& operator[] (const MuonSystemID& id) {
    std::vector<MuonTile>::iterator it=std::find(begin(), end(), id);
    if(it != end()) {
      return *it;
    } else {
      return m_default;
    }  
  }
  /// operator[] which takes MuonSystemID as an index. This operator
  /// was screened from std::vector<MuonTile> implementation
  MuonTile& operator[] (int ind){
    return std::vector<MuonTile>::operator[](ind);
  }
  
  MuonVector<MuonTile> subset(const MuonSystemID& id) {
  
    MuonVector<MuonTile> result;
    MuonLayout idLayout = id.layout();
    std::vector<MuonTile>::iterator it;
    for(it=begin();it!=end();it++) {
      MuonSystemID container = (*it).id().getContainerID(idLayout);
      if(container == id) {      
        result.push_back(*it);
      }
    }
    return result;
  }
  
private:
  MuonTile m_default;

};

#endif      // MUONKERNEL_MUONVECTOR_H  

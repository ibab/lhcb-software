// $Id: SortTileInTU.h,v 1.1 2008-04-02 11:48:20 asatta Exp $
#ifndef LIB_SORTTILEINTU_H 
#define LIB_SORTTILEINTU_H 1
#include <iostream>
#include <functional>

#include "Kernel/MuonTileID.h"
// Include files

/** @class SortTileInTU SortTileInTU.h Lib/SortTileInTU.h
 *  
 *
 *  @author 
 *  @date   2008-02-24
 */
class SortTileInTU :
  public std::binary_function<LHCb::MuonTileID,LHCb::MuonTileID,bool>{
public :
  bool operator()(  const LHCb::MuonTileID& first, const LHCb::MuonTileID& second )
    const;
};

bool SortTileInTU::operator()(const LHCb::MuonTileID& one,
                                     const LHCb::MuonTileID& two )const{
  if(one.nY()<two.nY())return true;
  if(one.nY()>two.nY())return false;
  if(one.nX()<=two.nX())return true;
  

  return false;
}



#endif // LIB_SORTTILEINTU_H

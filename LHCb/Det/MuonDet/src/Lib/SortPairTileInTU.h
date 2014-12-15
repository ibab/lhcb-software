// $Id: SortPairTileInTU.h,v 1.1 2008-06-30 11:45:30 asatta Exp $
#ifndef LIB_SORTPAIRTILEINTU_H 
#define LIB_SORTPAIRTILEINTU_H 1
#include <iostream>
#include <functional>

#include "Kernel/MuonTileID.h"
// Include files

/** @class SortPairTileInTU SortPairTileInTU.h Lib/SortPairTileInTU.h
 *  
 *
 *  @author 
 *  @date   2008-02-24
 */
class SortPairTileInTU :
  public 
std::binary_function<std::pair<LHCb::MuonTileID,LHCb::MuonTileID> 
,std::pair<LHCb::MuonTileID,LHCb::MuonTileID>,bool>{ public :
  bool operator()(  const std::pair<LHCb::MuonTileID,LHCb::MuonTileID>& 
first, const std::pair<LHCb::MuonTileID,LHCb::MuonTileID> & second )
    const;
};

bool SortPairTileInTU::operator()(const 
std::pair<LHCb::MuonTileID,LHCb::MuonTileID> & one,
                                     const 
std::pair<LHCb::MuonTileID,LHCb::MuonTileID> & two )const{
  if(one.first.nY()<two.first.nY())return true;
  if(one.first.nY()>two.first.nY())return false;
  if(one.first.nX()<=two.first.nX())return true;
  return false;
}



#endif // LIB_SORTPAIRTILEINTU_H

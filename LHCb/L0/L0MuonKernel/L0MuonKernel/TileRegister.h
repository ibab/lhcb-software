// $Id: TileRegister.h,v 1.3 2004-07-12 16:09:00 ltocco Exp $

#ifndef L0MUONKERNEL_TILEREGISTER_H
#define L0MUONKERNEL_TILEREGISTER_H     1

/** @class TILEREGISTER TILEREGISTER L0MuonKernel/TILEREGISTER.h

   Class representing a L0Muon Trigger register which has
   bits corresponding to MuonTileIDs.
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 


#include <vector>
#include "L0mConf/L0MPuNodeBase.h"
#include "L0mConf/L0MTile.h"
#include "L0mConf/L0MBase.h"
#include "L0MuonKernel/Register.h"
#include "MuonKernel/MuonTileID.h"
#include "MuonKernel/MuonSystemLayout.h"

namespace L0Muon {

enum RegisterType { STRIP, PAD } ;

class TileRegister : public L0Muon::Register {

public:
  TileRegister();
  TileRegister(int bits);
  TileRegister(int bits, unsigned long pattern);
  TileRegister(int bits, unsigned long pattern, std::vector<MuonTileID> ids);
  ~TileRegister();
  
  void setTileVector(const std::vector<MuonTileID>& ids);
  std::vector<MuonTileID> getTileVector() { return m_ids; }
  MuonTileID tile(int ind) { return m_ids[ind]; }
  std::vector<MuonTileID> firedTiles();
  RegisterType type() { return m_type; }

  void setType( std::string str){ m_fieldtype = str ;}
  void setTypeMT (int sta) { m_type = PAD ;}
  
  std::string Type() { return m_fieldtype;}

  void setType(int sta, int nreg);

  void setTypePad (int sta) {  m_type = PAD ; }
  void setTypeStrip (int sta){ m_type = STRIP ;
  }
  
  

  void setTile(MuonTileID & id);
  void setTile(MuonTileID & id, MsgStream * log);
  

  std::vector<MuonTileID> Pads() { return m_pads; }
  boost::dynamic_bitset<> TilesTag() { return m_tilestag ;}
  boost::dynamic_bitset<> StripsTag() { return m_stripstag ;}
  
  void setTag(std::vector<L0MTile> & tiles);
  
  void makePads();
  
    
  
    

private:

  std::vector<MuonTileID> m_ids ;  // MuonTileIDs mapping
  std::vector<MuonTileID> m_pads;
  RegisterType m_type;
  std::string m_fieldtype ;
  boost::dynamic_bitset<> m_tilestag ;
  boost::dynamic_bitset<> m_stripstag ;

};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_TILEREGISTER_H  

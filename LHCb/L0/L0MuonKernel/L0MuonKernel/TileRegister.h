// $Id: TileRegister.h,v 1.5 2004-12-21 14:33:03 ltocco Exp $

#ifndef L0MUONKERNEL_TILEREGISTER_H
#define L0MUONKERNEL_TILEREGISTER_H     1

/** @class TileRegister TileRegister L0MuonKernel/TileRegister.h

   Class representing a register where each 
   bit corresponds to a MuonTileID.
   
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


class TileRegister : public L0Muon::Register {

public:

  /// Constructor 
  TileRegister();

  /** Constructor 

      @param bits : number of bits
  */
  TileRegister(int bits);

  /** Constructor 

      @param bits    : number of bits
      @param pattern : bit pattern
  */
  TileRegister(int bits, unsigned long pattern);

  /** Constructor 

      @param bits    : number of bits
      @param pattern : bit pattern
      @param ids     : associated vector of MuonTileID
  */  
  TileRegister(int bits, unsigned long pattern, std::vector<MuonTileID> ids);

  /// Destructor
  ~TileRegister();
  
  /// set the vector of MuonTileIDs
  void setTileVector(const std::vector<MuonTileID>& ids);

  /// Return the vector of MuonTileIDs
  std::vector<MuonTileID> getTileVector() { return m_ids; }

  /** Return the MuonTileID

      @param ind : position in the vector
   */
  MuonTileID tile(int ind) { return m_ids[ind]; }

  /// Return the vector of MuonTileIDs for bits set to 1      
  std::vector<MuonTileID> firedTiles();

  /// Set the type of the register (optical link, neigh input/output)
  void setType( std::string str){ m_fieldtype = str ;}

  /// Return the type of the register  
  std::string Type() { return m_fieldtype;}

  /// Set the tile id
  void setTile(MuonTileID & id);
  
  /// Set the tile id
  void setTile(MuonTileID & id, MsgStream & log);
  
  /// Return the type of tiles
  boost::dynamic_bitset<> TilesTag() { return m_tilestag ;}

  /// Return the pads
  std::vector<MuonTileID> Pads() { return m_pads; }

  /// Return the strips
  boost::dynamic_bitset<> StripsTag() { return m_stripstag ;}
  
  /// set the type of tiles (strips/pad)
  void setTag(std::vector<L0MTile> & tiles);
  
  /// Cross the strips
  void makePads();
  
    
  
    

private:

  std::vector<MuonTileID> m_ids ;  // MuonTileIDs mapping
  std::vector<MuonTileID> m_pads;

  std::string m_fieldtype ;
  boost::dynamic_bitset<> m_tilestag ;
  boost::dynamic_bitset<> m_stripstag ;

};

};  // namespace L0Muon

#endif      // L0MUONKERNEL_TILEREGISTER_H  

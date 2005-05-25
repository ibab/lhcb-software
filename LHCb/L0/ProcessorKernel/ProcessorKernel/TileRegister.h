// $Id: TileRegister.h,v 1.5 2005-05-25 13:15:44 jucogan Exp $

#ifndef PROCESSORKERNEL_TILEREGISTER_H
#define PROCESSORKERNEL_TILEREGISTER_H     1

/** @class TileRegister TileRegister.h ProcessorKernel/TileRegister.h

   Class representing a register where each 
   bit corresponds to a MuonTileID.
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 


#include <vector>
#include "ProcessorKernel/Register.h"
#include "MuonKernel/MuonTileID.h"
#include <stdio.h>

//#include "MuonKernel/MuonSystemLayout.h"

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
  
  /// set the type of tiles (strips/pad)
  void setTilesTagVector(boost::dynamic_bitset<> & tilestag);

  /// Return the type of tiles
  boost::dynamic_bitset<> TilesTag() { return m_tilestag ;}

  /// Return the pads
  std::vector<MuonTileID> Pads() { return m_pads; }

  /// Return the strips
  boost::dynamic_bitset<> StripsTag() { return m_stripstag ;}
  
  /// set the type of strips (H/V)
  void setStripsTagVector(boost::dynamic_bitset<> & stripstag);

  /// Cross the strips, fill m_pads
  void makePads(int flag=0);
  
  /// Print the tile register for debug
  void print_bits(long unsigned int event, FILE *file);

  void print_words(FILE *file, int nwords_per_line=1);

  void print_tiles(FILE *file, int ntiles_per_line=4);
  
private:

  std::vector<MuonTileID> m_ids ;  // MuonTileIDs mapping
  std::vector<MuonTileID> m_pads;

  std::string m_fieldtype ;
  boost::dynamic_bitset<> m_tilestag ;
  boost::dynamic_bitset<> m_stripstag ;

};

};  // namespace L0Muon

#endif      // PROCESSORKERNEL_TILEREGISTER_H  

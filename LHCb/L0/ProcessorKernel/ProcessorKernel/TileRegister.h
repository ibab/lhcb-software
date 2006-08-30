// $Id: TileRegister.h,v 1.11 2006-08-30 06:42:04 cattanem Exp $

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
#include "Kernel/MuonTileID.h"
#include <stdio.h>

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
  TileRegister(int bits, unsigned long pattern, std::vector<LHCb::MuonTileID> ids);

  /// Destructor
  ~TileRegister();
  
  /// set the vector of MuonTileIDs
  void setTileVector(const std::vector<LHCb::MuonTileID>& ids);

  /// Return the vector of MuonTileIDs
  std::vector<LHCb::MuonTileID> getTileVector() { return m_ids; }

  /** Return the MuonTileID

      @param ind : position in the vector
   */
  LHCb::MuonTileID tile(int ind) { return m_ids[ind]; }

  /// Return the vector of MuonTileIDs for bits set to 1      
  std::vector<LHCb::MuonTileID> firedTiles();

  /// Set the tile id
  void setTile(LHCb::MuonTileID & id);
  
  /// set the type of tiles (strips/pad)
  void setTilesTagVector(boost::dynamic_bitset<> & tilestag);

  /// Return the type of tiles
  boost::dynamic_bitset<> TilesTag() { return m_tilestag ;}

  /// Return the pads
  std::vector<LHCb::MuonTileID> Pads() { return m_pads; }

  /// Return the strips
  boost::dynamic_bitset<> StripsTag() { return m_stripstag ;}
  
  /// set the type of strips (H/V)
  void setStripsTagVector(boost::dynamic_bitset<> & stripstag);

  /// Cross the strips, fill m_pads
  void makePads();
  
  /// Print the tile register for debug
  void print_bits(unsigned int event, FILE *file);

  void print_tiles(FILE *file, int ntiles_per_line=4);
  
  std::string toXML(std::string tab="");

private:

  std::vector<LHCb::MuonTileID> m_ids ;  // MuonTileIDs mapping
  std::vector<LHCb::MuonTileID> m_pads;

  boost::dynamic_bitset<> m_tilestag ;
  boost::dynamic_bitset<> m_stripstag ;

};

};  // namespace L0Muon

#endif      // PROCESSORKERNEL_TILEREGISTER_H  

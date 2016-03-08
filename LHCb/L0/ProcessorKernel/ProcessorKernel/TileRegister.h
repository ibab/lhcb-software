#ifndef PROCESSORKERNEL_TILEREGISTER_H
#define PROCESSORKERNEL_TILEREGISTER_H     1



#include <vector>
#include "ProcessorKernel/Register.h"
#include "Kernel/MuonTileID.h"
#include <stdio.h>


namespace L0Muon {


  /** @class TileRegister TileRegister.h ProcessorKernel/TileRegister.h

  Class representing a register where each
  bit corresponds to a MuonTileID.

  @author  Andrei Tsaregorodtsev, Julien Cogan
  @date  12 January 2002
  */
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

    /// set the duplicatedTiles flag
    void setDuplicatedTiles(bool flag=true)
    {
      m_duplicatedTiles=flag;
    }

    /// check if there are duplicated tiles in the tile list
    bool checkDuplicatedTiles();

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

    /** Set the tile id

        Uses the #m_duplicatedTiles to speed up the code.
    */
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

    /** Print out in a file the list of MuonTileIDs in the TileRegister

      @param file : pointer to the ouput file
      @param ntiles_per_line : nb of tiles per line (default is 4)
    */
    void print_tiles(FILE *file, int ntiles_per_line=4);

    /** Returns an xml representation of the TileRegister.
        (used to build the L0MuonKernel.xml configuration file
         for L0Muon emulation and decoding)
    */
    std::string toXML(std::string tab="");

  private:

    /** Flag indicating whether the TileRegister contains twice or more the same tile.

        This information is used to speed up the setTile method :
        in case the flag is set to false, one exits the loop over the tile as soon as
        the given tile is found; otherwise, it continues to see if the given tile
        is there more than once.
    */
    bool m_duplicatedTiles;

    std::vector<LHCb::MuonTileID> m_ids ;  /// MuonTileIDs mapping
    std::vector<LHCb::MuonTileID> m_pads;  /// Stores the list of fired pads (crossing H and V fired strips)

    boost::dynamic_bitset<> m_tilestag ;   /// Indicates whether a tile is a strip or a pad
    boost::dynamic_bitset<> m_stripstag ;  /// Indicates whether a strip is Horizontal or Vertical

  };

}  // namespace L0Muon

#endif      // PROCESSORKERNEL_TILEREGISTER_H

#ifndef KERNEL_IMUONLAYOUT_H
#define KERNEL_IMUONLAYOUT_H     1

/** @class IMuonLayout IMuonLayout.h Kernel/IMuonLayout.h

   Common interface for Muon System layout description classes

   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/

#include <vector>

#include "Kernel/MuonBase.h"

// Forward declarations
namespace LHCb {
  class MuonTileID;
}

class IMuonLayout {

public:

  /// destructor
  virtual ~IMuonLayout () { }

  /** Accessor to the layout grid values for a given MuonTileID
      @param   id   MuonTileID of the tile for which the grid is requested
      @return   std::pair<int,int> for the two grid values in X and in Y
  */
  virtual std::pair<unsigned int,unsigned int>
          grid(const LHCb::MuonTileID& id) const = 0 ;

  /** find a vector of MuonTileID's defined in terms of this MuonLayout
      which are touched by an area covered by a given MuonTileID

      @param pad   :  tile defining the area covered
  */

  virtual std::vector<LHCb::MuonTileID>
          tiles(const LHCb::MuonTileID& pad) const = 0;

  /** find a vector of MuonTileID's defined in terms of this MuonLayout
      which are touched by an area around a given MuonTileID defined
      in its own MuonLayout within given limits in X and in Y in units
      of the given MuonTileID.

      @param pad   : central for the search
      @param areaX : limits of the search area in X in units of central pad
      @param areaY : limits of the search area in Y in units of central pad
  */
  virtual std::vector<LHCb::MuonTileID>
          tilesInArea( const LHCb::MuonTileID& pad, int areaX,
                                                    int areaY ) const = 0;

  /// Generate all the MuonTileID's in the layout
  virtual std::vector<LHCb::MuonTileID> tiles() const = 0;

  /** Generate all the MuonTileID's in the layout in the given quarter
      @param iq   : quarter number
  */
  virtual std::vector<LHCb::MuonTileID> tiles(int iq) const = 0;

  /** Generate all the MuonTileID's in the layout in the given quarter
      and region
      @param iq   : quarter number
      @param ir   : region number
  */
  virtual std::vector<LHCb::MuonTileID> tiles(int iq, int ir) const = 0;

  /// find a tile containing the argument tile
  virtual LHCb::MuonTileID contains(const LHCb::MuonTileID& pad) const = 0;

  /// Get tiles touched by pad defined in terms of pregion region number
  virtual std::vector<LHCb::MuonTileID>
          tilesInRegion(const LHCb::MuonTileID& pad, int pregion) const =0;

  /// find all the MuonTileID's which are neighbours of the argument tile
  virtual std::vector<LHCb::MuonTileID>
          neighbours(const LHCb::MuonTileID& pad) const = 0;

  /** find all the MuonTileID's which are neighbours of the argument tile
      in the specified direction.
      @param   pad   find the neighbours of this tile
      @param   dirX  horizontal direction in which to look for neighbours
      @param   dirY  vertical direction in which to look for neighbours
  */
  virtual std::vector<LHCb::MuonTileID>
          neighbours(const LHCb::MuonTileID& pad, int dirX, int dirY ) const=0;

  /** find all the MuonTileID's which are neighbours of the argument tile
      in the specified direction.
      @param   pad   find the neighbours of this tile
      @param   dirX  horizontal direction in which to look for neighbours
      @param   dirY  vertical direction in which to look for neighbours
      @param   depth depth of the band in which to look for neighbours
  */
  virtual std::vector<LHCb::MuonTileID>
          neighbours(const LHCb::MuonTileID& pad, int dirX, int dirY,
                                                            int depth ) const=0;

  /** find all the MuonTileID's which are neighbours of the argument tile
      in the specified direction. This flavour of the neighbours function
      is only needed when looking for neighbours on the corners, since it
      takes two parameters for the depth of the search.
      @param   pad   find the neighbours of this tile
      @param   dirX  horizontal direction in which to look for neighbours
      @param   dirY  vertical direction in which to look for neighbours
      @param   depthX  depth of the band in X in which to look for neighbours
      @param   depthY  depth of the band in Y in which to look for neighbours
  */
  virtual std::vector<LHCb::MuonTileID>
          neighboursInArea(const LHCb::MuonTileID& pad, int dirX, int dirY,
                           int depthX, int depthY) const=0;

  /// check if the given MuonTileID is valid in the context of this layout
  virtual bool isValidID(const LHCb::MuonTileID& mt) const =0;

};

#endif      // KERNEL_IMUONLAYOUT_H

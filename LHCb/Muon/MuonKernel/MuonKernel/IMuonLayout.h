// $Id: IMuonLayout.h,v 1.4 2002-02-28 15:39:31 atsareg Exp $

#ifndef MUONKERNEL_IMUONLAYOUT_H
#define MUONKERNEL_IMUONLAYOUT_H     1

/** @class IMuonLayout IMuonLayout MuonKernel/IMuonLayout.h

   Interface for Muon System layout description classes
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 

#include <vector>

#include "MuonKernel/MuonBase.h"

// Forward declarations
class MuonTileID;
class IMuonTile;

class IMuonLayout {

public:       
    
    /** Accessor to the layout grid values for a given MuonTileID
       @param   id   MuonTileID of the tile for which the grid is requested
       @return   std::pair<int,int> for the two grid values in X and in Y
    */
    virtual std::pair<unsigned int,unsigned int> 
              grid(const MuonTileID& id) const = 0 ;
	      
    /** find a vector of MuonTileID's defined in terms of this MuonLayout
	which are touched by an area covered by a given MuonTileID 

	@param pad   :  tile defining the area covered
    */          

    virtual std::vector<MuonTileID> tiles(const MuonTileID& pad) const = 0; 	      
	      
    /** find a vector of MuonTileID's defined in terms of this MuonLayout
      which are touched by an area around a given MuonTileID defined 
      in its own MuonLayout within given limits in X and in Y in units
      of the given MuonTileID.
      
      @param pad   : central for the search
      @param areaX : limits of the search area in X in units of central pad
      @param areaY : limits of the search area in Y in units of central pad
    */ 
    virtual std::vector<MuonTileID> tilesInArea(const MuonTileID& pad, 
			                	  int areaX,
			                	  int areaY) const = 0; 		 
				   	  
    /// Generate all the MuonTileID's in the layout
    virtual std::vector<MuonTileID> tiles() const = 0; 	
    
    /// Generate all the MuonTileID's in the layout in the given quarter
    virtual std::vector<MuonTileID> tiles(int iq) const = 0; 	
    
    /// Generate all the MuonTileID's in the layout in the given quarter
    /// and region
    virtual std::vector<MuonTileID> tiles(int iq, int ir) const = 0; 					  

    /// find a tile containing the argument tile
    virtual MuonTileID contains(const MuonTileID& pad) const = 0;

    /// Get tiles touched by pad defined in terms of pregion region number
    virtual std::vector<MuonTileID> 
	 tilesInRegion(const MuonTileID& pad, int pregion) const =0;
	 
    /// find all the MuonTileID's which are neighbours of the argument tile
    virtual std::vector<MuonTileID> neighbours(const MuonTileID& pad) const = 0;
    
    /** find all the MuonTileID's which are neighbours of the argument tile
        in the specified direction. 
	@param   pad   find the neighbours of this tile
	@param   dirX  horizontal direction in which to look for neighbours
	@param   dirY  vertical direction in which to look for neighbours
    */	
    virtual std::vector<MuonTileID> neighbours(const MuonTileID& pad,
                                               int dirX, 
					       int dirY ) const=0;	
    
    /** find all the MuonTileID's which are neighbours of the argument tile
        in the specified direction. 
	@param   pad   find the neighbours of this tile
	@param   dirX  horizontal direction in which to look for neighbours
	@param   dirY  vertical direction in which to look for neighbours
	@param   depth depth of the band in which to look for neighbours
    */	
    virtual std::vector<MuonTileID> neighbours(const MuonTileID& pad,
                                               int dirX, 
					       int dirY,
					       int depth ) const=0;	 

    /// check if the given MuonTileID is valid for this layout
    virtual bool isValidID(const MuonTileID& mt) const =0;

};

#endif      // MUONKERNEL_IMUONLAYOUT_H  

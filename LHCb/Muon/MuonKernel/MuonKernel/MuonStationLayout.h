// $Id: MuonStationLayout.h,v 1.9 2005-12-16 15:57:59 asatta Exp $

#ifndef MUONKERNEL_MUONSTATIONLAYOUT_H
#define MUONKERNEL_MUONSTATIONLAYOUT_H 1   

// Include files
#include <vector>

/** @class MuonStationLayout MuonStationLayout.h MuonKernel/MuonStationLayout.h 
   
   Defines a Muon station (region) single plane logical layout
   together with various manipulation functions
   
   @author A.Tsaregorodtsev
   @date 6 April 2001 
*/

#include "Kernel/MuonLayout.h"
#include "Kernel/IMuonLayout.h"
#include "Kernel/MuonTileID.h"

class MuonStationLayout : public IMuonLayout {

public:
  /// Default constructor
  MuonStationLayout();
  /// Constructor taking MuonLayout's for each region
  MuonStationLayout(const MuonLayout& lq1,
                    const MuonLayout& lq2,
		    const MuonLayout& lq3,
                    const MuonLayout& lq4);
  /// Constructor taking one MuonLayout for all regions
  MuonStationLayout(const MuonLayout& lq);		    
  
  /// Destructor
  virtual ~MuonStationLayout();   
    
  /// Accessor to region layout
  MuonLayout regionLayout(int reg) const { return m_layouts[reg] ; }
  
  /// Accessor to LHCb::MuonLayout grid corresponding to the given LHCb::MuonTileID
  /// reimplemented from MuonLayout
  virtual std::pair<unsigned int, unsigned int> grid(const  LHCb::MuonTileID& id) const { 
    int reg = id.region();
    return std::make_pair(xGrid(reg),yGrid(reg)); 
  }
  /// Accessor to X granularity
  unsigned int xGrid(int reg) const { return regionLayout(reg).xGrid(); }
  /// Accessor to Y granularity
  unsigned int yGrid(int reg) const { return regionLayout(reg).yGrid(); }  
  
  /** find a vector of LHCb::MuonTileID's defined in terms of this MuonLayout
      which are touched by an area around a given LHCb::MuonTileID defined 
      in its own MuonLayout 
      
      @param pad   :  tile defining the area covered
  */          

  virtual std::vector<LHCb::MuonTileID> tiles(const LHCb::MuonTileID& pad) const;  
  
  /** find a vector of LHCb::MuonTileID's defined in terms of this MuonStationLayout
      which are touched by an area around a given LHCb::MuonTileID defined 
      in its own MuonStationLayout 
      
      @param pad   : central for the search
      @param areaX : limits of the search area in X
      @param areaY : limits of the search area in Y
  */          

  virtual std::vector<LHCb::MuonTileID> tilesInArea(const LHCb::MuonTileID& pad, 
			        	        int areaX,
			        	        int areaY) const;
				    
  /// returns a vector of its LHCb::MuonTileID's. 
  /// Implementation of the LHCb::ILHCb::MuonLayout	interface
  virtual std::vector<LHCb::MuonTileID> tiles() const;
  
  /// returns a vector of its LHCb::MuonTileID's in a given quarter. 
  /// Implementation of the LHCb::ILHCb::MuonLayout	interface
  virtual std::vector<LHCb::MuonTileID> tiles(int quarter) const;
  
  /// returns a vector of its LHCb::MuonTileID's in a given quarter and region. 
  /// Implementation of the LHCb::ILHCb::MuonLayout	interface
  virtual std::vector<LHCb::MuonTileID> tiles(int quarter, int region) const;
  	    
  /// find a tile containing the argument tile
  virtual LHCb::MuonTileID contains(const LHCb::MuonTileID& pad) const;
  
  /// Get tiles touched by pad defined in terms of pregion region number
  virtual std::vector<LHCb::MuonTileID> 
  tilesInRegion(const LHCb::MuonTileID& pad, int pregion) const;
  
  /// find all the LHCb::MuonTileID's which are neighbours of the argument tile
  virtual std::vector<LHCb::MuonTileID> neighbours(const LHCb::MuonTileID& pad) const;
  
  /** find all the LHCb::MuonTileID's which are neighbours of the argument tile
      in the specified direction. 
      @param   pad   find the neighbours of this tile
      @param   dirX  horizontal direction in which to look for neighbours
      @param   dirY  vertical direction in which to look for neighbours
  */	
  virtual std::vector<LHCb::MuonTileID> neighbours(const LHCb::MuonTileID& pad,
                                             int dirX,
					     int dirY) const;

  /** find all the LHCb::MuonTileID's which are neighbours of the argument tile
      in the specified direction. 
      @param   pad   find the neighbours of this tile
      @param   dirX  horizontal direction in which to look for neighbours
      @param   dirY  vertical direction in which to look for neighbours
      @param   depth  depth of the band in which to look for neighbours
  */	
  virtual std::vector<LHCb::MuonTileID> neighbours(const LHCb::MuonTileID& pad,
                                             int dirX,
					     int dirY,
					     int depth) const;
					     
  /** find all the LHCb::MuonTileID's which are neighbours of the argument tile
      in the specified direction. This flavour of the neighbours function
      is only needed when looking for neighbours on the corners, since it
      takes two parameters for the depth of the search.
      @param   pad   find the neighbours of this tile
      @param   dirX  horizontal direction in which to look for neighbours
      @param   dirY  vertical direction in which to look for neighbours
      @param   depthX  depth of the band in X in which to look for neighbours
      @param   depthY  depth of the band in Y in which to look for neighbours
  */	
  virtual std::vector<LHCb::MuonTileID> neighboursInArea(const LHCb::MuonTileID& pad,
                                        	   int dirX,
						   int dirY,
						   int depthX,
						   int depthY) const;					     
  
  /// check if the given LHCb::MuonTileID is valid for this layout
  virtual bool isValidID(const LHCb::MuonTileID& mt) const ;
  

private:  
  MuonLayout m_layouts[4];
    
};   

#endif    // MUONKERNEL_MUONSTATIONLAYOUT_H

// $Id: IMuonLayout.h,v 1.2 2002-01-31 10:13:24 atsareg Exp $

#ifndef MUONKERNEL_IMUONLAYOUT_H
#define MUONKERNEL_IMUONLAYOUT_H     1

/** @class IMuonLayout IMuonLayout MuonKernel/IMuonLayout.h

   Interface for Muon System layout description classes
   
   @author  Andrei Tsaregorodtsev
   @date  12 January 2002
*/ 

#include <vector>

// Forward declarations
class MuonSystemID;
class IMuonTile;

class IMuonLayout {

public:       
    
    /// Accessor to the grid for a given MuonSystemID
    virtual std::pair<unsigned int,unsigned int> 
              grid(const MuonSystemID& id) const = 0 ;
	      
    /** find a vector of MuonSystemID's defined in terms of this MuonLayout
	which are touched by an area around a given MuonSystemID defined 
	in its own MuonLayout 

	@param pad   :  tile defining the area covered
    */          

    virtual std::vector<MuonSystemID> tiles(const MuonSystemID& pad) const = 0; 	      
	      
    /** find a vector of MuonSystemID's defined in terms of this MuonLayout
      which are touched by an area around a given MuonSystemID defined 
      in its own MuonLayout 
      
      @param pad   : central for the search
      @param areaX : limits of the search area in X in units of central pad
      @param areaY : limits of the search area in Y in units of central pad
    */ 
    virtual std::vector<MuonSystemID> tilesInArea(const MuonSystemID& pad, 
			                	  int areaX,
			                	  int areaY) const = 0; 		 
				   	  
    /// Generate all the MuonSystemID's in the layout
    virtual std::vector<MuonSystemID> tiles() const = 0; 					  

    /// find a tile containing the argument tile
    virtual MuonSystemID contains(const MuonSystemID& pad) const = 0;

    /// Get tiles touched by pad defined in terms of pregion region number
    virtual std::vector<MuonSystemID> 
	 tilesInRegion(const MuonSystemID& pad, int pregion) const =0;

    /// check if the given MuonSystemID is valid for this layout
    virtual bool validID(const MuonSystemID& mt) const =0;

};

#endif      // MUONKERNEL_IMUONLAYOUT_H  

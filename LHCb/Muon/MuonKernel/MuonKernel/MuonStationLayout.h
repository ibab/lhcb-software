// $Id: MuonStationLayout.h,v 1.2 2002-01-31 10:13:24 atsareg Exp $

#ifndef MUONKERNEL_MUONSTATIONLAYOUT_H
#define MUONKERNEL_MUONSTATIONLAYOUT_H 1   

// Include files
#include <vector>
#include "GaudiKernel/MsgStream.h"  

/** @class MuonStationLayout MuonStationLayout.h MuonKernel/MuonStationLayout.h 
   
   Defines a Muon station (region) single plane logical layout
   together with various manipulation functions
   
   @author A.Tsaregorodtsev
   @date 6 April 2001 
*/

#include "MuonKernel/MuonLayout.h"
#include "MuonKernel/MuonSystemID.h"

// Forward declarations

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
  
  /// Accessor to MuonLayout grid corresponding to the given MuonSystemID
  /// reimplemented from IMuonLayout
  virtual std::pair<unsigned int, unsigned int> grid(const  MuonSystemID& id) const { 
    int reg = id.region();
    return std::make_pair(xGrid(reg),yGrid(reg)); 
  }
  /// Accessor to X granularity
  int xGrid(int reg) const { return regionLayout(reg).xGrid(); }
  /// Accessor to Y granularity
  int yGrid(int reg) const { return regionLayout(reg).yGrid(); }  
  
  /** find a vector of MuonSystemID's defined in terms of this MuonLayout
      which are touched by an area around a given MuonSystemID defined 
      in its own MuonLayout 
      
      @param pad   :  tile defining the area covered
  */          

  virtual std::vector<MuonSystemID> tiles(const MuonSystemID& pad) const;  
  
  /** find a vector of MuonSystemID's defined in terms of this MuonStationLayout
      which are touched by an area around a given MuonSystemID defined 
      in its own MuonStationLayout 
      
      @param pad   : central for the search
      @param areaX : limits of the search area in X
      @param areaY : limits of the search area in Y
  */          

  virtual std::vector<MuonSystemID> tilesInArea(const MuonSystemID& pad, 
			        	        int areaX,
			        	        int areaY) const;
				    
  /// returns a vector of its MuonSystemID's. 
  /// Implementation of the IMuonLayout	interface
  std::vector<MuonSystemID> tiles() const;
  	    
  /// find a tile containing the argument tile
  virtual MuonSystemID contains(const MuonSystemID& pad) const;
  
  /// Get tiles touched by pad defined in terms of pregion region number
  virtual std::vector<MuonSystemID> 
  tilesInRegion(const MuonSystemID& pad, int pregion) const;
  
  /// check if the given MuonSystemID is valid for this layout
  virtual bool validID(const MuonSystemID& mt) const ;
  

private:  
  MuonLayout m_layouts[4];
    
};   

#endif    // MUONKERNEL_MUONSTATIONLAYOUT_H

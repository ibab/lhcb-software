// $Id: MuonSystemLayout.h,v 1.1.1.1 2002-01-28 08:48:24 atsareg Exp $

#ifndef MUONKERNEL_MUONSYSTEMLAYOUT_H
#define MUONKERNEL_MUONSYSTEMLAYOUT_H 1   

// Include files
#include <vector>
#include "GaudiKernel/MsgStream.h"  

/** @class MuonSystemLayout MuonSystemLayout.h MuonKernel/MuonSystemLayout.h 
   
   Defines a Muon station (region) single plane logical layout
   together with various manipulation functions
   
   @author A.Tsaregorodtsev
   @date 6 April 2001 
*/

#include "MuonKernel/MuonStationLayout.h"

// Forward declarations

class MuonSystemID;
class IMuonTile;

class MuonSystemLayout : public IMuonLayout {

public:
  /// Default constructor
  MuonSystemLayout();
  /// Constructor taking MuonStationLayout's for each station
  MuonSystemLayout(const MuonStationLayout& lq1,
                   const MuonStationLayout& lq2,
		   const MuonStationLayout& lq3,
                   const MuonStationLayout& lq4,
		   const MuonStationLayout& lq5);
  /// Constructor taking one MuonStationLayout  for all stations
  MuonSystemLayout(const MuonStationLayout& lq);		   
  
  /// Destructor
  virtual ~MuonSystemLayout();
    
  /// Accessor to region/station MuonLayout  
  MuonLayout regionLayout(int st, int reg) const { 
    return  m_station_layouts[st].regionLayout(reg) ;
  }  
  /// Accessor to station MuonStationLayout  
  MuonStationLayout stationLayout(int st) const { 
    return  m_station_layouts[st] ;
  } 

  /// Accessor to MuonLayout grid corresponding to the given MuonSystemID
  /// reimplemented from IMuonLayout
  virtual std::pair<unsigned int, unsigned int> 
  grid(const  MuonSystemID& id) const { 
    int reg = id.region();
    int st  = id.station();
    return make_pair(xGrid(st,reg),yGrid(st,reg)); 
  } 
  /// Accessor to X granularity
  int xGrid(int st, int reg) const { return stationLayout(st).xGrid(reg); }
  /// Accessor to Y granularity
  int yGrid(int st, int reg) const { return stationLayout(st).yGrid(reg); }  
  
  /** find a vector of MuonSystemID's defined in terms of this MuonSystemLayout
      which are touched by an area around a given MuonSystemID defined 
      in its own MuonSystemLayout 
      
      @param pad   : central for the search
      @param areaX : limits of the search area in X
      @param areaY : limits of the search area in Y
  */          

  virtual std::vector<MuonSystemID> tiles(const MuonSystemID& pad, 
			        	  int areaX = 0 ,
			        	  int areaY = 0 ) const;    
				    
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
  MuonStationLayout m_station_layouts[5];
    
};   

#endif    // MUONKERNEL_MUONSYSTEMLAYOUT_H

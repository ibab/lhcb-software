// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/MuonLayout.h,v 1.5 2001-11-16 10:08:26 atsareg Exp $

#ifndef L0MUON_MUONLAYOUT_H
#define L0MUON_MUONLAYOUT_H 1   

// Include files
#include <vector>
#include "GaudiKernel/MsgStream.h"  

/** @class MuonLayout MuonLayout.h MuonLayout.h 
   
   Contains a Muon station logical layout
   
   @author A.Tsaregorodtsev
   @date 6 April 2001 
*/

// Forward declarations

class MuonTile;

class MuonLayout {

public:
  /// Constructors
  MuonLayout();
  /// Constructor taking X and Y granularities
  MuonLayout(int xgrid, int ygrid);
  
  /// Destructor
  ~MuonLayout();
    
  /// Accessor to X granularity
  int xGrid() const { return m_xgrid; }
  /// Accessor to Y granularity
  int yGrid() const { return m_ygrid; }  
  
  /** find a vector of its MuonTile's around the given MuonTile 
      defined in another MuonLayout within given limits
      
      @param pad : central for the search
      @param areaX : limits of the search area in X
      @param areaY : limits of the search area in Y
  */          
  std::vector<MuonTile> tilesInArea(const MuonTile& pad, 
			            int areaX,
			            int areaY );
  /** find a vector of its MuonTile's touched by a given MuonTile 
      defined in another MuonLayout 
      
      @param pad : central for the search
      @param areaX : limits of the search area in X
      @param areaY : limits of the search area in Y
  */	
  std::vector<MuonTile> tiles(const MuonTile& pad, 
			            int areaX = 0 ,
			            int areaY = 0 );	
  /// returns a vector of its MuonTile's	
  std::vector<MuonTile> tiles();			      
  /** returns a vector of its MuonTile's in a given quarter
      
      @param quarter : the quarter number to look into
  */	
  std::vector<MuonTile> tiles(int quarter);
  /** returns a vector of its MuonTile's in a given quarter and region
      
      @param quarter : the quarter number to look into
      @param region : the region number to look into
  */	
  std::vector<MuonTile> tiles(int quarter, int region);	      
  /// find a tile containing the argument tile
  MuonTile contains(const MuonTile& pad);
  /// Get tiles touched by pad defined in terms of pregion region number
  std::vector<MuonTile> tilesInRegion(const MuonTile& pad, int pregion);
  /// check if the given MuonTile is valid for this layout
  bool validTile(const MuonTile& mt);
  /// find magnification factor of pads in the given region
  int rfactor (int nr) const ;
  /// find region for the given pad indices
  int region (int bx, int by) const ;
  
private:

  int m_xgrid; 
  int m_ygrid;
    
};   

bool operator==(MuonLayout ml1, MuonLayout ml2);
MsgStream& operator<<(MsgStream& log, const MuonLayout& ml);

#endif    // L0MUON_MUONLAYOUT_H

// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Muon/L0Muon/MuonLayout.h,v 1.2 2001-06-07 16:46:19 atsareg Exp $

#ifndef L0MUON_MUONLAYOUT_H
#define L0MUON_MUONLAYOUT_H 1   

// Include files
#include <vector>
#include "GaudiKernel/SmartDataPtr.h"  
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
      defined in other MuonLayout within given limits
      
      @param pad : central for the search
      @param layout: MuonLayout in which the seed pad is defined
      @param areaX : limits of the search area in X
      @param areaY : limits of the search area in Y
  */          
  std::vector<MuonTile> tiles(const MuonTile& pad, 
			      int areaX = 0,
			      int areaY = 0);
  /// find a tile containing the argument tile
  MuonTile contains(const MuonTile& pad);
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

// $Header:

#ifndef L0MUON_MUONLAYOUT_H
#define L0MUON_MUONLAYOUT_H 1   

// Include files
#include <vector>
#include "GaudiKernel/SmartDataPtr.h"  

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
                              const MuonLayout& layout,
			      int areaX = 0,
			      int areaY = 0);

  /// Dump the contents to cout
  void print();
  /// find magnification factor of pads in the given region
  int rfactor (int nr) const ;
  /// find region for the given pad indices
  int region (int bx, int by) const ;
  
private:

  int m_xgrid; 
  int m_ygrid;
    
};   

#endif    // L0MUON_MUONLAYOUT_H

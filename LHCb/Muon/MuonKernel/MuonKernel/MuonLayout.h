// $Id: MuonLayout.h,v 1.1.1.1 2002-01-28 08:48:23 atsareg Exp $

#ifndef MUONKERNEL_MUONLAYOUT_H
#define MUONKERNEL_MUONLAYOUT_H 1   

// Include files
#include <iostream>
#include <vector>
#include "GaudiKernel/MsgStream.h"  

/** @class MuonLayout MuonLayout.h MuonKernel/MuonLayout.h 
   
   Defines a Muon station (region) single plane logical layout
   together with various manipulation functions
   
   @author A.Tsaregorodtsev
   @date 6 April 2001 
*/

#include "MuonKernel/IMuonLayout.h"

// Forward declarations

class MuonSystemID;
class IMuonTile;

class MuonLayout : public IMuonLayout {

public:
  /// Default constructor
  MuonLayout();
  /** Constructor taking X and Y granularities
    @param   xgrid  granularity in X
    @param   ygrid  granularity in Y
  */
  MuonLayout(unsigned int xgrid,unsigned  int ygrid);
  /** Constructor taking X and Y granularities as std::pair
    @param   xygrid  granularity in X
  */
  MuonLayout(std::pair<unsigned int, unsigned int> xygrid);
  
  /// Destructor
  virtual ~MuonLayout();
    
  /// Accessor to MuonLayout grid corresponding to the given MuonSystemID
  /// reimplemented from IMuonLayout
  virtual std::pair<unsigned int, unsigned int> 
  grid(const  MuonSystemID&) const { 
    return make_pair(xGrid(),yGrid()); 
  } 
    
  /// Accessor to X granularity
  int xGrid() const { return m_xgrid; }
  /// Accessor to Y granularity
  int yGrid() const { return m_ygrid; }  
  
  /** find a vector of MuonSystemID's defined in terms of this MuonLayout
      which are touched by an area around a given MuonSystemID defined 
      in its own MuonLayout 
      
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
  		      
  /** returns a vector of its MuonSystemID's in a given quarter
      
      @param quarter : the quarter number to look into
  */	
  std::vector<MuonSystemID> tiles(int quarter) const;
  
  /** returns a vector of its MuonSystemID's in a given quarter and region
      
      @param quarter : the quarter number to look into
      @param region : the region number to look into
  */	
  std::vector<MuonSystemID> tiles(int quarter, int region) const;	  
      
  /// find a tile containing the argument tile
  virtual MuonSystemID contains(const MuonSystemID& pad) const;
  
  /// Get tiles touched by pad defined in terms of pregion region number
  virtual std::vector<MuonSystemID> 
     tilesInRegion(const MuonSystemID& pad, int pregion) const;
  
  /// check if the given MuonSystemID is valid for this layout
  virtual bool validID(const MuonSystemID& mt) const;
  
  /// printout to std::ostream 
  inline std::ostream& printOut ( std::ostream& ) const ;

  /// printout to MsgStream
  inline MsgStream&    printOut ( MsgStream&    ) const ;

private:
  /// find magnification factor of pads in the given region
  int rfactor (int nr) const ;
  /// find region for the given pad indices
  int region (int bx, int by) const ;

private:  
  int m_xgrid; 
  int m_ygrid;
    
};   

// print to std::ostream
inline std::ostream& MuonLayout::printOut( std::ostream& os ) const { 
  os << "(" ;
  return 
    os <<  xGrid() << ","
       <<  yGrid() << ")" ;  
}

// print to MsgStream
inline MsgStream& MuonLayout::printOut( MsgStream& os ) const { 
  os << "(" ;
  return 
    os <<  xGrid() << ","
       <<  yGrid() << ")" ;  
}
// output to std::ostream 
inline std::ostream& operator<< ( std::ostream& os , const MuonLayout& id ) {
  return id.printOut( os );  
}

// output to MsgStream 
inline MsgStream& operator<<( MsgStream&    os , const MuonLayout& id ) {
  return id.printOut( os ); 
}
inline bool operator==(const MuonLayout& ml1, const MuonLayout& ml2) {
    return ml1.xGrid()==ml2.xGrid() &&
           ml1.yGrid()==ml2.yGrid();
}
#endif    // MUONKERNEL_MUONLAYOUT_H

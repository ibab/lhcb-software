// $Id: MuonLayout.h,v 1.9 2002-05-07 06:59:23 atsareg Exp $

#ifndef MUONKERNEL_MUONLAYOUT_H
#define MUONKERNEL_MUONLAYOUT_H 1   

// Include files
#include <iostream>
#include <vector>

#ifndef MUONKERNEL_LOCAL
#include "GaudiKernel/MsgStream.h"  
#endif  // MUONKERNEL_LOCAL

/** @class MuonLayout MuonLayout.h MuonKernel/MuonLayout.h 
   
   Defines a Muon station single plane logical layout. Layouts in
   all the regions of the station are the same with the scaling factor 2
   when passing from a region to a larger one. The class also implements 
   various layout/tiles manipulation functions.
   
   @author A.Tsaregorodtsev
   @date 6 April 2001 
*/

#include "MuonKernel/IMuonLayout.h"

// Forward declarations

class MuonTileID;

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
     
  /// Accessor to MuonLayout grid corresponding to the given MuonTileID
  /// reimplemented from IMuonLayout
  virtual std::pair<unsigned int, unsigned int> 
  grid(const  MuonTileID&) const { 
    return std::make_pair(xGrid(),yGrid()); 
  } 
    
  /// Accessor to X granularity
  unsigned int xGrid() const { return m_xgrid; }
  /// Accessor to Y granularity
  unsigned int yGrid() const { return m_ygrid; }  
  
  /** find a vector of MuonTileID's defined in terms of this MuonLayout
      which are touched by an area around a given MuonTileID defined 
      in its own MuonLayout 
      
      @param pad   :  tile defining the area covered
  */          

  virtual std::vector<MuonTileID> tiles(const MuonTileID& pad) const; 
  
  /** find a vector of MuonTileID's defined in terms of this MuonLayout
      which are touched by an area around a given MuonTileID defined 
      in its own MuonLayout 
      
      @param pad   : central for the search
      @param areaX : limits of the search area in X
      @param areaY : limits of the search area in Y
  */          

  virtual std::vector<MuonTileID> tilesInArea(const MuonTileID& pad, 
			        		int areaX,
			        		int areaY) const;    
				    					    				    
  /// returns a vector of its MuonTileID's. 
  /// Implementation of the IMuonLayout	interface
  virtual std::vector<MuonTileID> tiles() const;	
  		      
  /** returns a vector of its MuonTileID's in a given quarter
      
      @param quarter : the quarter number to look into
  */	
  virtual std::vector<MuonTileID> tiles(int quarter) const;
  
  /** returns a vector of its MuonTileID's in a given quarter and region
      
      @param quarter : the quarter number to look into
      @param region : the region number to look into
  */	
  virtual std::vector<MuonTileID> tiles(int quarter, int region) const;	  
      
  /// find a tile containing the argument tile
  virtual MuonTileID contains(const MuonTileID& pad) const;
  
  /// Get tiles touched by pad defined in terms of pregion region number
  virtual std::vector<MuonTileID> 
     tilesInRegion(const MuonTileID& pad, int pregion) const;
     
  /// find all the MuonTileID's which are neighbours of the argument tile
  virtual std::vector<MuonTileID> neighbours(const MuonTileID& pad) const;
  
  /** find all the MuonTileID's which are neighbours of the argument tile
      in the specified direction. 
      @param   pad   find the neighbours of this tile
      @param   dirX  horizontal direction in which to look for neighbours
      @param   dirY  vertical direction in which to look for neighbours
  */	
  virtual std::vector<MuonTileID> neighbours(const MuonTileID& pad,
                                             int dirX,
					     int dirY) const;	 

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
					     int depth) const;	 

  /** find all the MuonTileID's which are neighbours of the argument tile
      in the specified direction. This version takes into account that 
      area to look in can take two parameters to specify the depth of the 
      search in cases of neighbours at the corners, i.e. LEFT-UP.
      @param   pad   find the neighbours of this tile
      @param   dirX  horizontal direction in which to look for neighbours
      @param   dirY  vertical direction in which to look for neighbours
      @param   depthX depth of the band in which to look for neighbours
      @param   depthY depth of the band in which to look for neighbours
  */	
  virtual std::vector<MuonTileID> neighboursInArea(const MuonTileID& pad,
                                        	   int dirX,
						   int dirY,
						   int depthX,
						   int depthY) const;	
  
  /// check if the given MuonTileID is valid for this layout
  virtual bool isValidID(const MuonTileID& mt) const;
  
  /// check if the layout itself is defined
  bool isDefined() const;
  
  /// printout to std::ostream 
  inline std::ostream& printOut ( std::ostream& ) const ;

#ifndef MUONKERNEL_LOCAL
  /// printout to MsgStream
  inline MsgStream&    printOut ( MsgStream&    ) const ;
#endif  // MUONKERNEL_LOCAL

private:
  /// find magnification factor of pads in the given region
  int rfactor (unsigned int nr) const { return 1 << nr; }
  
  /** find region for the given pad indices. The pad indices
      are given in terms of the most inner region. If the pad indices
      are not fitting into the layout, the result returned is -1   
  */
  int region (unsigned int bx,unsigned int by) const ;

private:  
  unsigned int m_xgrid; 
  unsigned int m_ygrid;
    
};   

// print to std::ostream
inline std::ostream& MuonLayout::printOut( std::ostream& os ) const { 
  os << "(" ;
  return 
    os <<  xGrid() << ","
       <<  yGrid() << ")" ;  
}

#ifndef MUONKERNEL_LOCAL
// print to MsgStream
inline MsgStream& MuonLayout::printOut( MsgStream& os ) const { 
  os << "(" ;
  return 
    os <<  xGrid() << ","
       <<  yGrid() << ")" ;  
}

// output to MsgStream 
inline MsgStream& operator<<( MsgStream&  os , const MuonLayout& id ) {
  return id.printOut( os ); 
}
#endif  // MUONKERNEL_LOCAL

// output to std::ostream 
inline std::ostream& operator<< ( std::ostream& os , const MuonLayout& id ) {
  return id.printOut( os );  
}

inline bool operator==(const MuonLayout& ml1, const MuonLayout& ml2) {
    return ml1.xGrid()==ml2.xGrid() &&
           ml1.yGrid()==ml2.yGrid();
}
inline bool operator!=(const MuonLayout& ml1, const MuonLayout& ml2) {
    return !(ml1 == ml2);
}
#endif    // MUONKERNEL_MUONLAYOUT_H

// $Id: PixelLadder.h,v 1.3 2009-10-28 13:02:28 cocov Exp $
#ifndef VELOPIXDET_PIXELLADDER_H 
#define VELOPIXDET_PIXELLADDER_H 1

// Include files

/** @class PixelLadder PixelLadder.h VeloPixelDet/PixelLadder.h
 *  A class to simplify manipulation of Detector element parameters
 *
 *  @author Coco Victor
 *  @date   2009-07-13
 */

#include "GaudiKernel/Point3DTypes.h"
#include <vector>

class PixelLadder {
public: 
  /// Standard constructor
  PixelLadder(Gaudi::XYZPoint pRef ,int n , bool isHoriz  );
  virtual ~PixelLadder( ); ///< Destructor

  inline Gaudi::XYZPoint ReferencePoint() const {return m_pRef;}
  inline int nChip() const {return m_NChip;} 
  inline bool isHorizontal() const {return m_horizontalChip;} 
  int edgeOrientation(int chipNum) const { return m_edgesOrientation[chipNum];}
  void setReferencePoint( Gaudi::XYZPoint p);
  void setNChip( int n );
  void setIsHorizontal( bool isHoriz );
  void setEdgesOrientation( std::vector< int > orientation );

private:
  Gaudi::XYZPoint m_pRef;
  int m_NChip;
  bool m_horizontalChip;
  std::vector<int> m_edgesOrientation;
};


#endif // VELOPIXDET_PIXELLADDER_H

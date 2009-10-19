// $Id: PixelLadder.h,v 1.2 2009-10-19 07:32:11 cocov Exp $
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
  PixelLadder(Gaudi::XYZPoint pRef ,int n);
  virtual ~PixelLadder( ); ///< Destructor

  Gaudi::XYZPoint ReferencePoint() const;
  int nChip() const;
  std::vector<int> edgesOrientation() const;

  void setReferencePoint( Gaudi::XYZPoint p);
  void setNChip( int n );
  void setEdgesOrientation( std::vector< int > orientation );

private:
  Gaudi::XYZPoint m_pRef;
  int m_NChip;
  std::vector<int> m_edgesOrientation;
};


#endif // VELOPIXDET_PIXELLADDER_H

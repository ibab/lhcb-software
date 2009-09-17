// $Id: PixelLadder.h,v 1.1.1.1 2009-09-17 16:07:20 cocov Exp $
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

class PixelLadder {
public: 
  /// Standard constructor
  PixelLadder(Gaudi::XYZPoint pRef ,int n);
  virtual ~PixelLadder( ); ///< Destructor

  Gaudi::XYZPoint ReferencePoint() const;
  int nChip() const;

  void setReferencePoint( Gaudi::XYZPoint p);
  void setNChip( int n );

private:
  Gaudi::XYZPoint m_pRef;
  int m_NChip;
};


#endif // VELOPIXDET_PIXELLADDER_H

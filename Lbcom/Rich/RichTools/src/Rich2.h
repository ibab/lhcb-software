// $Id: Rich2.h,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
#ifndef RICHDETTOOLS_RICH2_H
#define RICHDETTOOLS_RICH2_H 1

// Include files
#include "RichX.h"

/** @class Rich2 Rich2.h RichDetTools/Rich2.h
 *
 *
 *  @author Artur Barczyk
 *  @date   29/05/2002
 */

class Rich2 : public RichX {

public:

  /// Standard constructor
  Rich2( );

  /// Constructor from detector service
  Rich2( IDataProviderSvc* );

  virtual ~Rich2( ); ///< Destructor

  /// Perform the geometrical reconstruction of a photon candidate from
  /// a RichTrackSegment and a detection point
  virtual int reconstructPhoton( const RichTrackSegment& segment,
                                 const HepPoint3D& point,
                                 RichGeomPhoton& photon );

  /// Returns which side (left/right) a given point falls in
  virtual int side( const HepPoint3D& point ) const;

};

#endif // RICHDETTOOLS_RICH2_H

// $Id: Rich1.h,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
#ifndef RICHDETTOOLS_RICH1_H
#define RICHDETTOOLS_RICH1_H 1

// Include files
#include "RichX.h"

/** @class Rich1 Rich1.h RichDetTools/Rich1.h
 *
 *
 *  @author Artur Barczyk
 *  @date   29/05/2002
 */

class IToolSvc;
class IRichRefractiveIndex;

class Rich1 : public RichX {

public:

  /// Standard constructor
  Rich1( );

  /// Constructor from detector service
  Rich1( IDataProviderSvc* );

  virtual ~Rich1(); ///< Destructor

  /// Perform the geometrical reconstruction of a photon candidate from
  /// a RichTrackSegment and a detection point
  virtual int reconstructPhoton( const RichTrackSegment& segment,
                                 const HepPoint3D& point,
                                 RichGeomPhoton& photon );

  /// Returns which side (up/down) a given point falls in
  inline virtual int side( const HepPoint3D& position ) const
  {
    return ( position.y() > 0.0 ? 0 : 1 );
  }

private:

  IRichRefractiveIndex * m_refIndex;
  IToolSvc * m_toolSvc;

};

#endif // RICHDETTOOLS_RICH1_H

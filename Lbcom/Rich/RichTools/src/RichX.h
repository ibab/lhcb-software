// $Id: RichX.h,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
#ifndef RICHDETTOOLS_RICHX_H
#define RICHDETTOOLS_RICHX_H 1

// local
#include "RichRadiator.h"
#include "RichReflector.h"
#include "RichDetector.h"

// RichEvent
#include "RichEvent/RichTrackSegment.h"
#include "RichEvent/RichGeomPhoton.h"

// LHCbKernel
#include "Kernel/RichSide.h"

/** @class RichX RichX.h RichDetTools/RichX.h
 *
 *  Abstract base class for Rich1 and Rich2
 *
 *  @author Artur Barczyk
 *  @date   18/05/2002
 */

class RichX {

public:

  typedef std::vector<RichX>                   Collection;
  typedef std::vector<RichX>::iterator         Iterator;
  typedef std::vector<RichX>::const_iterator   ConstIterator;
  typedef std::vector<RichX*>                  PointerCollection;
  typedef std::vector<RichX*>::iterator        PointerIterator;
  typedef std::vector<RichX*>::const_iterator  PointerConstIterator;

  /// Standard constructor
  RichX::RichX() : m_radiator   ( Rich::NRadiatorTypes, (RichRadiator*)NULL ),
                   m_detectorID ( Rich::Rich1 ),
                   m_statusOK   ( false       ) { }

  ///< Destructor
  virtual ~RichX() { cleanUp(); }
  
  /// Photon reconstruction. Implementation in concrete derived Rich classes
  virtual int reconstructPhoton( const RichTrackSegment& segment,
                                 const HepPoint3D& point,
                                 RichGeomPhoton& photon ) = 0;
  
  /// Returns pointer to radiator type.  Implementation in concrete derived Rich classes
  RichRadiator* radiator( const Rich::RadiatorType radiator );

  /// Returns pointer to reflector
  RichReflector* reflector( const Rich::Side side );

  /// Returns pointer to reflector
  RichDetector* detector( const Rich::Side side );

  /// General status flag. Check all is ok
  bool statusOK() const;

  /// Returns detector type
  Rich::DetectorType detectorID() const;

  /// Returns the detector "side"
  virtual int side( const HepPoint3D& point ) const = 0;

protected: // data

  // reflector and detector arrays are always of size 2, and are
  // indexed by side(): for Rich1 side() returns 0 = top      ( y>0 )
  //                                             1 = bottom   ( y<0 )
  //                    for Rich2 side() returns 0 = in-side  ( x>0 )
  //                                             1 = out-side ( x<0 )

  RichReflector::Collection m_reflector;       ///< Rich reflectors
  RichDetector::Collection m_detector;         ///< Rich detectors
  RichRadiator::PointerCollection m_radiator;  ///< Rich radiators
  Rich::DetectorType m_detectorID;             ///< Rich detector type

  /// General status flag
  bool m_statusOK;

private: // methods

  /// Perform housekeeping cleanup on data members
  void cleanUp();

};

inline RichRadiator* RichX::radiator( const Rich::RadiatorType id )
{
  return m_radiator[id];
}

inline RichReflector* RichX::reflector( const Rich::Side side )
{
  return ( static_cast<unsigned int>(side) < m_reflector.size() ? &m_reflector[side] : NULL );
}

inline RichDetector* RichX::detector( const Rich::Side side )
{
  return ( static_cast<unsigned int>(side) < m_detector.size() ? &m_detector[side] : NULL );
}

inline Rich::DetectorType RichX::detectorID() const
{
  return m_detectorID; 
};

inline bool RichX::statusOK() const 
{ 
  return m_statusOK; 
};

inline void RichX::cleanUp()
{
  for ( RichRadiator::PointerIterator i = m_radiator.begin();
        i != m_radiator.end(); ++i ) { delete *i; }
}

#endif // RICHDETTOOLS_RICHX_H

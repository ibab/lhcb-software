#ifndef TrackFitEvent_MeasurementFunctor_H
#define TrackFitEvent_MeasurementFunctor_H 1

// Include files
// -------------
#include <functional>
#include "Event/Measurement.h"
#include "Event/Track.h"

/** @namespace MeasurementFunctor
 *
 *  This namespace:
 * - contains functors (previously contained in associated classes)
 * - other small classes and things that do not fit (yet)
 *
 *  @author Eduardo Rodrigues (adaptations to new track event model)
 *  @date   2005-04-05
 *
 * @author Rutger van der Eijk
 *  created Thu Jun 20 14:47:34 2002
 *
 */

namespace MeasurementFunctor
{

//=============================================================================
// Class to test equality of Measurements
//=============================================================================
//  class equalTo: public std::unary_function<Measurement*, bool> {
//    private:
//      const Measurement& m_meas;
//    public:
//      explicit equalTo( const Measurement& meas ):m_meas(meas) {}
//      bool operator()( const Measurement* meas ) const 
//      {
//        return m_meas.equal( meas );
//      }
//  };

//=============================================================================
// Class to test equality of Measurement types
//=============================================================================
  class typeEqualTo: public std::unary_function<Measurement*, bool> {
    private:
      const Measurement::Type& m_measType;
    public:
      explicit typeEqualTo( const Measurement::Type& measType ):
        m_measType( measType ) {}
      bool operator()( const Measurement* meas ) const 
      {
        return meas -> checkType( m_measType );
//        return m_measType == meas -> type();
      }
  };

//=============================================================================
// Class to test if the z of a Measurement is < than a certain value
//=============================================================================
  class less_z: public std::unary_function<Measurement*, bool> {
    private:
      double m_z;
    public:
      explicit less_z( double z ):m_z(z) {}
      bool operator()( const Measurement* meas ) const 
      {
        return meas -> z() < m_z;
      }
  };

//=============================================================================
// Class to test if the z of a Measurement is > than a certain value
//=============================================================================
  class greater_z: public std::unary_function<Measurement*, bool> {
    private:
      double m_z;
    public:
      explicit greater_z( double z ):m_z(z) {}
      bool operator()( const Measurement* meas ) const 
      {
        return meas -> z() > m_z;
      }
  };

//=============================================================================
// Class for sorting Measurements by increasing z
//=============================================================================
  class increasingByZ {
    public:
      bool operator()( const Measurement* firstHit,
                       const Measurement* secondHit ) const
      {
        return (0==firstHit) ? true : (0==secondHit) ?
          false : firstHit->z() < secondHit->z();
      }
  };

//=============================================================================
// Class for sorting Measurements by decreasing z
//=============================================================================
  class decreasingByZ {
    public:
      bool operator()( const Measurement* firstHit,
                       const Measurement* secondHit ) const
      {
        return  (0==firstHit) ? true : (0==secondHit) ?
          false : firstHit->z() > secondHit->z();
      }
  };

//=============================================================================
// Predicate returning if Measurement is on a Track
//=============================================================================
//  // Temp direct use of concrete class Track because of time shortage
//  class hasOnTrack: public std::unary_function<Measurement*, bool> {
//    private:
//      const Track* m_track;
//    public:
//      explicit hasOnTrack( const Track* track ):m_track(track) {}

//      bool operator()( const Measurement* meas ) const {
//        return m_track -> hasOn( meas );
//      }
//  };

//=============================================================================
// Predicate returning if Measurement is on a Track
//=============================================================================
//  // Temp direct use of concrete class Track because of time shortage
//  class hasOnTrackOfType: public std::unary_function<Measurement*, bool> {
//    private:
//      const Track* m_track;
//      Measurement::Type m_measType;
//    public:
//      explicit hasOnTrackOfType( const Track* track,
//                                 Measurement::Type measType ):
//        m_track(track),
//        m_measType(measType) {}

//      bool operator()( const Measurement* meas ) const {
//        return (m_measType == meas->measType()) && m_track->hasOn(meas);
//      }
//  };

}

#endif   /// TrackFitEvent_MeasurementFunctor_H

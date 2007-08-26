// $Id: VeloSensorHits.h,v 1.4 2007-08-26 14:10:41 krinnert Exp $
#ifndef INCLUDE_TF_VELOSENSORHITS_H
#define INCLUDE_TF_VELOSENSORHITS_H 1

#include <vector>
#include <algorithm>

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>

#include <math.h>

#include "VeloDet/DeVeloRType.h"
#include "VeloDet/DeVeloPhiType.h"

#include "TfKernel/VeloRHit.h"
#include "TfKernel/VeloPhiHit.h"

namespace Tf {

  /** @class VeloSensorHits TfKernel/VeloSensorHits.h
   *  Provides measurements of either an R or a Phi sensor at a VELO station.
   *
   *  There will be an instance of this object for each sensor.  I is called
   *
   *  This class holds links to the measurement data for a sensor further
   *  segmented in zones.  Tha data itself is not owned by VeloSensorHits, but
   *  by some instance of a VeloHitManager.
   *
   * @author Kurt Rinnert <kurt.rinnert@cern.ch>
   * @date   2007-07-31
   *
   * @see VeloHitManager
   */
  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  class VeloSensorHits {

  public:

    typedef DATATYPE* value_type;
    typedef std::vector<value_type> container_type;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;
    typedef typename DATATYPE::range_type range_type;
    typedef typename DATATYPE::CompareIdeal compare_type;

  public:

    /**@struct ZLessThan
     * Compate station data by station z coordinate
     *
     * @author Kurt Rinnert <kurt.rinnert@cern.ch>
     * @date   2007-07-14
     */
    struct ZLessThan {
      inline bool operator() (const VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>* lhs, 
                              const VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>* rhs)
      {
        return lhs->z() < rhs->z();
      }
    };

  public:

    /// Construct with link to sensor
    VeloSensorHits(const SENSORTYPE* sensor);

    /// Check whether there is any data in a given zone
    bool empty(unsigned int zone) const { return m_data[zone].empty(); }

    /// Get the number of hits in a given zone
    size_t size(unsigned int zone) const { return m_data[zone].size(); }

    /** Access to the data.
     *  Non-const version, modification is explicitly allowed.
     *  No safety checks for performance reasons.
     */
    container_type& zone(unsigned int zone) { return m_data[zone]; }

    /// Get all hits in a zone
    typename DATATYPE::range_type hits(unsigned int zone) { return range_type(m_data[zone].begin(),m_data[zone].end()); }

    /// Get all hits in a zone inside an interval specified in global coordinates
    typename DATATYPE::range_type  hits(unsigned int zone, double min, double max) ;

    /// Get the hit closest to pos inside pos +/- tol, if any. Pos and tol refer to global coordinates.
    value_type closestHit(unsigned int zone, double pos, double tol) ;

    /// Get all hits in a zone inside an interval specified in half box coordinates
    typename DATATYPE::range_type  hitsHalfBox(unsigned int zone, double min, double max) ;

    /// Get the hit closest to pos inside pos +/- tol, if any. Pos and tol refer to half box coordinates.
    value_type closestHitHalfBox(unsigned int zone, double pos, double tol) ;

    /// Get all hits in a zone inside an interval specified in ideal coordinates
    typename DATATYPE::range_type  hitsIdeal(unsigned int zone, double min, double max) ;

    /// Get the hit closest to pos inside pos +/- tol, if any. Pos and tol refer to ideal coordinates.
    value_type closestHitIdeal(unsigned int zone, double pos, double tol) ;

    /// find the hit with a given LHCbID
    value_type hitByLHCbID(unsigned int zone, LHCb::LHCbID id);

    /// Access the sensor link
    const SENSORTYPE* sensor() const { return m_sensor; }

    /// Retrieve the station number
    unsigned int stationNumber() const { return m_sensor->station(); }

    /// Retrieve the z position
    double z() const { return m_sensor->z(); }

    /// Get the number of zones
    unsigned int nZones() const { return ZONES; }

    /// Get the VELO half index
    unsigned int veloHalf() const { return static_cast<unsigned int>(m_sensor->isRight()); }

    /// Sort the containers
    void sort();

    /// Clear the data containers
    void clear();

    /// strict weak ordering
    bool operator< (const VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>& rhs) const;

  private:

    /// Construction without providing a sensor makes no sense
    VeloSensorHits();

  private:

    const SENSORTYPE* m_sensor;

    container_type m_data[ZONES];
  };

  //----------------------------------------------------------------------
  // inline implementations
  //----------------------------------------------------------------------

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  inline void VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::sort()
  {
    for (unsigned int zone=0; zone<ZONES; ++zone) std::sort(m_data[zone].begin(),m_data[zone].end(),compare_type());
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  inline void VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::clear()
  {
    for (unsigned int zone=0; zone<ZONES; ++zone) m_data[zone].clear();
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  inline bool VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::operator< (const VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>& rhs) const
  {
    return m_sensor->z() < rhs.m_sensor->z();
  }

  //----------------------------------------------------------------------
  // implementations
  //----------------------------------------------------------------------

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::VeloSensorHits(const SENSORTYPE* sensor)
    : m_sensor(sensor)
  {;}

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  typename DATATYPE::range_type VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::hits(unsigned int zone, double min, double max)
  {
    // cache begin and end
    iterator begin = m_data[zone].begin();
    iterator end   = m_data[zone].end();

    // find the first hit
    iterator beginHit =
      std::lower_bound(begin, end, min,
                       boost::lambda::bind(&DATATYPE::coord,boost::lambda::_1) <  boost::lambda::_2);

    // find the last hit
    iterator endHit =
      std::lower_bound(beginHit, end, max,
                       boost::lambda::bind(&DATATYPE::coord,boost::lambda::_1) <  boost::lambda::_2);

    return range_type( beginHit, endHit ) ;
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  typename DATATYPE::range_type VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::hitsHalfBox(unsigned int zone, double min, double max)
  {
    // cache begin and end
    iterator begin = m_data[zone].begin();
    iterator end   = m_data[zone].end();

    // find the first hit
    iterator beginHit =
      std::lower_bound(begin, end, min,
                       boost::lambda::bind(&DATATYPE::coordHalfBox,boost::lambda::_1) <  boost::lambda::_2);

    // find the last hit
    iterator endHit =
      std::lower_bound(beginHit, end, max,
                       boost::lambda::bind(&DATATYPE::coordHalfBox,boost::lambda::_1) <  boost::lambda::_2);

    return range_type( beginHit, endHit ) ;
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  typename DATATYPE::range_type VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::hitsIdeal(unsigned int zone, double min, double max)
  {
    // cache begin and end
    iterator begin = m_data[zone].begin();
    iterator end   = m_data[zone].end();

    // find the first hit
    iterator beginHit =
      std::lower_bound(begin, end, min,
                       boost::lambda::bind(&DATATYPE::coordIdeal,boost::lambda::_1) <  boost::lambda::_2);

    // find the last hit
    iterator endHit =
      std::lower_bound(beginHit, end, max,
                       boost::lambda::bind(&DATATYPE::coordIdeal,boost::lambda::_1) <  boost::lambda::_2);

    return range_type( beginHit, endHit ) ;
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  DATATYPE*  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::closestHit(unsigned int zone, double pos, double tol)
  {
    // nothing to see here, please move along
    if ( m_data[zone].empty() ) return 0;

    DATATYPE* closestHit = 0;

    iterator it  = m_data[zone].begin();
    iterator end = m_data[zone].end();
    for ( ; it != end; ++it ) {
      double coord = (*it)->coord();
      if ( pos + tol < coord ) break;
      if ( pos - tol < coord ) {
        tol = fabs( pos - coord );
        closestHit = *it;
      }
    }

    return closestHit;
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  DATATYPE*  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::closestHitHalfBox(unsigned int zone, double pos, double tol)
  {
    // nothing to see here, please move along
    if ( m_data[zone].empty() ) return 0;


    DATATYPE* closestHit = 0;

    iterator it  = m_data[zone].begin();
    iterator end = m_data[zone].end();
    for ( ; it != end; ++it ) {
      double coord = (*it)->coordHalfBox();
      if ( pos + tol < coord ) break;
      if ( pos - tol < coord ) {
        tol = fabs( pos - coord );
        closestHit = *it;
      }
    }

    return closestHit;
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  DATATYPE*  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::closestHitIdeal(unsigned int zone, double pos, double tol)
  {
    // nothing to see here, please move along
    if ( m_data[zone].empty() ) return 0;

    DATATYPE* closestHit = 0;

    iterator it  = m_data[zone].begin();
    iterator end = m_data[zone].end();
    for ( ; it != end; ++it ) {
      double coord = (*it)->coordIdeal();
      if ( pos + tol < coord ) break;
      if ( pos - tol < coord ) {
        tol = fabs( pos - coord );
        closestHit = *it;
      }
    }

    return closestHit;
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  DATATYPE*  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::hitByLHCbID(unsigned int zone, const LHCb::LHCbID id) {

    DATATYPE* hit = 0;

    iterator it  = m_data[zone].begin();
    iterator end = m_data[zone].end();
    for ( ; it != end; ++it ) {
      if ( (*it)->lhcbID() == id ) {
        hit = *it;
        break;
      }
    }

    return hit;
  }

  typedef VeloSensorHits<DeVeloRType,VeloRHit,4>   VeloRSensorHits;   ///< short name for default r hit container
  typedef VeloSensorHits<DeVeloRType,VeloPhiHit,2> VeloPhiSensorHits; ///< short name for default phi hit container

} // namespace Tf
#endif // INCLUDE_TF_VELOSENSORHITS_H


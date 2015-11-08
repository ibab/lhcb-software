#ifndef INCLUDE_TF_VELOSENSORHITS_H
#define INCLUDE_TF_VELOSENSORHITS_H 1

#include <vector>
#include <algorithm>
#include <cmath>

#include "VeloDet/DeVeloRType.h"
#include "VeloDet/DeVeloPhiType.h"

#include "TfKernel/VeloRHit.h"
#include "TfKernel/VeloPhiHit.h"

namespace Tf {

    /**@struct ZLessThan
     * Compate station data by station z coordinate
     *
     * @author Kurt Rinnert <kurt.rinnert@cern.ch>
     * @date   2007-07-14
     */
  constexpr struct ZLessThan_t {
    template <typename T>
    inline bool operator() (const T* lhs, const T* rhs) { return lhs->z() < rhs->z(); }
  } ZLessThan {};

  namespace detail {

    constexpr struct transform_pos_t {
        double operator()(const DeVeloRType& , double pos) const { return pos; }
        double operator()(const DeVeloPhiType& sensor, double pos) const {
          return sensor.isRight() && pos<0 ? pos + 2*Gaudi::Units::pi : pos;
        }
    } transform_pos {};

    template <typename Container, typename Proj>
    typename std::remove_pointer<typename Container::value_type>::type::range_type
    range(Container c, double min, double max, Proj proj) {
        auto cmp = [&](typename Container::const_reference d, double m) { return (d->*proj)() < m; };
        auto first = std::begin(c);
        auto last  = std::end(c);
        first = std::lower_bound( first, last, min, cmp );
        last  = std::lower_bound( first, last, max, cmp );
        return { first, last };
    }

    template <typename Container, typename Projection>
    typename Container::value_type closestHit(Container c, double pos, double tol, Projection proj) {
        typename Container::value_type closest = nullptr;
        //auto first  = c.begin(); // @TODO: use lower_bound to skip ahead to pos-tol, as the code implicilty assumes weak ordered by projection
        //auto last = c.end();
        for (const auto& i : c ) {
          auto coord = (i->*proj)();
          if ( pos + tol < coord ) break;
          if ( pos - tol < coord ) {
            tol = fabs( pos - coord );
            closest = i;
          }
        }
        return closest;
    }
  }

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

    /// Construction without providing a sensor makes no sense
    VeloSensorHits() = delete;

    /// Construct with link to sensor
    explicit VeloSensorHits(const SENSORTYPE* sensor) : m_sensor(sensor) { }

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
    typename DATATYPE::range_type hits(unsigned int zone) {
      return range_type(m_data[zone].begin(),m_data[zone].end());
    }

    /// Get all hits in a zone inside an interval specified in global coordinates
    typename DATATYPE::range_type  hits(unsigned int zone, double min, double max) {
    return detail::range( m_data[zone],
                          detail::transform_pos(*m_sensor,min),
                          detail::transform_pos(*m_sensor,max),
                          &DATATYPE::sortCoord );
    }

    /// Get all hits in a zone inside an interval specified in half box coordinates
    typename DATATYPE::range_type  hitsHalfBox(unsigned int zone, double min, double max) {
    return detail::range( m_data[zone],
                          detail::transform_pos(*m_sensor, min),
                          detail::transform_pos(*m_sensor, max),
                          &DATATYPE::sortCoordHalfBox );
    }

    /// Get all hits in a zone inside an interval specified in ideal coordinates
    typename DATATYPE::range_type  hitsIdeal(unsigned int zone, double min, double max) {
      return detail::range( m_data[zone],
                            detail::transform_pos(*m_sensor, min),
                            detail::transform_pos(*m_sensor, max),
                            &DATATYPE::sortCoordIdeal );
    }

    /// Get the hit closest to pos inside pos +/- tol, if any. Pos and tol refer to global coordinates.
    value_type closestHit(unsigned int zone, double pos, double tol) {
      return detail::closestHit( m_data[zone],
                                 detail::transform_pos(*m_sensor,pos), tol,
                                 &DATATYPE::sortCoord );
    }

    /// Get the hit closest to pos inside pos +/- tol, if any. Pos and tol refer to half box coordinates.
    value_type closestHitHalfBox(unsigned int zone, double pos, double tol)  {
      return detail::closestHit(m_data[zone],
                                detail::transform_pos(*m_sensor, pos), tol,
                                &DATATYPE::sortCoordHalfBox );
    }

    /// Get the hit closest to pos inside pos +/- tol, if any. Pos and tol refer to ideal coordinates.
    value_type closestHitIdeal(unsigned int zone, double pos, double tol) {
      return detail::closestHit( m_data[zone],
                                 detail::transform_pos(*m_sensor,pos), tol,
                                 &DATATYPE::sortCoordIdeal );
    }

    /** Get the hit closest to pos inside pos +/- tol at reference radius, if any.
     * Pos and tol refer to global coordinates.
     * This only makes sense for phi type sensor hits.  For r type there is no
     * implementation.  Any attempt to call this for r type sensor hits will result
     * in a compile time error.
     */
    value_type closestHitAtR(unsigned int zone, double r, double pos, double tol) {
      static_assert( std::is_same<SENSORTYPE,DeVeloPhiType>::value, "SENSORTYPE must be DeVeloPhiType");
      return detail::closestHit(m_data[zone],
                                detail::transform_pos(*m_sensor,pos) - m_sensor->globalPhiOffset(zone,r) ,tol,
                                &DATATYPE::sortCoord );
    }

    /** Get the hit closest to pos inside pos +/- tol at reference radius, if any.
     * Pos and tol refer to half box coordinates.
     * This only makes sense for phi type sensor hits.  For r type there is no
     * implementation.  Any attempt to call this for r type sensor hits will result
     * in a compile time error.
     */
    value_type closestHitAtRHalfBox(unsigned int zone, double r, double pos, double tol) {
      static_assert( std::is_same<SENSORTYPE,DeVeloPhiType>::value, "SENSORTYPE must be DeVeloPhiType");
      return detail::closestHit(m_data[zone],
                                detail::transform_pos(*m_sensor,pos)-m_sensor->halfboxPhiOffset(zone,r),tol,
                                &DATATYPE::sortCoordHalfBox );
    }

    /** Get the hit closest to pos inside pos +/- tol at reference radius, if any.
     * Pos and tol refer to ideal coordinates.
     * This only makes sense for phi type sensor hits.  For r type there is no
     * implementation.  Any attempt to call this for r type sensor hits will result
     * in a compile time error.
     */
    value_type closestHitAtRIdeal(unsigned int zone, double r, double pos, double tol) {
      static_assert( std::is_same<SENSORTYPE,DeVeloPhiType>::value, "SENSORTYPE must be DeVeloPhiType");
      return detail::closestHit( m_data[zone],
                                 detail::transform_pos(*m_sensor,pos) - m_sensor->idealPhiOffset(zone,r), tol,
                                 &DATATYPE::sortCoordIdeal );
    }

    /// find the hit with a given LHCbID
    value_type hitByLHCbID(unsigned int zone, LHCb::LHCbID id) {
      auto i = std::find_if( m_data[zone].begin(), m_data[zone].end(),
                             [&](const value_type& j) { return j->lhcbID() == id; } );
      return i!=m_data[zone].end() ? *i : nullptr;
    }

    /// Access the sensor link
    const SENSORTYPE* sensor() const { return m_sensor; }

    /// Access the sensor number, wrapper to make client code more readable
    unsigned int sensorNumber() const { return m_sensor->sensorNumber(); }

    /// Retrieve the station number
    unsigned int stationNumber() const { return m_sensor->station(); }

    /// Retrieve the z position
    double z() const { return m_sensor->z(); }

    /// Get the number of zones
    unsigned int nZones() const { return ZONES; }

    /// Get the VELO half index
    unsigned int veloHalf() const { return static_cast<unsigned int>(m_sensor->isRight()); }

    /// Sort the containers
    void sort() {
      for( auto& data : m_data ) std::sort(data.begin(),data.end(),compare_type());
    }

    /// Clear the data containers
    void clear() {
      for( auto& data : m_data ) data.clear();
      m_hitsPrepared = false;
    }

    /// strict weak ordering
    bool operator< (const VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>& rhs) const {
      return m_sensor->z() < rhs.m_sensor->z();
    }

    /// check whether the hits in this station are prepared
    bool hitsPrepared() const { return m_hitsPrepared; }

    /// alter the hit preparation flag
    void setHitsPrepared(const bool flag = true) { m_hitsPrepared = flag; }

  private:
    const SENSORTYPE* m_sensor;
    container_type m_data[ZONES];
    bool m_hitsPrepared = false;
  };

  typedef VeloSensorHits<DeVeloRType,VeloRHit,4>   VeloRSensorHits;   ///< short name for default r hit container
  typedef VeloSensorHits<DeVeloRType,VeloPhiHit,2> VeloPhiSensorHits; ///< short name for default phi hit container

} // namespace Tf
#endif // INCLUDE_TF_VELOSENSORHITS_H

// $Id: VeloSensorHits.h,v 1.7 2008-01-20 15:39:43 krinnert Exp $
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
    typename DATATYPE::range_type hits(unsigned int zone) { 
      return range_type(m_data[zone].begin(),m_data[zone].end()); }

    /// Get all hits in a zone inside an interval specified in global coordinates
    typename DATATYPE::range_type  hits(unsigned int zone, double min, double max) { 
      return hitsWrap(zone,min,max,m_sensor); }

    /// Get all hits in a zone inside an interval specified in half box coordinates
    typename DATATYPE::range_type  hitsHalfBox(unsigned int zone, double min, double max) { 
      return hitsHalfBoxWrap(zone,min,max,m_sensor); }
    
    /// Get all hits in a zone inside an interval specified in ideal coordinates
    typename DATATYPE::range_type  hitsIdeal(unsigned int zone, double min, double max) { 
      return hitsIdealWrap(zone,min,max,m_sensor); }

    /// Get the hit closest to pos inside pos +/- tol, if any. Pos and tol refer to global coordinates.
    value_type closestHit(unsigned int zone, double pos, double tol) { 
      return closestHitWrap(zone,pos,tol,m_sensor); }

    /// Get the hit closest to pos inside pos +/- tol, if any. Pos and tol refer to half box coordinates.
    value_type closestHitHalfBox(unsigned int zone, double pos, double tol)  { 
      return closestHitHalfBoxWrap(zone,pos,tol,m_sensor); }

    /// Get the hit closest to pos inside pos +/- tol, if any. Pos and tol refer to ideal coordinates.
    value_type closestHitIdeal(unsigned int zone, double pos, double tol) { 
      return closestHitIdealWrap(zone,pos,tol,m_sensor); }

    /** Get the hit closest to pos inside pos +/- tol at reference radius, if any. 
     * Pos and tol refer to global coordinates.
     * This only makes sense for phi type sensor hits.  For r type there is no 
     * implementation.  Any attempt to call this for r type sensor hits will result
     * in a compile time error.
     */
    value_type closestHitAtR(unsigned int zone, double r, double pos, double tol) { 
      return closestHitAtRImp(zone,r,pos,tol,m_sensor); }

    /** Get the hit closest to pos inside pos +/- tol at reference radius, if any. 
     * Pos and tol refer to half box coordinates.
     * This only makes sense for phi type sensor hits.  For r type there is no 
     * implementation.  Any attempt to call this for r type sensor hits will result
     * in a compile time error.
     */
    value_type closestHitAtRHalfBox(unsigned int zone, double r, double pos, double tol) { 
      return closestHitAtRHalfBoxImp(zone,r,pos,tol,m_sensor); }

    /** Get the hit closest to pos inside pos +/- tol at reference radius, if any. 
     * Pos and tol refer to ideal coordinates.
     * This only makes sense for phi type sensor hits.  For r type there is no 
     * implementation.  Any attempt to call this for r type sensor hits will result
     * in a compile time error.
     */
    value_type closestHitAtRIdeal(unsigned int zone, double r, double pos, double tol) { 
      return closestHitAtRIdealImp(zone,r,pos,tol,m_sensor); }

    /// find the hit with a given LHCbID
    value_type hitByLHCbID(unsigned int zone, LHCb::LHCbID id);

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
    void sort();

    /// Clear the data containers
    void clear();

    /// strict weak ordering
    bool operator< (const VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>& rhs) const;

    /// check whether the hits in this station are prepared
    bool hitsPrepared() const { return m_hitsPrepared; }

    /// alter the hit preparation flag
    void setHitsPrepared(const bool flag = true) { m_hitsPrepared = flag; }

  private:

    /// Construction without providing a sensor makes no sense
    VeloSensorHits();

    /// r-type wrapper for closest hit in global frame implementation
    value_type closestHitWrap(unsigned int zone, double pos, double tol, const DeVeloRType*) {
      return closestHitImp(zone, pos, tol); 
    }
      
    /// phi-type wrapper for closest hit in global frame implementation
    value_type closestHitWrap(unsigned int zone, double pos, double tol, const DeVeloPhiType*) {
      return closestHitImp(zone, (m_sensor->isRight() && pos<0 ? pos + 2.0*Gaudi::Units::pi : pos), tol); 
    }
      
    /// r-type wrapper for closest hit in half box frame implementation
    value_type closestHitHalfBoxWrap(unsigned int zone, double pos, double tol, const DeVeloRType*) {
      return closestHitHalfBoxImp(zone, pos, tol); 
    }
      
    /// phi-type wrapper for closest hit in half box frame implementation
    value_type closestHitHalfBoxWrap(unsigned int zone, double pos, double tol, const DeVeloPhiType*) {
      return closestHitHalfBoxImp(zone, (m_sensor->isRight() && pos<0 ? pos + 2.0*Gaudi::Units::pi : pos), tol); 
    }
      
    /// r-type wrapper for closest hit in ideal frame implementation
    value_type closestHitIdealWrap(unsigned int zone, double pos, double tol, const DeVeloRType*) {
      return closestHitIdealImp(zone, pos, tol); 
    }
      
    /// phi-type wrapper for closest hit in ideal frame implementation
    value_type closestHitIdealWrap(unsigned int zone, double pos, double tol, const DeVeloPhiType*) {
      return closestHitIdealImp(zone, (m_sensor->isRight() && pos<0 ? pos + 2.0*Gaudi::Units::pi : pos), tol); 
    }
      
    /// Implementation of access to hit closest to pos inside pos +/- tol, if any. Pos and tol refer to global coordinates.
    value_type closestHitImp(unsigned int zone, double pos, double tol) ;

    /// Implementation of access to hit closest to pos inside pos +/- tol, if any. Pos and tol refer to half box coordinates.
    value_type closestHitHalfBoxImp(unsigned int zone, double pos, double tol) ;

    /// Implementation of access to hit closest to pos inside pos +/- tol, if any. Pos and tol refer to ideal coordinates.
    value_type closestHitIdealImp(unsigned int zone, double pos, double tol) ;

    /// r-type wrapper for closest hit in global frame implementation
    typename DATATYPE::range_type hitsWrap(unsigned int zone, double min, double max, const DeVeloRType*) {
      return hitsImp(zone, min, max); 
    }
      
    /// phi-type wrapper for closest hit in global frame implementation
    typename DATATYPE::range_type hitsWrap(unsigned int zone, double min, double max, const DeVeloPhiType*) {
      return hitsImp(zone
          , (m_sensor->isRight() && min<0 ? min + 2.0*Gaudi::Units::pi : min)
          , (m_sensor->isRight() && max<0 ? max + 2.0*Gaudi::Units::pi : max));
    }
      
    /// r-type wrapper for closest hit in half box frame implementation
    typename DATATYPE::range_type hitsHalfBoxWrap(unsigned int zone, double min, double max, const DeVeloRType*) {
      return hitsHalfBoxImp(zone, min, max); 
    }
      
    /// phi-type wrapper for closest hit in half box frame implementation
    typename DATATYPE::range_type hitsHalfBoxWrap(unsigned int zone, double min, double max, const DeVeloPhiType*) {
      return hitsHalfBoxImp(zone
          , (m_sensor->isRight() && min<0 ? min + 2.0*Gaudi::Units::pi : min)
          , (m_sensor->isRight() && max<0 ? max + 2.0*Gaudi::Units::pi : max));
    }
      
    /// r-type wrapper for closest hit in ideal frame implementation
    typename DATATYPE::range_type hitsIdealWrap(unsigned int zone, double min, double max, const DeVeloRType*) {
      return hitsIdealImp(zone, min, max); 
    }
      
    /// phi-type wrapper for closest hit in ideal frame implementation
    typename DATATYPE::range_type hitsIdealWrap(unsigned int zone, double min, double max, const DeVeloPhiType*) {
      return hitsIdealImp(zone
          , (m_sensor->isRight() && min<0 ? min + 2.0*Gaudi::Units::pi : min)
          , (m_sensor->isRight() && max<0 ? max + 2.0*Gaudi::Units::pi : max));
    }
      
    /// Implementation of accesss to  hits in a zone inside an interval specified in global coordinates
    typename DATATYPE::range_type  hitsImp(unsigned int zone, double min, double max);

    /// Implementation of accesss to  hits in a zone inside an interval specified in half box coordinates
    typename DATATYPE::range_type  hitsHalfBoxImp(unsigned int zone, double min, double max);

    /// Implementation of accesss to  hits in a zone inside an interval specified in ideal coordinates
    typename DATATYPE::range_type  hitsIdealImp(unsigned int zone, double min, double max);

    /// Implementation of access to closest hit at reference radius in global frame. Only makes sense for phi.
    value_type closestHitAtRImp(unsigned int zone, double r, double pos, double tol, const DeVeloPhiType*);

    /// Implementation of access to closest hit at reference radius in half box frame. Only makes sense for phi.
    value_type closestHitAtRHalfBoxImp(unsigned int zone, double r, double pos, double tol, const DeVeloPhiType*);

    /// Implementation of access to closest hit at reference radius in ideal frame. Only makes sense for phi.
    value_type closestHitAtRIdealImp(unsigned int zone, double r, double pos, double tol, const DeVeloPhiType*);

  private:

    const SENSORTYPE* m_sensor;

    container_type m_data[ZONES];

    bool m_hitsPrepared;
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
    m_hitsPrepared = false;
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
    , m_hitsPrepared(false)
  {;}

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  typename DATATYPE::range_type VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::hitsImp(unsigned int zone, double min, double max)
  {
    // cache begin and end
    iterator begin = m_data[zone].begin();
    iterator end   = m_data[zone].end();

    // find the first hit
    iterator beginHit =
      std::lower_bound(begin, end, min,
                       boost::lambda::bind(&DATATYPE::sortCoord,boost::lambda::_1) <  boost::lambda::_2);

    // find the last hit
    iterator endHit =
      std::lower_bound(beginHit, end, max,
                       boost::lambda::bind(&DATATYPE::sortCoord,boost::lambda::_1) <  boost::lambda::_2);

    return range_type( beginHit, endHit ) ;
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  typename DATATYPE::range_type VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::hitsHalfBoxImp(unsigned int zone, double min, double max)
  {
    // cache begin and end
    iterator begin = m_data[zone].begin();
    iterator end   = m_data[zone].end();

    // find the first hit
    iterator beginHit =
      std::lower_bound(begin, end, min,
                       boost::lambda::bind(&DATATYPE::sortCoordHalfBox,boost::lambda::_1) <  boost::lambda::_2);

    // find the last hit
    iterator endHit =
      std::lower_bound(beginHit, end, max,
                       boost::lambda::bind(&DATATYPE::sortCoordHalfBox,boost::lambda::_1) <  boost::lambda::_2);

    return range_type( beginHit, endHit ) ;
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  typename DATATYPE::range_type VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::hitsIdealImp(unsigned int zone, double min, double max)
  {
    // cache begin and end
    iterator begin = m_data[zone].begin();
    iterator end   = m_data[zone].end();

    // find the first hit
    iterator beginHit =
      std::lower_bound(begin, end, min,
                       boost::lambda::bind(&DATATYPE::sortCoordIdeal,boost::lambda::_1) <  boost::lambda::_2);

    // find the last hit
    iterator endHit =
      std::lower_bound(beginHit, end, max,
                       boost::lambda::bind(&DATATYPE::sortCoordIdeal,boost::lambda::_1) <  boost::lambda::_2);

    return range_type( beginHit, endHit ) ;
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  DATATYPE*  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::closestHitImp(unsigned int zone, double pos, double tol)
  {
    // nothing to see here, please move along
    if ( m_data[zone].empty() ) return 0;

    DATATYPE* closestHit = 0;

    iterator it  = m_data[zone].begin();
    iterator end = m_data[zone].end();
    for ( ; it != end; ++it ) {
      double coord = (*it)->sortCoord();
      if ( pos + tol < coord ) break;
      if ( pos - tol < coord ) {
        tol = fabs( pos - coord );
        closestHit = *it;
      }
    }

    return closestHit;
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  DATATYPE*  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::closestHitHalfBoxImp(unsigned int zone, double pos, double tol)
  {
    // nothing to see here, please move along
    if ( m_data[zone].empty() ) return 0;


    DATATYPE* closestHit = 0;

    iterator it  = m_data[zone].begin();
    iterator end = m_data[zone].end();
    for ( ; it != end; ++it ) {
      double coord = (*it)->sortCoordHalfBox();
      if ( pos + tol < coord ) break;
      if ( pos - tol < coord ) {
        tol = fabs( pos - coord );
        closestHit = *it;
      }
    }

    return closestHit;
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  DATATYPE*  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::closestHitIdealImp(unsigned int zone, double pos, double tol)
  {
    // nothing to see here, please move along
    if ( m_data[zone].empty() ) return 0;

    DATATYPE* closestHit = 0;

    iterator it  = m_data[zone].begin();
    iterator end = m_data[zone].end();
    for ( ; it != end; ++it ) {
      double coord = (*it)->sortCoordIdeal();
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

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  DATATYPE*  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::closestHitAtRImp(unsigned int zone, double r, double pos, double tol, const DeVeloPhiType*)
  {
    // nothing to see here, please move along
    if ( m_data[zone].empty() ) return 0;

    double mappedPos = m_sensor->isRight() && pos<0 ? pos + 2.0*Gaudi::Units::pi : pos;
    DATATYPE* closestHit = 0;

    iterator it  = m_data[zone].begin();
    iterator end = m_data[zone].end();
    for ( ; it != end; ++it ) {
      double coord = (*it)->sortCoord() + m_sensor->globalPhiOffset(zone,r);
      if ( mappedPos + tol < coord ) break;
      if ( mappedPos - tol < coord ) {
        tol = fabs( mappedPos - coord );
        closestHit = *it;
      }
    }

    return closestHit;
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  DATATYPE*  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::closestHitAtRHalfBoxImp(unsigned int zone, double r, double pos, double tol, const DeVeloPhiType*)
  {
    // nothing to see here, please move along
    if ( m_data[zone].empty() ) return 0;

    double mappedPos = m_sensor->isRight() && pos<0 ? pos + 2.0*Gaudi::Units::pi : pos;
    DATATYPE* closestHit = 0;

    iterator it  = m_data[zone].begin();
    iterator end = m_data[zone].end();
    for ( ; it != end; ++it ) {
      double coord = (*it)->sortCoordHalfBox() + m_sensor->halfBoxPhiOffset(zone,r);
      if ( mappedPos + tol < coord ) break;
      if ( mappedPos - tol < coord ) {
        tol = fabs( mappedPos - coord );
        closestHit = *it;
      }
    }

    return closestHit;
  }

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
  DATATYPE*  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::closestHitAtRIdealImp(unsigned int zone, double r, double pos, double tol, const DeVeloPhiType*)
  {
    // nothing to see here, please move along
    if ( m_data[zone].empty() ) return 0;

    double mappedPos = m_sensor->isRight() && pos<0 ? pos + 2.0*Gaudi::Units::pi : pos;
    DATATYPE* closestHit = 0;

    iterator it  = m_data[zone].begin();
    iterator end = m_data[zone].end();
    for ( ; it != end; ++it ) {
      double coord = (*it)->sortCoordIdeal() + m_sensor->idealPhiOffset(zone,r);
      if ( mappedPos + tol < coord ) break;
      if ( mappedPos - tol < coord ) {
        tol = fabs( mappedPos - coord );
        closestHit = *it;
      }
    }

    return closestHit;
  }

  typedef VeloSensorHits<DeVeloRType,VeloRHit,4>   VeloRSensorHits;   ///< short name for default r hit container
  typedef VeloSensorHits<DeVeloRType,VeloPhiHit,2> VeloPhiSensorHits; ///< short name for default phi hit container

} // namespace Tf
#endif // INCLUDE_TF_VELOSENSORHITS_H


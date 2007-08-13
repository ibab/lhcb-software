// $Id: VeloSensorHits.h,v 1.1.1.1 2007-08-13 11:13:58 jonrob Exp $
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

  /** @class VeloSensorHits VeloSensorHits.h
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
          inline bool operator() (const VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>* lhs, const VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>* rhs)
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

        /// global minimum phi of a given zone
        double phiMin(unsigned int zone) const { return m_phiMin[zone]; } 

        /// global maximum phi of a given zone
        double phiMax(unsigned int zone) const { return m_phiMax[zone]; } 

        /// global minimum r of a given zone
        double rMin(unsigned int zone) const { return m_rMin[zone]; } 

        /// global maximum r of a given zone
        double rMax(unsigned int zone) const { return m_rMax[zone]; } 

        /// check whether a given phi +/- tol lies inside the zone boundaries in the global frame
        bool insidePhiRange(unsigned int zone, double phi, double tol);
        
        /// check whether a given r +/- tol lies inside the zone boundaries in the global frame
        bool insideRRange(unsigned int zone, double r, double tol);
        
        /// minimum phi in VELO half box frame of a given zone
        double phiMinHalfBox(unsigned int zone) const { return m_phiMinHalfBox[zone]; } 

        /// maximum phi in VELO half box frame of a given zone
        double phiMaxHalfBox(unsigned int zone) const { return m_phiMaxHalfBox[zone]; } 

        /// minimum r in VELO half box frame of a given zone
        double rMinHalfBox(unsigned int zone) const { return m_rMinHalfBox[zone]; } 

        /// maximum r in VELO half box frame of a given zone
        double rMaxHalfBox(unsigned int zone) const { return m_rMaxHalfBox[zone]; } 

        /// check whether a given phi +/- tol lies inside the zone boundaries in the half box frame
        bool insidePhiRangeHalfBox(unsigned int zone, double phi, double tol);
        
        /// check whether a given r +/- tol lies inside the zone boundaries in the half box frame
        bool insideRRangeHalfBox(unsigned int zone, double r, double tol);
        
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

        double m_phiMin[ZONES];
        double m_phiMax[ZONES];
        double m_rMin  [ZONES];
        double m_rMax  [ZONES];
        double m_phiMinHalfBox[ZONES];
        double m_phiMaxHalfBox[ZONES];
        double m_rMinHalfBox  [ZONES];
        double m_rMaxHalfBox  [ZONES];
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

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
    inline bool  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::insidePhiRange(unsigned int zone, double phi, double tol) {
      return ( phi > m_phiMin[zone] - tol ) && ( phi < m_phiMax[zone] + tol ); 
    }
  
  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
    inline bool  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::insideRRange(unsigned int zone, double r, double tol) {
      return ( r > m_rMin[zone] - tol ) && ( r < m_rMax[zone] + tol ); 
    }
  
  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
    inline bool  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::insidePhiRangeHalfBox(unsigned int zone, double phi, double tol) {
      return ( phi > m_phiMinHalfBox[zone] - tol ) && ( phi < m_phiMaxHalfBox[zone] + tol ); 
    }
  
  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
    inline bool  VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::insideRRangeHalfBox(unsigned int zone, double r, double tol) {
      return ( r > m_rMinHalfBox[zone] - tol ) && ( r < m_rMaxHalfBox[zone] + tol ); 
    }
  
  //----------------------------------------------------------------------
  // implementations  
  //----------------------------------------------------------------------

  template<typename SENSORTYPE, typename DATATYPE, unsigned int ZONES>
    VeloSensorHits<SENSORTYPE,DATATYPE,ZONES>::VeloSensorHits(const SENSORTYPE* sensor)
    : m_sensor(sensor)
    { 
      if (const DeVeloRType* rs = sensor->rType()) {
        for (unsigned int localZone=0; localZone<ZONES; ++localZone) {
          unsigned int minStrip = localZone*512;
          unsigned int maxStrip = minStrip+511;
          unsigned int midStrip = (minStrip+maxStrip)/2;
          unsigned int zone     = (rs->isDownstream() ? 3-localZone : localZone);
          
          // determine the phi ranges of the zones in global frame
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMin = rs->globalStripLimits(minStrip);
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMax = rs->globalStripLimits(maxStrip);
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMid = rs->globalStripLimits(midStrip);
          std::vector<double> phiLimits;
          phiLimits.push_back(globalLimitsMin.first.phi()); phiLimits.push_back(globalLimitsMin.second.phi());
          phiLimits.push_back(globalLimitsMax.first.phi()); phiLimits.push_back(globalLimitsMax.second.phi());
          phiLimits.push_back(globalLimitsMid.first.phi()); phiLimits.push_back(globalLimitsMid.second.phi());
          m_phiMin[zone] = *std::min_element(phiLimits.begin(),phiLimits.end());
          m_phiMax[zone] = *std::max_element(phiLimits.begin(),phiLimits.end());
          
          // determine the phi ranges of the zones in VELO half box frame
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMin
            (rs->globalToVeloHalfBox(globalLimitsMin.first),rs->globalToVeloHalfBox(globalLimitsMin.second));
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMax
            (rs->globalToVeloHalfBox(globalLimitsMax.first),rs->globalToVeloHalfBox(globalLimitsMax.second));
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMid
            (rs->globalToVeloHalfBox(globalLimitsMid.first),rs->globalToVeloHalfBox(globalLimitsMid.second));
          phiLimits.clear();
          phiLimits.push_back(halfBoxLimitsMin.first.phi()); phiLimits.push_back(halfBoxLimitsMin.second.phi());
          phiLimits.push_back(halfBoxLimitsMax.first.phi()); phiLimits.push_back(halfBoxLimitsMax.second.phi());
          phiLimits.push_back(halfBoxLimitsMid.first.phi()); phiLimits.push_back(halfBoxLimitsMid.second.phi());
          m_phiMinHalfBox[zone] = *std::min_element(phiLimits.begin(),phiLimits.end());
          m_phiMaxHalfBox[zone] = *std::max_element(phiLimits.begin(),phiLimits.end());

          // r limits are the radii of the outer strip + local pitch/2 and the innder strip - local pitch/2
          m_rMin[zone]        = rs->globalROfStrip(minStrip)  - rs->rPitch(minStrip)/2.0;
          m_rMax[zone]        = rs->globalROfStrip(maxStrip)  + rs->rPitch(maxStrip)/2.0;
          m_rMinHalfBox[zone] = rs->halfboxROfStrip(minStrip) - rs->rPitch(minStrip)/2.0;
          m_rMaxHalfBox[zone] = rs->halfboxROfStrip(maxStrip) + rs->rPitch(maxStrip)/2.0;
        }
        
      } else if (const DeVeloPhiType* ps = sensor->phiType()) {
        for (unsigned int zone=0; zone<ZONES; ++zone) {
          unsigned int minStrip = (zone ? 683 : 0 );
          unsigned int maxStrip = (zone ? 2047 : 682 );
          unsigned int midStrip = (minStrip+maxStrip)/2;
          
          // determine the r ranges of the zones in global frame
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMin = ps->globalStripLimits(minStrip);
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMax = ps->globalStripLimits(maxStrip);
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> globalLimitsMid = ps->globalStripLimits(midStrip);
          std::vector<double> rLimits;
          rLimits.push_back(globalLimitsMin.first.rho()); rLimits.push_back(globalLimitsMin.second.rho());
          rLimits.push_back(globalLimitsMax.first.rho()); rLimits.push_back(globalLimitsMax.second.rho());
          rLimits.push_back(globalLimitsMid.first.rho()); rLimits.push_back(globalLimitsMid.second.rho());
          m_rMin[zone] = *std::min_element(rLimits.begin(),rLimits.end());
          m_rMax[zone] = *std::max_element(rLimits.begin(),rLimits.end());
          std::vector<double> phiLimits;
          phiLimits.push_back(globalLimitsMin.first.phi()); phiLimits.push_back(globalLimitsMin.second.phi());
          phiLimits.push_back(globalLimitsMax.first.phi()); phiLimits.push_back(globalLimitsMax.second.phi());
          m_phiMin[zone] = *std::min_element(phiLimits.begin(),phiLimits.end()); 
          m_phiMax[zone] = *std::max_element(phiLimits.begin(),phiLimits.end()); 
          
          // determine the r ranges of the zones in VELO half box frame
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMin
            (rs->globalToVeloHalfBox(globalLimitsMin.first),rs->globalToVeloHalfBox(globalLimitsMin.second));
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMax
            (rs->globalToVeloHalfBox(globalLimitsMax.first),rs->globalToVeloHalfBox(globalLimitsMax.second));
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> halfBoxLimitsMid
            (rs->globalToVeloHalfBox(globalLimitsMid.first),rs->globalToVeloHalfBox(globalLimitsMid.second));
          rLimits.clear();
          rLimits.push_back(halfBoxLimitsMin.first.rho()); rLimits.push_back(halfBoxLimitsMin.second.rho());
          rLimits.push_back(halfBoxLimitsMax.first.rho()); rLimits.push_back(halfBoxLimitsMax.second.rho());
          rLimits.push_back(halfBoxLimitsMid.first.rho()); rLimits.push_back(halfBoxLimitsMid.second.rho());
          m_rMinHalfBox[zone] = *std::min_element(rLimits.begin(),rLimits.end());
          m_rMaxHalfBox[zone] = *std::max_element(rLimits.begin(),rLimits.end());
          phiLimits.clear();
          phiLimits.push_back(halfBoxLimitsMin.first.phi()); phiLimits.push_back(halfBoxLimitsMin.second.phi());
          phiLimits.push_back(halfBoxLimitsMax.first.phi()); phiLimits.push_back(halfBoxLimitsMax.second.phi());
          m_phiMinHalfBox[zone] = *std::min_element(phiLimits.begin(),phiLimits.end()); 
          m_phiMaxHalfBox[zone] = *std::max_element(phiLimits.begin(),phiLimits.end()); 

          // phi limits are the radii of the max strip + local pitch/2 and the min strip - local pitch/2
          // the pitch is maximum pitch at the local radius of the outward strip limit
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> localLimitsMin = ps->localStripLimits(minStrip);
          std::pair<Gaudi::XYZPoint, Gaudi::XYZPoint> localLimitsMax = ps->localStripLimits(maxStrip);
          double localMaxRMinStrip = std::max(localLimitsMin.first.rho(),localLimitsMin.second.rho());                    
          double localMaxRMaxStrip = std::max(localLimitsMax.first.rho(),localLimitsMax.second.rho());                    
          double minStripPitch = ps->phiPitch(localMaxRMinStrip);
          double maxStripPitch = ps->phiPitch(localMaxRMaxStrip);
          double pitch = std::max(minStripPitch,maxStripPitch);
          
          m_phiMin[zone]        -= pitch/2.0;
          m_phiMax[zone]        += pitch/2.0;
          m_phiMinHalfBox[zone] -= pitch/2.0;
          m_phiMaxHalfBox[zone] += pitch/2.0;
        }
      }
    }

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


// $Id: PatVeloHit.h,v 1.4 2007-09-14 13:20:29 dhcroft Exp $
#ifndef INCLUDE_TF_PATVELOHIT_H
#define INCLUDE_TF_PATVELOHIT_H 1

#include <math.h>

#include "VeloDet/DeVeloRType.h"
#include "VeloDet/DeVeloPhiType.h"

#include "TfKernel/VeloPhiHit.h"
#include "TfKernel/VeloRHit.h"
#include "TfKernel/VeloHitExtension.h"

namespace Tf {
  /** @class PatVeloHit PatVeloHit.h
   *  A hit extension specialised for the space tracking
   *
   * @author Kurt Rinnert <kurt.rinnert@cern.ch>
   * @date   2007-08-08
   * @see PatVeloSpaceTracking
   */
  template<typename SENSORTYPE, typename HIT>
    class PatVeloHit : public Tf::VeloHitExtension<HIT> {

      //using Tf::VeloHitExtension<HIT>::hit;

      public:

        //----------------------------------------------------------------------
        // typedefs
        //----------------------------------------------------------------------

        typedef std::vector<PatVeloHit* > container_type; 
        typedef LoKi::Range_<container_type> range_type;
        typedef typename HIT::hit_type_tag    hit_type_tag;

      public:

        /// Constructor from hit pointer
        PatVeloHit( const HIT* hit = 0 )
          : Tf::VeloHitExtension<HIT>(hit)
          , m_referenceR(0.0)
          , m_referencePhi(0.0)
          , m_sinPhi(0.0)
          , m_cosPhi(1.0)
          , m_zone(hit->sensor()->globalZoneOfStrip(hit->strip()))
          {;}

        ~PatVeloHit() {;} ///< Destructor

        /// access to weight
        double weight() const { return this->hit()->weight(); }

        /// access to sensor pointer
        const SENSORTYPE* sensor() const { return this->hit()->sensor(); } 

        /// access to sensor number of hit
        unsigned int sensorNumber() const { 
	  return this->hit()->channelID().sensor(); 
	} 

        /// access to strip number of hit
        unsigned int stripNumber() const { 
	  return this->hit()->channelID().strip(); 
	} 

        /// shortcut access to the global zone
        unsigned int zone() const { return m_zone; }

        /// access to reference radius
        double referenceR() const { return m_referenceR; }

        /// access to phi a given radius
        double referencePhi() const { return m_referencePhi; }

        /// access cache of sin(phi)
        double sinPhi() const { return m_sinPhi; }

        /// access cache of cos(phi)
        double cosPhi() const { return m_cosPhi; }

        /// set reference radius
        void setReferenceR(double r) { m_referenceR = r; }

        /// set phi at reference radius
        void setReferencePhi(double phi) { 
          m_referencePhi = phi; 
          m_sinPhi       = sin(phi); 
          m_cosPhi       = cos(phi); 
        }

        /// set both, reference radius and phi
        void setRadiusAndPhi(double r, double phi) {
          m_referenceR   = r;
          m_referencePhi = phi; 
          m_sinPhi       = sin(phi); 
          m_cosPhi       = cos(phi); 
        }

      private:

        double m_referenceR;
        double m_referencePhi;
        double m_sinPhi;
        double m_cosPhi;
        unsigned int m_zone;

    };

  typedef PatVeloHit<DeVeloRType,Tf::VeloRHit>     PatVeloRHit;
  typedef PatVeloHit<DeVeloPhiType,Tf::VeloPhiHit> PatVeloPhiHit;

  typedef PatVeloRHit::range_type   PatVeloRHitRange;
  typedef PatVeloPhiHit::range_type PatVeloPhiHitRange;
}
#endif // INCLUDE_TF_PATVELOHIT_H


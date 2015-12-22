// $Id: PatRZTrack.h,v 1.6 2009-07-20 11:35:32 dhcroft Exp $
#ifndef TF_PATRZTRACK_H
#define TF_PATRZTRACK_H 1

// Include files
// Include files
#include "GaudiKernel/MsgStream.h"

#include <cmath>

#include "PatVeloHit.h"

namespace Tf {
  /** @class PatRZTrack PatRZTrack.h
   *  This stores a local, work RZ Velo track
   *
   *  @authors Olivier Callot, Kurt Rinnert
   *  @date   2005-05-31
   */

  class PatRZTrack {
    public:
      /// Standard constructor
      PatRZTrack( ) ;

      ~PatRZTrack( ) { }; ///< Destructor

      // inline access to track parameters
      inline double meanZ()       const { return m_sz / m_s0; }
      inline double rSlope()      const { return m_slope; }
      inline double errR2()       const { return 1. / m_s0; }
      inline double errSl2()      const { return m_slopeErr; }
      inline double rPred( double z )  const { return m_pos0 + z * m_slope; }
      inline double rErr2( double z )  const {
        return errR2() + std::pow(z-meanZ(),2) * m_slopeErr;
      }
      inline bool valid()          const { return m_valid; }
      inline PatVeloRHits* coords()       { return &m_coord; };
      inline unsigned int nbCoords() const { return m_coord.size(); }
      inline int missedStations() const { return m_missedStations; }
      inline int zone()            const { return m_zone; } ///< get R zone
      inline bool backward()       const { return m_backward; }

      inline unsigned int minSensor() const { return m_minSensor; }
      inline unsigned int maxSensor() const { return m_maxSensor; }

      // set track parmaters
      void setValid( bool flag )  { m_valid = flag; }
      void setZone( unsigned int zone )    { m_zone = zone;   } ///< Set track R zone
      void setMissedStations( int missed ) { m_missedStations = missed ; }
      void setBackward( bool flag )      { m_backward = flag; }

      void addRCoord( PatVeloRHit* coord ); ///< new R coord + fit

      void tagUsedCoords();   ///< Label coords in PatVeloDataHolder as used

      double rInterpolated( double z ); ///< get expected r at given Z

      /// get the chi2 of the track
      double chi2() const;

    protected:

    private:
      double m_s0;   ///< sum of weight of R clusters
      double m_sr;   ///< sum of ( weight of R clusters * r )
      double m_sz;   ///< sum of ( weight of R clusters * z )
      double m_srz;  ///< sum of ( weight of R clusters * r * z )
      double m_sz2;  ///< sum of ( weight of R clusters * z**2 )
      bool   m_valid;///< Track is OK to use
      unsigned int    m_zone; ///< R sensor zone of track
      bool   m_backward; ///< True if backwards track
      int m_missedStations; ///< Num of stations missed to beam line
      unsigned int m_minSensor; ///< min sensor number on track
      unsigned int m_maxSensor; ///< max sensor number on track

      double m_pos0; ///< position on track in R at z=0
      double m_slope; ///< dr/dz

      double m_slopeErr; ///< error on dr/dz

      PatVeloRHits m_coord; ///< Container of R coords

  };
}
#endif // TF_PATRZTRACK_H

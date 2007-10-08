// $Id: PatRZTrack.h,v 1.2 2007-10-08 17:06:05 dhcroft Exp $
#ifndef TF_PATRZTRACK_H 
#define TF_PATRZTRACK_H 1

// Include files
// Include files
#include "GaudiKernel/MsgStream.h"

#include "TfKernel/VeloRHit.h"

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
      inline double meanZ()       const { return m_meanZ; }
      inline double rSlope()      const { return m_slope; }
      inline double errR2()       const { return m_posErr; }
      inline double errSl2()      const { return m_slopeErr; }
      inline double rPred( double z )  const { return m_pos0 + z * m_slope; }
      inline double rErr2( double z )  const { 
        return m_posErr + (z-m_meanZ)*(z-m_meanZ) * m_slopeErr;  
      }
      inline bool valid()          const { return m_valid; }
      inline VeloRHits* coords()       { return &m_coord; };
      inline unsigned int nbCoords()              const { return m_nbCoord; }
      inline bool missedStations() const { return m_missedStations; }
      inline int zone()            const { return m_zone; } ///< get R zone
      inline bool backward()       const { return m_backward; }

      inline unsigned int minSensor() const { return m_minSensor; }
      inline unsigned int maxSensor() const { return m_maxSensor; }

      // set track parmaters
      void setValid( bool flag )  { m_valid = flag; }
      void setZone( unsigned int zone )    { m_zone = zone;   } ///< Set track R zone
      void setMissedStations( bool flag )  { m_missedStations = flag; }
      void setBackward( bool flag )      { m_backward = flag; }  

      void addRCoord( VeloRHit* coord ); ///< new R coord + fit

      void tagUsedCoords();   ///< Label coords in PatVeloDataHolder as used

      double rInterpolated( double z ); ///< get expected r at given Z

      /// get the chi2 of the track
      inline double chi2() const{
	return (m_sz2 - 2.*m_sz*m_pos0 - 2.*m_srz*m_slope + m_pos0*m_pos0 +
		2.*m_pos0*m_slope*m_sr + m_sr2);
      }

    protected:

    private:
      double m_s0;
      double m_sr;
      double m_sz;
      double m_srz;
      double m_sr2;
      double m_sz2;
      bool   m_valid;
      unsigned int    m_zone;
      bool   m_backward;
      bool   m_missedStations;
      unsigned int m_minSensor;
      unsigned int m_maxSensor;

      double m_pos0;
      double m_slope;

      double m_meanZ;
      double m_posErr;
      double m_slopeErr;

      unsigned int m_nbCoord;
      VeloRHits m_coord;

  };
}
#endif // TF_PATRZTRACK_H

#ifndef TF_PATVELO_PATVELOALIGNTRACKFILTER_H
#define TF_PATVELO_PATVELOALIGNTRACKFILTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiHistos.h"
#include "GaudiKernel/PhysicalConstants.h"

// from VeloDet
#include "VeloDet/DeVelo.h"

// Local
#include "TfKernel/DefaultVeloRHitManager.h"
#include "TfKernel/DefaultVeloPhiHitManager.h"

namespace Tf {
  /** @class PatVeloAlignTrackFilter PatVeloAlignTrackFilter.h Alignment/PatVeloAlignTrackFilter.h
   *
   *  @author Marco Gersabeck
   *  @date   2008-09-08
   */

  class PatVeloAlignTrackFilter : public GaudiAlgorithm {

    public: 
      /// Standard constructor
      PatVeloAlignTrackFilter( const std::string& name, ISvcLocator* pSvcLocator );
      virtual ~PatVeloAlignTrackFilter( ); ///< Destructor

      virtual StatusCode initialize(); ///< Algorithm initialization
      virtual StatusCode execute   (); ///< Algorithm execution
      virtual StatusCode finalize  (); ///< Algorithm finalization

    private:

      DefaultVeloRHitManager*   m_rHitManager;
      DefaultVeloPhiHitManager* m_phiHitManager;

      std::string m_rHitManagerName;
      std::string m_phiHitManagerName;
      unsigned int m_numberOfRSectors;
      unsigned int m_numberOfPSectors;
      double m_radiusOffset;
      double m_min_r_charge;
      double m_max_r_charge;
      double m_min_phi_charge;
      double m_max_phi_charge;
      int m_min_tot;
      int m_max_tot;
      int m_min_rslice;
      int m_min_cell;
      int m_max_cell;
      int m_max_diff_cell;
      int m_max_sensor;
      bool m_overlaps;
      int m_nBinsR;
      int m_nBinsPhi;
      int m_nStations;
      bool m_bigCell;
      int m_min_overlap;

      bool m_debugLevel;
      bool m_verboseLevel;

      int m_3Dhits[ 360 ][ 360 ][ 42 ];
      int m_hits[ 360 ][ 360 ];
      int m_hitsR[ 360 ][ 360 ];
      int m_hitsL[ 360 ][ 360 ];
      int m_rhits[ 360 ];
      int m_tothits;
      int hitsInCell( int rhit, int phit );
      int hitsInRightCell( int rhit, int phit );
      int hitsInLeftCell( int rhit, int phit );
      int sensorHitsInCell( int rhit, int phit, int sensor );
      StatusCode saveTracks();
  };
}

#endif // TF_PATVELO_PATVELOALIGNTRACKFILTER_H


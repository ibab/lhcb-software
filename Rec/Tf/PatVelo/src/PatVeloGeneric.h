// $Id: PatVeloGeneric.h,v 1.4 2008-10-09 11:57:37 krinnert Exp $
#ifndef TF_PAT_PATVELOGENERIC_H 
#define TF_PAT_PATVELOGENERIC_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/PhysicalConstants.h"

// output tracks and states
#include "Event/Track.h"
#include "Event/State.h"

// from VeloDet
#include "VeloDet/DeVelo.h"


// Local
#include "PatVeloRHitManager.h"
#include "PatVeloPhiHitManager.h"
#include "PatVeloTrackTool.h"
#include "PatGenericFitter.h"

namespace Tf {
  /** @class PatVeloGeneric PatVeloGeneric.h Tf/PatVeloGeneric.h
   *
   *  @author Tomas LASTOVICKA
   *  @date   2005-09-09
   */

  class PatVeloGeneric : public GaudiAlgorithm {

    public: 
      /// Standard constructor
      PatVeloGeneric( const std::string& name, ISvcLocator* pSvcLocator );
      virtual ~PatVeloGeneric( ); ///< Destructor

      virtual StatusCode initialize(); ///< Algorithm initialization
      virtual StatusCode execute   (); ///< Algorithm execution
      virtual StatusCode finalize  (); ///< Algorithm finalization

    private:
      static constexpr const auto s_binary = 1.0/sqrt(12.);  // digital resolution a priori

      CircularRangeUtils<double> m_angleUtils = {-Gaudi::Units::pi,Gaudi::Units::pi };
      PatVeloRHitManager*   m_rHitManager = nullptr;
      PatVeloPhiHitManager* m_phiHitManager = nullptr;
      PatVeloTrackTool * m_PatVeloTrackTool = nullptr; ///< tool to handle PatVeloSpaceTracks
      DeVelo* m_velo = nullptr;
      LHCb::Tracks* m_outputTracks = nullptr;  ///< Container for output Space tracks

      std::string m_outputTracksLocation;
      std::string m_rHitManagerName;
      std::string m_phiHitManagerName;
      std::string m_trackToolName; /// name of the track tool instance used here

      bool m_cleanSeeds;  ///< Cleaning track seeds flag
      bool m_privateBest; ///< Best candidate cluster selection flag
      bool m_align;       ///< Flag to perform own alignment
      bool m_forward;     ///< Propage seeds also in the forward direction
      bool m_checkIfSensorIsReadOut;   ///< If true use only sensors that are readout, according to CondDb
      bool m_acdc;        ///< Turn R sensors 180 deg around x-axis (real data)  
      bool m_doNotRefit;  ///< Force no re-fitting during track propagation
      bool m_considerOverlaps; ///< Look for clusters in the other hals during propagation

      double m_maxGapForOverlapSearch; ///< Maximum gap between halfs for which overlap search is enabled 
      double m_sigma;    ///< Corridor width in standard deviations
      double m_rOff;     ///< Alignment tolerance in R
      double m_pOff;     ///< Alignment tolerance in phi
      double m_errorX2, m_errorY2, m_errorTx2, m_errorTy2;
      double m_momentumError;
      double m_momentumDefault; ///< Default track momentum
      double m_inner2;
      double m_outer2;  

      
      std::vector<double> m_shiftL;
      std::vector<double> m_shiftR;

      int  m_kalmanState; ///< Kalman seeding (0=standard, 1=last module, 2=first module)

      int m_nEvt;
      int m_nSkip;
      int m_nMin;
      unsigned int m_clusterCut;
      unsigned int numberOfRSectors;
      unsigned int numberOfPSectors;


      ///  Check whether the VELO is closed
      bool veloIsClosed() {
        return fabs(m_velo->halfBoxOffset(0).x() - m_velo->halfBoxOffset(1).x()) < m_maxGapForOverlapSearch;
      }

      /// Returns R-cluster candidate
      PatVeloRHit* rCandidate(PatVeloRHitManager::Station* rStation, unsigned int zone, double rEst)  {    
        if (rEst < 8.2 || rEst > 42.) return nullptr; // to be changed or removed!    
        const DeVeloRType* rSensor = rStation->sensor();
        double rCor = s_binary*rSensor->rPitch( rEst );    
        rCor *= rCor;                  
        rCor += m_rOff;
        rCor = sqrt(rCor);
        rCor *= m_sigma;
        debug() << "R estimate = " << rEst << ", R tolerance = " << rCor << endmsg;
        return rStation->closestHitHalfBox( zone,rEst,rCor );
      }

      /// Returns phi-cluster candidate
      PatVeloPhiHit* pCandidate(PatVeloRHitManager::Station* rStation, PatGenericFitter* fitter)  {
        bool consist = false;
        double pEst = 0;
        double rEst = 0;
        double zP = 0;
        PatVeloPhiHitManager::Station* phiStation 
          = m_phiHitManager->station(rStation->sensor()->associatedPhiSensor()->sensorNumber());
        unsigned int iSectorP = 0;
        for ( ; numberOfPSectors > iSectorP; ++iSectorP) {
          zP = phiStation->z();
          rEst = fitter->rExtrap(zP);      
          consist = pSectorTest( phiStation,iSectorP,rEst );
          if (consist) break;        
        }
        if (!consist) {
          debug() << "Extrapolated r not consistent with any phi zone." << endmsg;
          return nullptr; 
        }       
        double pCor = m_inner2;
        if ( 1 == iSectorP ) pCor = m_outer2;
        pCor += m_pOff;
        pCor = sqrt(pCor);
        pCor *= m_sigma;
        pEst = fitter->pExtrap(zP);
        debug() << "PHI estimate = " << pEst << ", PHI tolerance = " << pCor << endmsg;
        auto best = phiStation->closestHitAtRHalfBox( iSectorP,rEst,pEst,pCor );
        if (!best)
          debug() << "No phi hit in search window." << endmsg;
        return best;
      }

      /// Tests R-sector for consistency in phi
      bool rSectorTest(PatVeloRHitManager::Station* rStation, unsigned int zone, double phi) {
        unsigned int correctedZone = m_acdc ? 3-zone : zone;
        return m_angleUtils.contains(rStation->sensor()->halfboxPhiRange(correctedZone),phi);
      }

      /// Tests phi-sector for consistency in R
      bool pSectorTest(PatVeloPhiHitManager::Station* phiStation, unsigned int zone, double rr) {
        const std::pair<double,double>& rRange = phiStation->sensor()->halfboxRRange(zone);
        if (rr < rRange.first)  return false;    
        if (rr > rRange.second) return false;
        return true;                        
      }

  };
}

#endif // TF_PAT_PATVELOGENERIC_H

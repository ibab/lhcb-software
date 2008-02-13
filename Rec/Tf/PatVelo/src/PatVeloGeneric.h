// $Id: PatVeloGeneric.h,v 1.1 2008-02-13 16:10:19 krinnert Exp $
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

      CircularRangeUtils<double> m_angleUtils;
      PatVeloRHitManager*   m_rHitManager;
      PatVeloPhiHitManager* m_phiHitManager;
      DeVelo* m_velo;
      LHCb::Tracks* m_outputTracks;  ///< Container for output Space tracks

      std::string m_outputTracksLocation;

      bool m_cleanSeeds;  ///< Cleaning track seeds flag
      bool m_privateBest; ///< Best candidate cluster selection flag
      bool m_align;       ///< Flag to perform own alignment
      bool m_forward;     ///< Propage seeds also in the forward direction
      bool m_checkIfSensorIsReadOut;   ///< If true use only sensors that are readout, according to CondDb
      bool m_acdc;        ///< Turn R sensors 180 deg around x-axis (real data)  
      bool m_doNotRefit;  ///< Force no re-fitting during track propagation

      double m_binary;
      double m_sigma;    ///< Corridor width in standard deviations
      double m_rOff;     ///< Alignment tolerance in R
      double m_pOff;     ///< Alignment tolerance in phi
      double m_errorX2, m_errorY2, m_errorTx2, m_errorTy2;
      double m_momentumError;
      double m_momentumDefault; ///< Default track momentum
      double m_inner2;
      double m_outer2;  

      int  m_kalmanState; ///< Kalman seeding (0=standard, 1=last module, 2=first module)

      int m_nEvt;
      int m_nSkip;
      int m_nMin;
      unsigned int m_clusterCut;
      unsigned int numberOfRSectors;
      unsigned int numberOfPSectors;


      /// Returns R-cluster candidate
      PatVeloRHit* rCandidate(PatVeloRHitManager::Station* rStation, unsigned int zone, double rEst)  {    
        PatVeloRHit* best = 0;
        if (rEst < 8.2 || rEst > 42.) return best; // to be changed or removed!    
        const DeVeloRType* rSensor = rStation->sensor();
        double rCor = m_binary*rSensor->rPitch( rEst );    
        rCor *= rCor;                  
        rCor += m_rOff;
        rCor = sqrt(rCor);
        rCor *= m_sigma;
        debug() << "R estimate = " << rEst << ", R tolerance = " << rCor << endmsg;
        best = rStation->closestHitHalfBox( zone,rEst,rCor );
        return best;
      }

      /// Returns phi-cluster candidate
      PatVeloPhiHit* pCandidate(PatVeloRHitManager::Station* rStation, PatGenericFitter* fitter)  {
        PatVeloPhiHit* best = 0;
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
          return best; 
        }       
        double pCor = m_inner2;
        if ( 1 == iSectorP ) {
          pCor = m_outer2;
        }    
        pCor += m_pOff;
        pCor = sqrt(pCor);
        pCor *= m_sigma;
        pEst = fitter->pExtrap(zP);
        debug() << "PHI estimate = " << pEst << ", PHI tolerance = " << pCor << endmsg;
        best = phiStation->closestHitAtRHalfBox( iSectorP,rEst,pEst,pCor );
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


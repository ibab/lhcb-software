// $Id: $
#ifndef RICHPARTICLESEARCHMAIN_H 
#define RICHPARTICLESEARCHMAIN_H 1

// Include files
//#include <CheckPV.h>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/RichRadiatorType.h"
#include "Event/RichRecSegment.h"
// base class
#include "RichRecBase/RichRecTupleAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IAlgTool.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/RichRecTrack.h"
#include "Event/Track.h"
#include "Event/Particle.h"
#include "Event/RecVertex.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichTrackSelector.h"
#include "RichRecBase/IRichIsolatedTrack.h"
#include "RichKernel/IRichRefractiveIndex.h"
#include "RichRecBase/IRichRecGeomTool.h"
#include "IMuonInformation.h"
#include "RichRecBase/IRichTrackEffectiveRefractiveIndex.h"
// RichDet
#include "RichDet/DeRichSphMirror.h"
#include "RichDet/DeRichHPD.h"
#include "RichDet/DeRichLocations.h"
#include "RichDet/DeRich.h"

// Kernel
#include "RichKernel/BoostArray.h"
#include "RichKernel/RichSmartIDCnv.h"
#include "RichKernel/IRichSmartIDTool.h"
#include <RichKernel/IRichRayTracing.h>
#include <Kernel/IOnOffline.h>
//#include "Kernel/DVAlgorithm.h"
//#include "Kernel/DVAlgorithm.h"

// boost
#include "boost/lexical_cast.hpp"
#include <boost/foreach.hpp>
#include "boost/format.hpp"
#include "boost/assign/list_of.hpp"


//class IOnOffline;


/** @class RichParticleSearchMain RichParticleSearchMain.h
 *  
 *
 *  @author Matt Coombes
 *  @date   2010-09-22 
 */
 
 
 namespace Rich
{
  namespace Rec
  {
    namespace MC
    {
    class RichParticleSearchMain : public RichRecTupleAlgBase {
    public:
    	
      /// Standard constructor
      RichParticleSearchMain( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~RichParticleSearchMain( ); ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization
      virtual StatusCode execute   ();    ///< Algorithm execution
      virtual StatusCode finalize  ();    ///< Algorithm finalization

      double MinimumTrackSeparation(LHCb::RichRecSegment*);
      int GetRichInt();



    private:
    
      int m_radTemp;    ///< which rich detector to monitor
      Rich::RadiatorType m_radiator;

      //std::vector<int> m_IsoCuts;
      int m_maxUsedTracks;

        // set to know to stop all MC Truth
      bool m_useMCTruth;

        // particle type when fixed
      int m_particleType;
      Rich::ParticleIDType m_pType;

        // keep totals of particle types
     // std::vector<int> m_pTypes;

        // histogram output level
      int m_histoOutputLevel;

      // tools
      const ITrackSelector* m_trSelector;        ///< Track selector
      const IMCTruthTool* m_richRecMCTruth;
      const IParticleProperties* m_richPartProp; ///< Rich Particle properties
      const ICherenkovAngle* m_ckAngle;  ///< Pointer to RichCherenkovAngle tool
      const IIsolatedTrack * m_isoTrack; ///< Pointer to isolated track tool
      const IRefractiveIndex* m_refractiveIndex;
      const IGeomTool* m_geomTool;
      const ITrackEffectiveRefractiveIndex * m_tkIndex;
      IMuonInformation* m_MuonInformation;

    		//Isolation cuts for each radiator
      std::vector<double> m_IsoCut;

    		//Beta cuts
      std::vector<double> m_maxBetaCut;

      //Mass Deviation from mean cut
      std::vector<double> m_massDevCut;

      //CK Deviation from mean Cut
      std::vector<double> m_CKDevCut;

      //Minimum cut on CK angle
      std::vector<double> m_minCK;

      //Plot Per Photon
      bool m_plotPerPhoton;
      //whether to use Muon information
      bool m_useMuonInfo;

	  std::string m_tracksInContainer;
      mutable LHCb::MuonPID m_pid;
        
      // mcoombes event counter
      int EvtNum;
    //  IOnOffline* m_onOfflineTool;
   //   IOnOffline* m_OnOffline;

      int m_tkTotal;

      double m_avNPV;

      int m_PVSum;
      //std::string m_PVContainer;

    };

    }
  }
}


#endif // RICHPARTICLESEARCHMAIN_H

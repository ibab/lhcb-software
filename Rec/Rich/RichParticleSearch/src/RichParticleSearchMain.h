// $Id: $
#ifndef RICHPARTICLESEARCHMAIN_H
#define RICHPARTICLESEARCHMAIN_H 1

// Include files
// base class
#include "RichRecBase/RichRecTupleAlgBase.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/MCRichOpticalPhoton.h"

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
#include "RichRecBase/IRichTrackEffectiveRefractiveIndex.h"
#include "RichRecBase/IRichExpectedTrackSignal.h"

// RichDet
#include "RichDet/DeRichSphMirror.h"

// Kernel
#include "RichKernel/BoostArray.h"

// boost
#include "boost/lexical_cast.hpp"
#include <boost/foreach.hpp>
#include "boost/assign/list_of.hpp"

//My Interfaces

#include "IMuonInformation.h"

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
      class RichParticleSearchMain : public TupleAlgBase {
      public:

        /// Standard constructor
        RichParticleSearchMain( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~RichParticleSearchMain( ); ///< Destructor

        virtual StatusCode initialize();    ///< Algorithm initialization
        virtual StatusCode execute   ();    ///< Algorithm execution

        double MinimumTrackSeparation(LHCb::RichRecSegment*);
        int GetRichInt();

      private:

        int m_radTemp;    ///< which rich detector to monitor
        Rich::RadiatorType m_radiator;

        //std::vector<int> m_IsoCuts;
        int m_maxUsedTracks;

        // set to know to stop all MC Truth
        bool m_useMCTruth;

        // histogram output level
        int m_histoOutputLevel;

        // tools
        const ITrackSelector* m_trSelector;        ///< Track selector
        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;
        const IParticleProperties* m_richPartProp; ///< Rich Particle properties
        const ICherenkovAngle* m_ckAngle;  ///< Pointer to RichCherenkovAngle tool
        const IIsolatedTrack * m_isoTrack; ///< Pointer to isolated track tool
        const ITrackEffectiveRefractiveIndex * m_tkIndex;
        IMuonInformation* m_MuonInformation;
        const IExpectedTrackSignal * m_signal;
        //Isolation cuts for each radiator
        double m_IsoCut;

        // particle type when fixed
        int m_particleType;
        Rich::ParticleIDType m_pType;

        //Beta cuts
        double m_maxCKcut;

        //CK Deviation from mean Cut
        double m_CKDevCut;

        //Minimum cut on CK angle
        double m_minCK;

        double m_minTrackMass;
        //Plot Per Photon
        bool m_plotPerPhoton;
        //whether to use Muon information
        bool m_useMuonInfo;

        bool findYields;

        int m_maxPhotons;

        int m_minPhotons;

        //  event counter
        int EvtNum;

        int m_tkTotal;

       // friend class MattTest;
      };

      class MattTest
      {
      private:
    	double  m_AvTheta;
      public:
    	void SetAvTheta(double AvTheta)
    	{
    	    m_AvTheta = AvTheta;
    	}

      	bool operator() (int i,int j)
      	{
      		//return (i<j);
      		return (fabs(i-m_AvTheta)>fabs(j-m_AvTheta));
      	}
      }MattTestOb;


      inline int RichParticleSearchMain::GetRichInt(){
      return (int) m_radiator;
      }
    }
  }
}


#endif // RICHPARTICLESEARCHMAIN_H

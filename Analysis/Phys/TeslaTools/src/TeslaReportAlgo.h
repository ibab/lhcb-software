#ifndef TESLAREPORTALGO_H 
#define TESLAREPORTALGO_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include <algorithm>
#include <functional>
#include <TVector3.h>
#include <TTree.h>
#include <TFile.h>
//
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "Event/RelatedInfoMap.h"
//
#include <TLorentzVector.h>
#include "GaudiKernel/Vector4DTypes.h"
#include "LoKi/apply.h"
#include "LoKi/Filters.h"
#include "LoKi/Streamers.h"
#include "LoKi/Interface.h"
#include "LoKi/Services.h"
#include "Kernel/IDistanceCalculator.h"
#include "Kernel/Particle2Vertex.h"
#include "Kernel/IANNSvc.h"
#include "Kernel/IIndexedANNSvc.h"
//
#include "ReportCheckTool.h"
//
#include "HltDAQ/IReportConvert.h"
#include "HltDAQ/ReportConvertTool.h"

/** @class TeslaReportAlgo TeslaReportAlgo.h
 * 
 *
 */
class TeslaReportAlgo : public GaudiAlgorithm 
{

public: 

	// Standard constructor
  	TeslaReportAlgo( const std::string& name, ISvcLocator* pSvcLocator );

  	virtual ~TeslaReportAlgo( ); ///< Destructor

  	StatusCode initialize();    ///< Algorithm initialization
  	StatusCode execute   ();    ///< Algorithm execution

        StatusCode ProcessObject(int,LHCb::Particle*, const LHCb::HltObjectSummary*
            ,LHCb::RecVertex::Container*
            ,LHCb::Vertex::Container*
            ,LHCb::Particle::Container*
            ,LHCb::ProtoParticle::Container*
            ,LHCb::RichPID::Container*
            ,LHCb::MuonPID::Container*
            ,LHCb::Track::Container*
            ,LHCb::CaloHypo::Container*
            ,LHCb::CaloCluster::Container*
            ,Particle2Vertex::Table*);
        void fillParticleInfo(std::vector<ContainedObject*>,const LHCb::HltObjectSummary*,bool, LHCb::Track::Container*,std::vector<ContainedObject*>*,
            LHCb::CaloHypos*, LHCb::CaloClusters*);
        void fillVertexInfo(LHCb::Vertex*,const LHCb::HltObjectSummary*);
        void AddRelInfo(const LHCb::HltObjectSummary*,LHCb::Particle*);
        /// for splitting strings
        std::vector<std::string> &split(const std::string &, char, std::vector<std::string> &);
        std::vector<std::string> split(const std::string &, char);

        template <typename Iter, typename Cont>
        inline bool is_last(Iter iter, const Cont& cont)
        {
          return (iter != cont.end()) && (next(iter) == cont.end());
        }

        LHCb::CaloDigit* DigitSearchRaw(LHCb::CaloCellID);
        LHCb::RecVertex* bestPV(LHCb::Particle*, LHCb::RecVertex::Container*);
        LHCb::CaloCluster* processCluster(const LHCb::HltObjectSummary*, std::vector<ContainedObject*>*, LHCb::CaloClusters*);
        LHCb::CaloCluster* compareClusterPosition(LHCb::CaloCluster*, LHCb::CaloClusters*);
        LHCb::CaloHypo* compareHypoPosition(LHCb::CaloHypo*, LHCb::CaloHypos*);
protected:

private:
	std::string m_inputName;
	std::string m_PV;
	std::string m_PVLoc;
	std::string m_VertRepLoc;
	std::string m_OutputPref;
        std::map<int,std::string> m_RelInfoLocationsMap;
       
        bool turbo;
        bool m_PV3D;
        bool m_refitted;

        IDistanceCalculator *   m_dist;
        IReportCheck*   m_check;
        IReportConvert* m_conv;
        /// HltANNSvc for making selection names to int selection ID
        IANNSvc* m_hltANNSvc;
        IIndexedANNSvc* m_tckANNSvc;
        unsigned int m_lastTCK;
};
#endif // TESLAREPORTALGO_H

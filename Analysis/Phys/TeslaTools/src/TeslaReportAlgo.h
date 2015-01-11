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
 
        void fillParticleInfo(std::vector<ContainedObject*>,const LHCb::HltObjectSummary*,bool);
        void fillVertexInfo(LHCb::Vertex*,const LHCb::HltObjectSummary*);

        template <typename Iter, typename Cont>
        inline bool is_last(Iter iter, const Cont& cont)
        {
          return (iter != cont.end()) && (next(iter) == cont.end());
        }

protected:

private:
	std::string m_inputName;
	std::string m_PV;
	bool m_PreSplit;
	std::string m_OutputPref;
	int m_ReportVersion; // 1=OldSelReports, 2=Extended (GaudiMap)
        IDistanceCalculator *   m_dist;
        IReportCheck*   m_check;
        IReportConvert* m_conv;
};
#endif // TESLAREPORTALGO_H

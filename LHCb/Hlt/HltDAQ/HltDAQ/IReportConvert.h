#ifndef IREPORTCONVERT_H 
#define IREPORTCONVERT_H 1

#include <string>
#include "Event/HltObjectSummary.h"
#include "Event/HltSelReports.h"
#include "HltDAQ/HltSelRepRBStdInfo.h"
#include "GaudiKernel/IAlgTool.h"

// For LHCb objects
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
#include "Event/RecSummary.h"
#include "Event/RichPID.h"
#include "Event/MuonPID.h"
#include "Event/CaloCluster.h"
#include "Event/RecVertex.h"
#include "Event/Vertex.h"
#include "GaudiKernel/VectorMap.h"

using LHCb::HltObjectSummary;
using LHCb::HltSelRepRBStdInfo;

static const InterfaceID IID_IReportConvert("IReportConvert", 1 , 0);
class IReportConvert : virtual public IAlgTool { 
  
  public:
    virtual void setReportVersion(int) = 0;
    virtual void setReportVersionLatest() = 0;
    virtual int  getReportVersion() = 0;
    virtual int getLatestVersion() = 0;
    virtual int getSizeSelRepParticleLatest() = 0;
    
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IReportConvert; } // Actual operator function
    
    // Decode the sub-bank properly
    virtual void SummaryFromRaw( HltObjectSummary::Info*, HltSelRepRBStdInfo::StdInfo*, int ) = 0;
    //
    // Put the information in to the HltObjectSummary
    virtual void ParticleObject2Summary( HltObjectSummary::Info*, const LHCb::Particle*, bool) = 0;
    virtual void ProtoParticleObject2Summary( HltObjectSummary::Info*, const LHCb::ProtoParticle*, bool) = 0;
    virtual void TrackObject2Summary( HltObjectSummary::Info*, const LHCb::Track*, bool) = 0;
    virtual void RichPIDObject2Summary( HltObjectSummary::Info*, const LHCb::RichPID*, bool) = 0;
    virtual void MuonPIDObject2Summary( HltObjectSummary::Info*, const LHCb::MuonPID*, bool) = 0;
    virtual void CaloClusterObject2Summary( HltObjectSummary::Info*, const LHCb::CaloCluster*, bool) = 0;
    virtual void CaloHypoObject2Summary( HltObjectSummary::Info*, const LHCb::CaloHypo*, bool) = 0;
    virtual void RecVertexObject2Summary( HltObjectSummary::Info*, const LHCb::RecVertex*, bool) = 0;
    virtual void VertexObject2Summary( HltObjectSummary::Info*, const LHCb::Vertex*, bool) = 0;
    virtual void RecSummaryObject2Summary( HltObjectSummary::Info*, const LHCb::RecSummary*) = 0;
    virtual void GenericMapObject2Summary( HltObjectSummary::Info*, const GaudiUtils::VectorMap<short,float>*) = 0;
    //
    // Put the information in the summary back in the object
    virtual void ParticleObjectFromSummary( const HltObjectSummary::Info*, LHCb::Particle*, bool) = 0;
    virtual void ProtoParticleObjectFromSummary( const HltObjectSummary::Info*, LHCb::ProtoParticle*, bool) = 0;
    virtual void TrackObjectFromSummary( const HltObjectSummary::Info*, LHCb::Track*, bool) = 0;
    virtual void RichPIDObjectFromSummary( const HltObjectSummary::Info*, LHCb::RichPID*, bool) = 0;
    virtual void MuonPIDObjectFromSummary( const HltObjectSummary::Info*, LHCb::MuonPID*, bool) = 0;
    virtual void CaloClusterObjectFromSummary( const HltObjectSummary::Info*, LHCb::CaloCluster*, bool) = 0;
    virtual void CaloHypoObjectFromSummary( const HltObjectSummary::Info*, LHCb::CaloHypo*, bool) = 0;
    virtual void RecVertexObjectFromSummary( const HltObjectSummary::Info*, LHCb::RecVertex*, bool) = 0;
    virtual void VertexObjectFromSummary( const HltObjectSummary::Info*, LHCb::Vertex*, bool) = 0;
    virtual void RecSummaryObjectFromSummary( const HltObjectSummary::Info*, LHCb::RecSummary*) = 0;
    virtual void GenericMapObjectFromSummary( const HltObjectSummary::Info*, GaudiUtils::VectorMap<short,float>*) = 0;

};

#endif

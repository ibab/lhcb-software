#include "GaudiKernel/IRegistry.h"

#include <cmath>
#include <cassert>
#include <limits>
#include <exception>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/type_traits.hpp>

#include "TrackExpectedHitsXYZTool.h"

#include "Math/CholeskyDecomp.h"

#include "Kernel/ITrajPoca.h"
#include "Kernel/LineTraj.h"
#include "TrackKernel/TrackTraj.h"
#include "Kernel/LHCbID.h"
#include "Event/State.h"
#include "Event/Track.h"
#include "Event/Measurement.h"
#include "Event/Node.h"
#include "Event/FitNode.h"
#include "PatKernel/PatForwardHit.h"
#include "PatKernel/PatTTHit.h"
#include "TfKernel/TStationHitManager.h"
#include "TfKernel/TTStationHitManager.h"
#include "TfKernel/DefaultVeloRHitManager.h"
#include "TfKernel/DefaultVeloPhiHitManager.h"
#include "TfKernel/RecoFuncs.h"
#include "TfKernel/RegionID.h"
#include "TfKernel/RegionSelectors.h"

/// exception to throw when collection fails for some reason
class CollectionFailure : public std::exception
{
    public:
	CollectionFailure(const char* msg) : m_msg(msg)
    { }

	const char* what() const throw () { return m_msg; }

    private:
	const char* m_msg;
};


//-----------------------------------------------------------------------------
// Implementation file for class : TrackExpectedHitsXYZTool
//
// 2010-06-11 : Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
// 2010-07-02 : Georg Alexander Krocker <krocker@physi.uni-heidelberg.de>
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackExpectedHitsXYZTool )

/// default constructor for HitPatternXYZ struct
TrackExpectedHitsXYZTool::HitPatternXYZ::HitPatternXYZ()
{
    const Gaudi::XYZVector nanv(
	    std::numeric_limits<double>::quiet_NaN(),
	    std::numeric_limits<double>::quiet_NaN(),
	    std::numeric_limits<double>::quiet_NaN());
    std::fill(VeloRA.begin(), VeloRA.end(), nanv);
    std::fill(VeloRC.begin(), VeloRC.end(), nanv);
    std::fill(VeloPhiA.begin(), VeloPhiA.end(), nanv);
    std::fill(VeloPhiC.begin(), VeloPhiC.end(), nanv);
    std::fill(TT.begin(), TT.end(), nanv);
    std::fill(ITAC.begin(), ITAC.end(), nanv);
    std::fill(ITTopBot.begin(), ITTopBot.end(), nanv);
    std::fill(OT.begin(), OT.end(), nanv);
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackExpectedHitsXYZTool::TrackExpectedHitsXYZTool(const std::string& type,
	const std::string& name,
	const IInterface* parent) : GaudiTool(type, name, parent),
    m_hitManagerT(0), m_hitManagerTT(0), m_hitManagerVeloR(0),
    m_hitManagerVeloPhi(0), m_trajPoca(0)
{
    declareInterface<TrackExpectedHitsXYZTool>(this);
    // rough preselection of hits in the vincinity of the track
    // T and TT use TolX and TolY
    declareProperty("TolX", m_tolX = 3.0 * Gaudi::Units::cm);
    declareProperty("TolY", m_tolY = 15.0 * Gaudi::Units::cm);
    // blow up track uncertainties by nSigma
    declareProperty("nSigma", m_nSigma = 5.);
}

//=============================================================================
// Destructor
//=============================================================================
TrackExpectedHitsXYZTool::~TrackExpectedHitsXYZTool() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackExpectedHitsXYZTool::initialize()
{
    StatusCode sc = GaudiTool::initialize(); // must be executed first
    if (sc.isFailure()) return sc;  // error printed already by GaudiTool

    if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

    m_hitManagerT = tool<Tf::TStationHitManager<PatForwardHit> >(
	    "PatTStationHitManager");
    m_hitManagerTT = tool<Tf::TTStationHitManager <PatTTHit> >(
	    "PatTTStationHitManager");
    m_hitManagerVeloR = tool<Tf::DefaultVeloRHitManager>(
	    "Tf::DefaultVeloRHitManager");
    m_hitManagerVeloPhi = tool<Tf::DefaultVeloPhiHitManager>(
	    "Tf::DefaultVeloPhiHitManager");
    m_trajPoca = tool<ITrajPoca>("TrajPoca");

    assert(m_tolX > 0.);
    assert(m_tolY > 0.);
    
    assert(m_nSigma > 0.);
    return StatusCode::SUCCESS;
}

//=============================================================================
// we'll need a few helpers to convert triples (reg, sta, lay) into
// consecutive layer numbers and select the right field in HitPatternXYZ

/// helper for indexing IT/OT in HitPattern
struct ITOTIndexHelper
{
    TrackExpectedHitsXYZTool::HitPatternXYZ& m_pat;
    
    ITOTIndexHelper(TrackExpectedHitsXYZTool::HitPatternXYZ& pat) : m_pat(pat) { }

    Gaudi::XYZVector& operator()(unsigned reg, unsigned sta, unsigned lay) const
    {
	if (reg < 2) return m_pat.OT[4 * sta + lay];
	return (reg < 4)?m_pat.ITAC[4 * sta + lay]:m_pat.ITTopBot[4 * sta + lay];
    }
};

/// helper for indexing TT in HitPattern
struct TTIndexHelper
{
    TrackExpectedHitsXYZTool::HitPatternXYZ& m_pat;
    
    TTIndexHelper(TrackExpectedHitsXYZTool::HitPatternXYZ& pat) : m_pat(pat) { }

    Gaudi::XYZVector& operator()(unsigned /*reg*/, unsigned sta, unsigned lay) const
    { return m_pat.TT[2 * sta + lay]; }
};

/// helper for indexing Velo in HitPattern
struct VeloIndexHelper
{
    TrackExpectedHitsXYZTool::HitPatternXYZ& m_pat;
    
    VeloIndexHelper(TrackExpectedHitsXYZTool::HitPatternXYZ& pat) : m_pat(pat) { }

    Gaudi::XYZVector& operator()(unsigned side, unsigned sta, unsigned type) const
    { 
	sta += 2*(1-type/2) ;
	type     = type%2 ;

	switch( side+2*type ) {
	    case 0:
		return m_pat.VeloRA[sta] ;
	    case 1:
		return m_pat.VeloRC[sta] ;
	    case 2:
		return m_pat.VeloPhiA[sta] ;
	    default:
		return m_pat.VeloPhiC[sta] ;			   
	}
    }	
};

StatusCode TrackExpectedHitsXYZTool::execute(
	const LHCb::Track& tr, HitPatternXYZ& pat,
	bool collectVelo, bool collectTT, bool collectIT,
	bool collectOT, bool collectMuon)
{
    try {
	LHCb::TrackTraj ttraj(tr);
	if (collectVelo) {
	    this->collectVeloHits<HitManVeloR, VeloIndexHelper>(
		    m_hitManagerVeloR, ttraj, pat);
	    this->collectVeloHits<HitManVeloPhi, VeloIndexHelper>(
		    m_hitManagerVeloPhi, ttraj, pat);
	}
	if (collectTT)
	    collectLineHits<HitManTT, TTIndexHelper,
		Tf::RegionID::TTIndex::kMinRegion,
		Tf::RegionID::TTIndex::kMaxRegion,
		Tf::RegionID::TTIndex::kMinStation,
		Tf::RegionID::TTIndex::kMaxStation,
		Tf::RegionID::TTIndex::MinLayer,
		Tf::RegionID::TTIndex::kMaxLayer>(m_hitManagerTT, ttraj, pat);
	if (collectOT)
	    collectLineHits<HitManT, ITOTIndexHelper,
		Tf::RegionID::OTIndex::kMinRegion,
		Tf::RegionID::OTIndex::kMaxRegion,
		Tf::RegionID::OTIndex::kMinStation,
		Tf::RegionID::OTIndex::kMaxStation,
		Tf::RegionID::OTIndex::kMinLayer,
		Tf::RegionID::OTIndex::kMaxLayer>(m_hitManagerT, ttraj, pat);
	if (collectIT)
	    collectLineHits<HitManT, ITOTIndexHelper,
		Tf::RegionID::OTIndex::kNRegions + Tf::RegionID::ITIndex::kMinRegion,
		Tf::RegionID::OTIndex::kNRegions + Tf::RegionID::ITIndex::kMaxRegion,
		Tf::RegionID::OTIndex::kMinStation,
		Tf::RegionID::OTIndex::kMaxStation,
		Tf::RegionID::OTIndex::kMinLayer,
		Tf::RegionID::OTIndex::kMaxLayer>(m_hitManagerT, ttraj, pat);
	if (collectMuon) {
	    ; // not implemented yet
	}
    } catch (CollectionFailure f) {
	return Warning( f.what(), StatusCode::FAILURE, 0 ); 
    }
    return StatusCode::SUCCESS;
}

template<typename HITMANAGER, typename IndexHelper,
    unsigned regMin, unsigned regMax,
    unsigned staMin, unsigned staMax,
    unsigned layMin, unsigned layMax>
void TrackExpectedHitsXYZTool::collectLineHits(HITMANAGER* hitman,
	const LHCb::TrackTraj& ttraj, HitPatternXYZ& pat) const
{
    IndexHelper idx(pat);
    hitman->prepareHits();
    for (unsigned reg = regMin; regMax >= reg; ++reg) {
	for (unsigned sta = staMin; staMax >= sta; ++sta) {
	    for (unsigned lay = layMin; layMax >= lay; ++lay) {
		const Tf::EnvelopeBase& region = *(hitman->region(sta, lay, reg));
		LHCb::State st(ttraj.state(region.z()));
		const double xtrerr = m_nSigma * std::sqrt(st.errX2());
		const double ytrerr = m_nSigma * std::sqrt(st.errY2());
		// skip region if track can not possibly be inside
		if (!region.isXCompatible(st.x(), m_tolX + xtrerr) ||
			    !region.isYCompatible(st.y(), m_tolY + ytrerr))
		    continue;
		const Gaudi::XYZVector preg(region.x(), region.y(), region.z());
		const Gaudi::XYZVector nreg(region.vz());
		double z = region.z(), dz = 0.;
		unsigned iter = 0;
		do {
		    const LHCb::State st(ttraj.state(z));
		    Gaudi::XYZVector ptra(st.x(), st.y(), z);
		    Gaudi::XYZVector vtra(st.tx(), st.ty(), 1.0);
		    dz = (ptra - preg).Dot(nreg) / vtra.Dot(nreg);
		    z += dz;
		    if (++iter > 42) throw CollectionFailure("No convergence");
		} while (std::abs(dz) < 1e-3);
		st = ttraj.state(z);
		idx(reg, sta, lay) = Gaudi::XYZVector(st.x(), st.y(), z);
	    }
	}
    }
}

/// helpers for TrackExpectedHitsXYZTool
namespace TrackExpectedHitsXYZToolHelpers {
    /// traits class exposing correct sensor/hit types for Velo R/Phi hit managers
    template<typename T> struct VeloTraits;
    template<> struct VeloTraits< ::TrackExpectedHitsXYZTool::HitManVeloR> {
	typedef DeVeloRType Sensor;
	typedef Tf::VeloRHit Hit;
    };
    template<> struct VeloTraits< ::TrackExpectedHitsXYZTool::HitManVeloPhi> {
	typedef DeVeloPhiType Sensor;
	typedef Tf::VeloPhiHit Hit;
    };
}


template<typename HITMANAGER, typename IndexHelper>
void TrackExpectedHitsXYZTool::collectVeloHits(HITMANAGER* hitman,
	const LHCb::TrackTraj& ttraj, HitPatternXYZ& pat) const
{
    using namespace TrackExpectedHitsXYZToolHelpers;
    hitman->prepareHits();
    typename HITMANAGER::StationIterator
	ibeg = hitman->stationsAllBegin(),
	iend = hitman->stationsAllEnd();
    IndexHelper idx(pat);
    for ( ; ibeg < iend; ++ibeg) {
	typename HITMANAGER::Station* station = *ibeg;
	const typename VeloTraits<HITMANAGER>::Sensor* sensor = station->sensor();
	//Calculate station, side, type according to the funny Velo numbering
	//scheme. Needed to put xy positions correctly in pattern.	
	unsigned int sta = (station->sensorNumber()%64)/2 ;
	unsigned int side    = (station->sensorNumber()%2) ;
	unsigned int type    = (station->sensorNumber()/64) ;

	double z = station->z();
	const LHCb::State st(ttraj.state(z));
	Gaudi::XYZPoint p(st.x(), st.y(), z);
	Gaudi::XYZPoint hit(0., 0., 0.);
	Gaudi::XYZVector vdir(st.tx(), st.ty(), 1.0);

	//Calculates the intersection point of track and sensor taking
	//alignement into account	
	StatusCode sc = ((DeVeloSensor*)sensor)->intersectWithLine(p, vdir, hit);
	if (sc.isFailure()) {
		error() << "Could not calculate intesecting point in Velo";  // error printed already by GaudiTool
	}
	 

	idx(side, sta, type) = Gaudi::XYZVector(hit.x(), hit.y(), hit.z());

    }
}

// vim: sw=4:tw=78:ft=cpp

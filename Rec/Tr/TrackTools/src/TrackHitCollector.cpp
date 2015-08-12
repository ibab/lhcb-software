#include "GaudiKernel/IRegistry.h"

#include <cmath>
#include <cassert>
#include <exception>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/type_traits.hpp>

#include "TrackHitCollector.h"

#include "Math/CholeskyDecomp.h"

#include "Kernel/ITrajPoca.h"
#include "Kernel/LineTraj.h"
#include "TrackKernel/TrackTraj.h"
#include "Kernel/LHCbID.h"
#include "Event/State.h"
#include "Event/Track.h"
#include "Event/Measurement.h"
#include "Event/TrackFitResult.h"
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
// Implementation file for class : TrackHitCollector
//
// 2010-01-12 : Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackHitCollector )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TrackHitCollector::TrackHitCollector(const std::string& type,
	const std::string& name,
	const IInterface* parent) : GaudiTool(type, name, parent),
    m_hitManagerT(0), m_hitManagerTT(0), m_hitManagerVeloR(0),
    m_hitManagerVeloPhi(0), m_trajPoca(0)
{
    declareInterface<ITrackHitCollector>(this);
    // rough preselection of hits in the vincinity of the track
    // T and TT use TolX and TolY
    declareProperty("TolX", m_tolX = 3.0 * Gaudi::Units::cm);
    declareProperty("TolY", m_tolY = 15.0 * Gaudi::Units::cm);
    // Velo uses TolR and TolPhi
    declareProperty("TolR", m_tolR = 0.5 * Gaudi::Units::cm);
    declareProperty("TolPhi", m_tolPhi = 0.05);
    // blow up track uncertainties by nSigma
    declareProperty("nSigma", m_nSigma = 5.);
    // final cuts happen on residual in each subdetector
    declareProperty("MaxDistOT", m_maxDistOT = 1.0 * Gaudi::Units::cm);
    declareProperty("MaxDistIT", m_maxDistIT = 1.0 * Gaudi::Units::mm);
    declareProperty("MaxDistTT", m_maxDistTT = 1.5 * Gaudi::Units::cm);
    declareProperty("MaxDistVelo", m_maxDistVelo = 1.0 * Gaudi::Units::mm);
}

//=============================================================================
// Destructor
//=============================================================================
TrackHitCollector::~TrackHitCollector() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode TrackHitCollector::initialize()
{
    StatusCode sc = GaudiTool::initialize(); // must be executed first
    if (sc.isFailure()) return sc;  // error printed already by GaudiTool

    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

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
    assert(m_tolR > 0.);
    assert(m_tolPhi > 0.);
    assert(m_nSigma > 0.);
    assert(m_maxDistOT > 0.);
    assert(m_maxDistIT > 0.);
    assert(m_maxDistTT > 0.);
    assert(m_maxDistVelo > 0.);

    return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode TrackHitCollector::execute(
	const LHCb::Track& tr, std::vector<IDWithResidual>& ids,
	bool collectVelo, bool collectTT, bool collectIT,
	bool collectOT, bool collectMuon)
{
    try {
	LHCb::TrackTraj ttraj(tr);
	// reserve ample space for collected hits
	ids.reserve(3 * tr.lhcbIDs().size());
	if (collectVelo) {
	    this->collectVeloHits<HitManVeloR>(
		    m_hitManagerVeloR, ttraj, ids);
	    this->collectVeloHits<HitManVeloPhi>(
		    m_hitManagerVeloPhi, ttraj, ids);
	}
	if (collectTT)
	    collectLineHits<HitManTT,
		Tf::RegionID::TTIndex::kMinRegion,
		Tf::RegionID::TTIndex::kMaxRegion,
		Tf::RegionID::TTIndex::kMinStation,
		Tf::RegionID::TTIndex::kMaxStation,
		Tf::RegionID::TTIndex::MinLayer,
		Tf::RegionID::TTIndex::kMaxLayer>(m_hitManagerTT, ttraj, ids);
	if (collectOT)
	    collectLineHits<HitManT,
		Tf::RegionID::OTIndex::kMinRegion,
		Tf::RegionID::OTIndex::kMaxRegion,
		Tf::RegionID::OTIndex::kMinStation,
		Tf::RegionID::OTIndex::kMaxStation,
		Tf::RegionID::OTIndex::kMinLayer,
		Tf::RegionID::OTIndex::kMaxLayer>(m_hitManagerT, ttraj, ids);
	if (collectIT)
	    collectLineHits<HitManT,
		Tf::RegionID::OTIndex::kNRegions + Tf::RegionID::ITIndex::kMinRegion,
		Tf::RegionID::OTIndex::kNRegions + Tf::RegionID::ITIndex::kMaxRegion,
		Tf::RegionID::OTIndex::kMinStation,
		Tf::RegionID::OTIndex::kMaxStation,
		Tf::RegionID::OTIndex::kMinLayer,
		Tf::RegionID::OTIndex::kMaxLayer>(m_hitManagerT, ttraj, ids);
	if (collectMuon) {
	    ; // not implemented yet
	}
    } catch (CollectionFailure f) {
	info() << f.what() << endmsg;
	return StatusCode::FAILURE;
    }
    updateWithProperResiduals(ids, tr);
    return StatusCode::SUCCESS;
}

template<typename HITMANAGER,
    unsigned regMin, unsigned regMax,
    unsigned staMin, unsigned staMax,
    unsigned layMin, unsigned layMax>
void TrackHitCollector::collectLineHits(HITMANAGER* hitman,
	const LHCb::TrackTraj& ttraj, std::vector<IDWithResidual>& ids) const
{
    typedef typename HITMANAGER::Hits::value_type HITP;
    typedef typename boost::remove_pointer<HITP>::type HIT;
    typedef Tf::increasingByXAtYEq0<HIT> SORTER;
    hitman->prepareHits();
    for (unsigned reg = regMin; regMax >= reg; ++reg) {
	for (unsigned sta = staMin; staMax >= sta; ++sta) {
	    for (unsigned lay = layMin; layMax >= lay; ++lay) {
		const Tf::EnvelopeBase& region = *(hitman->region(sta, lay, reg));
		const LHCb::State st(ttraj.state(region.z()));
		const double xtrerr = m_nSigma * std::sqrt(st.errX2());
		const double ytrerr = m_nSigma * std::sqrt(st.errY2());
		// skip region if track can not possibly be inside
		if (!region.isXCompatible(st.x(), m_tolX + xtrerr) ||
			    !region.isYCompatible(st.y(), m_tolY + ytrerr))
		    continue;
		const double regdxdy = -region.sinT() / region.cosT();
		const std::pair<double, double> y(st.y() - ytrerr - m_tolY,
			st.y() + ytrerr + m_tolY);
		// x range: adjust for stereo angle to give x at y = 0
		std::pair<double, double> x(st.x() - regdxdy * y.first,
			st.x() - regdxdy * y.second);
		if (x.first > x.second) std::swap(x.first, x.second);
		x.first -= m_tolX + xtrerr;
		x.second += m_tolX + xtrerr;
		// the next call is *very* important for hitsWithMinX
		// below to work properly...
		hitman->template sortHits < SORTER > (sta, lay, reg);
		BOOST_FOREACH(const HITP hit,
			hitman->hitsWithMinX(x.first, sta, lay, reg)) {
		    const Tf::LineHit& lhit = *(hit->hit());
		    if (lhit.xAtYEq0() > x.second) break;
		    if (!lhit.isYCompatible(st.y(), ytrerr + m_tolY)) continue;
		    double resid, err;
		    if (!computeResidAndErr(hit, ttraj, st, resid, err)) continue;
		    ids.push_back(IDWithResidual(lhit.lhcbID(), resid, err));
		}
	    }
	}
    }
}

bool TrackHitCollector::computeResidAndErr(
	const PatForwardHit* hit, const LHCb::TrackTraj& ttraj,
	const LHCb::State& st, double& resid, double& err) const
{
    const Tf::LineHit& lhit = *(hit->hit());
    // ok, hit is in the proximity of track, so get POCA of
    // track and hit
    Gaudi::XYZVector doca;
    docaInTandTT(ttraj, lhit, st.y(), st.z(), doca);
    // estimate variance due to track position uncertainty
    // in measurement direction
    const double cosT = lhit.cosT();
    const double sinT = lhit.sinT();
    const double trvar = st.errX2() * cosT * cosT +
	st.errY2() * sinT * sinT;
    const double var = lhit.variance() + trvar;
    err = std::sqrt(var);

    resid = doca.R();
    // for OT, subtract drift radius
    const bool isOT = 0 != lhit.othit();
    if (isOT) resid -= hit->driftDistance();
    // make sure that residual is signed (vector from track to hit
    // pointing in positive x direction)
    if (doca.x() >= 0.) resid = -resid;
    
    return std::abs(resid) < (isOT?m_maxDistOT:m_maxDistIT);
}

bool TrackHitCollector::computeResidAndErr(
	const PatTTHit* hit, const LHCb::TrackTraj& ttraj,
	const LHCb::State& st, double& resid, double& err) const
{
    const Tf::LineHit& lhit = *(hit->hit());
    // ok, hit is in the proximity of track, so get POCA of
    // track and hit
    Gaudi::XYZVector doca;
    docaInTandTT(ttraj, lhit, st.y(), st.z(), doca);
    // estimate variance due to track position uncertainty
    // in measurement direction
    const double cosT = lhit.cosT();
    const double sinT = lhit.sinT();
    const double trvar = st.errX2() * cosT * cosT +
	st.errY2() * sinT * sinT;
    const double var = lhit.variance() + trvar;
    err = std::sqrt(var);

    resid = doca.R();
    // make sure that residual is signed (vector from track to hit
    // pointing in positive x direction)
    if (doca.x() >= 0.) resid = -resid;
    
    return std::abs(resid) < m_maxDistTT;
}

void TrackHitCollector::docaInTandTT(const LHCb::TrackTraj& ttraj,
	const Tf::LineHit& lhit, const double refy, const double refz,
	Gaudi::XYZVector& doca) const
{
    double lambda = refz, mu = refy;
    unsigned iter = 0;
    Gaudi::XYZVector d1(ttraj.direction(lambda).Unit());
    const Gaudi::XYZVector d2(lhit.direction().Unit());
    double mat[3], rhs[2];
    rhs[0] = rhs[1] = 0.;
    do {
	doca = ttraj.position(lambda) - Gaudi::XYZVector(lhit.position(mu));
	mat[0] = 1.;
	mat[1] = -(d1.Dot(d2));
	mat[2] = 1.;
	rhs[0] = -(doca.Dot(d1));
	rhs[1] = +(doca.Dot(d2));

	ROOT::Math::CholeskyDecomp<double, 2> decomp(mat);
	if (!decomp)
	    throw CollectionFailure("Singular matrix in T/TT station DOCA!");
	decomp.Solve(rhs);
	lambda += rhs[0];
	mu += rhs[1];
	if (++iter > 42)
	    throw CollectionFailure("No convergence in T/TT station DOCA!");
	d1 = ttraj.direction(lambda).Unit();
    } while (std::abs(rhs[0]) > 1e-3 && std::abs(rhs[1]) > 1e-3);
    doca = ttraj.position(lambda) - Gaudi::XYZVector(lhit.position(mu));
}

bool TrackHitCollector::phiRangesOverlap(const std::pair<double, double>& r1,
	const std::pair<double, double>& r2) const
{
    // this is extremely ugly code (and quite possibly suboptimal),
    // but it should work well...
    //
    // use units of 2 pi in order to avoid writing 2 pi in code
    double r1a = .5 * r1.first / M_PI, r1b = .5 * r1.second / M_PI;
    double r2a = .5 * r2.first / M_PI, r2b = .5 * r2.second / M_PI;
    // normalise to -pi to pi range
    while (r1a <= -0.5) r1a += 1.;
    while (r1a > 0.5) r1a -= 1.;
    while (r1b <= -0.5) r1b += 1.;
    while (r1b > 0.5) r1b -= 1.;
    while (r2a <= -0.5) r2a += 1.;
    while (r2a > 0.5) r2a -= 1.;
    while (r2b <= -0.5) r2b += 1.;
    while (r2b > 0.5) r2b -= 1.;
    // adjust if r1 covers the 0 to 2pi jump
    while (r1a > r1b) r1b += 1.;
    // adjust if r2 covers the 0 to 2pi jump
    while (r2a > r2b) r2b += 1.;
    // if the ranges do not overlap because we adjusted one
    // but not the other, correct
    if (r2a < r1a && r2b < r1a)
	r2a += 1., r2b += 1.;
    if (r1a < r2a && r1b < r2a)
	r1a += 1., r1b += 1.;
    // check for overlap
    if (r2b < r1a || r1b < r2a) return false;
    else return true;
}

/// helpers for TrackHitCollector
namespace TrackHitCollectorHelpers {
    /// traits class exposing correct sensor/hit types for Velo R/Phi hit managers
    template<typename T> struct VeloTraits;
    template<> struct VeloTraits< ::TrackHitCollector::HitManVeloR> {
	typedef DeVeloRType Sensor;
	typedef Tf::VeloRHit Hit;
    };
    template<> struct VeloTraits< ::TrackHitCollector::HitManVeloPhi> {
	typedef DeVeloPhiType Sensor;
	typedef Tf::VeloPhiHit Hit;
    };
}

bool TrackHitCollector::computeResidAndError(
	const Tf::VeloRHit* hit,
	std::auto_ptr<LHCb::Trajectory> htraj,
	const LHCb::TrackTraj& ttraj,
	const Gaudi::XYZPoint& ptrack,
	const DeVeloRType* sensor,
	const double trerr2, const double,
	double& resid, double& err) const
{
    // minimize doca
    double mu1 = ptrack.z(), mu2 = htraj->muEstimate(ptrack);
    Gaudi::XYZVector doca;
    StatusCode sc(m_trajPoca->minimize(
		ttraj, mu1, true, *htraj, mu2, true, doca, 0.0001));
    if (sc.isFailure())
	throw CollectionFailure("Failed to minimize DOCA in VeloR collection!");
    // get signed distance
    const Gaudi::XYZPoint pt = sensor->globalToVeloHalfBox(ttraj.position(mu1));
    const double phi = pt.Phi();
    const Gaudi::XYZVector dirR(std::cos(phi), std::sin(phi), 0);
    resid = doca.R() * ((doca.Dot(dirR) > 0.)?1.:-1.);
    err = std::sqrt(1. / hit->weight() + trerr2);
    return std::abs(resid) < m_maxDistVelo;
}

bool TrackHitCollector::computeResidAndError(
	const Tf::VeloPhiHit* hit,
	std::auto_ptr<LHCb::Trajectory> htraj,
	const LHCb::TrackTraj& ttraj,
	const Gaudi::XYZPoint& ptrack,
	const DeVeloPhiType* sensor,
	const double, const double trerrphi2,
	double& resid, double& err) const
{
    // minimize doca
    double mu1 = ptrack.z(), mu2 = htraj->muEstimate(ptrack);
    Gaudi::XYZVector doca;
    StatusCode sc(m_trajPoca->minimize(
		ttraj, mu1, true, *htraj, mu2, true, doca, 0.0001));
    if (sc.isFailure())
	throw CollectionFailure("Failed to minimize DOCA in VeloPhi collection!");
    // get signed distance
    const Gaudi::XYZPoint pt = sensor->globalToVeloHalfBox(ttraj.position(mu1));
    const double phi = pt.Phi();
    const Gaudi::XYZVector dirPhi(-std::sin(phi), std::cos(phi), 0);
    resid = doca.R() * ((doca.Dot(dirPhi) > 0.)?1.:-1.);
    err = std::sqrt(1. / hit->weight() + trerrphi2);
    return std::abs(resid) < m_maxDistVelo;
}

std::pair<double, double> TrackHitCollector::selectVeloCoordinates(
	const DeVeloRType*, const int, const double,
	const double rmin, const double rmax,
	const double, const double) const
{
    return std::make_pair(rmin, rmax);
}

std::pair<double, double> TrackHitCollector::selectVeloCoordinates(
	const DeVeloPhiType* sensor, const int iZone, const double r,
	const double, const double,
	const double phimin, const double phimax) const
{
    const double offs = - sensor->halfboxPhiOffset(iZone, r);
    return std::make_pair(phimin + offs, phimax + offs);
}

template<typename HITMANAGER>
void TrackHitCollector::collectVeloHits(HITMANAGER* hitman,
	const LHCb::TrackTraj& ttraj, std::vector<IDWithResidual>& ids) const
{
    using namespace TrackHitCollectorHelpers;
    hitman->prepareHits();
    typename HITMANAGER::StationIterator
	ibeg = hitman->stationsAllBegin(),
	iend = hitman->stationsAllEnd();
    for ( ; ibeg < iend; ++ibeg) {
	typename HITMANAGER::Station* station = *ibeg;
	const double z = station->z();
	const typename VeloTraits<HITMANAGER>::Sensor* sensor = station->sensor();
	LHCb::State st(ttraj.state(z));
	// transform into halfbox coordinates
	Gaudi::XYZPoint p(st.x(), st.y(), z);
	Gaudi::XYZPoint hbp = sensor->globalToVeloHalfBox(p);
	// (rough) estimate of track uncertainty
	// (idea is that x and y errors in global and half box frame are
	// roughly the same)
	const double r2 = hbp.Perp2();
	const double r = std::sqrt(r2);
	const double cosphi2 = hbp.x()*hbp.x() / r2;
	const double sinphi2 = hbp.y()*hbp.y() / r2;
	const double phi = hbp.Phi();
	const double trerr2 = st.errX2() * cosphi2 + st.errY2() * sinphi2;
	const double trerr = std::sqrt(trerr2);
	const double trerrphi2 = (st.errX2() * sinphi2 + st.errY2() * cosphi2) / r2;
	const double trerrphi = std::sqrt(trerrphi2);
	const double rmin = std::min(r - m_tolR - m_nSigma * trerr, 0.);
	const double rmax = r + m_tolR + m_nSigma * trerr;
	// loop over hits in range
	for (unsigned iZone = 0; station->nZones() > iZone; ++iZone) {
	    if (rmax < sensor->halfboxRRange(iZone).first ||
		    rmin > sensor->halfboxRRange(iZone).second) continue;
	    // get range in phi in half box frame
	    const double hbphi = (sensor->isRight() && phi < 0.) ?
		(phi + 2. * M_PI) : phi;
	    const double hbphimin = hbphi - m_tolPhi - m_nSigma * trerrphi;
	    const double hbphimax = hbphi + m_tolPhi + m_nSigma * trerrphi;
	    if (!phiRangesOverlap(std::make_pair(hbphimin, hbphimax),
			sensor->halfboxPhiRange(iZone)))
		continue;
	    std::pair<double, double> coordrange(
		    selectVeloCoordinates(sensor, iZone, r, rmin, rmax, hbphimin, hbphimax));
	    BOOST_FOREACH(const typename VeloTraits<HITMANAGER>::Hit* hit,
		    station->hitsHalfBox(iZone, coordrange.first, coordrange.second)) {
		double resid, err;
		// get trajectory
		if (!computeResidAndError(
			    hit, sensor->trajectory(hit->channelID(), 0.),
			    ttraj, p, sensor, trerr2, trerrphi2 * r2, resid, err))
		    continue;
		ids.push_back(
			IDWithResidual(LHCb::LHCbID(hit->channelID()), resid, err));
	    }
	}
    }
}

void TrackHitCollector::updateWithProperResiduals(
	std::vector<IDWithResidual>& ids, const LHCb::Track& tr) const
{
    if (ids.empty()) return;
    const LHCb::TrackFitResult* fr = tr.fitResult() ;
    if( !fr || fr->nodes().empty()) return ;
    typedef LHCb::TrackFitResult::NodeContainer Nodes ;
    Nodes nodes = fr->nodes();
    if (nodes.begin() == nodes.end()) return;
    BOOST_FOREACH(IDWithResidual& idwr, ids) {
	/// check if we have a node for given LHCbID
	Nodes::const_iterator nit = nodes.begin(), nend = nodes.end();
	for (; nit != nend; ++nit) {
	    // only consider hits on track or outliers
	    if (LHCb::Node::HitOnTrack != (*nit)->type() &&
		    LHCb::Node::Outlier != (*nit)->type())
		continue;
	    if ((*nit)->measurement().lhcbID() == idwr.m_id) break;
	}
	// if not, we're done
	if (nit == nend) continue;
	const LHCb::Node* np = *nit;
	if (np->type() == LHCb::Node::HitOnTrack) {
	    const LHCb::FitNode* fnp = static_cast<const LHCb::FitNode*>(np);
	    idwr.m_res = fnp->unbiasedResidual();
	    idwr.m_reserr = fnp->errUnbiasedResidual();
	} else {
	    idwr.m_res = np->residual();
	    idwr.m_reserr = np->errResidual();
	}
    }
}

// vim: sw=4:tw=78:ft=cpp

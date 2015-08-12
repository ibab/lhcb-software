#include "HitEffPlotter.h"

#include <limits>
#include <boost/foreach.hpp>

#include "Event/Track.h"
#include "Kernel/HitPattern.h"

#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/IVeloExpectation.h"

#include "TrackExpectedHitsXYZTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HitEffPlotter
//
// 2010-06-10 : Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( HitEffPlotter )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
    HitEffPlotter::HitEffPlotter(const std::string& name, ISvcLocator* pSvcLocator)
      : GaudiHistoAlg(name, pSvcLocator), m_veloExpectation(0),
        m_ttExpectation(0), m_itExpectation(0), m_otExpectation(0),
        m_xyzExpectation(0)
{
    declareProperty("InputCollection", m_inputCollection = LHCb::TrackLocation::Default);
    declareProperty("UseUT"       , m_useUT =  false );
}

//=============================================================================
// Destructor
//=============================================================================
HitEffPlotter::~HitEffPlotter() {;}

//=============================================================================
// Initialization
//=============================================================================
StatusCode HitEffPlotter::initialize()
{
    StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
    if (sc.isFailure()) return sc;  // error printed already by GaudiHistoAlg

    if(msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

    m_veloExpectation = tool<IVeloExpectation>("VeloExpectation");
    if ( m_useUT) m_ttExpectation = tool<IHitExpectation>("UTHitExpectation");
    else          m_ttExpectation = tool<IHitExpectation>("TTHitExpectation");
    m_itExpectation = tool<IHitExpectation>("ITHitExpectation");
    m_otExpectation = tool<IHitExpectation>("OTHitExpectation");
    m_xyzExpectation = tool<TrackExpectedHitsXYZTool>("TrackExpectedHitsXYZTool");

    return StatusCode::SUCCESS;
}

template<size_t N> void HitEffPlotter::plot(
	std::string namepfx, std::string titlepfx,
	unsigned nxbins, const double xmin, const double xmax,
	unsigned nybins, const double ymin, const double ymax,
	const std::bitset<N>& expected, const std::bitset<N>& ontrack,
	const boost::array<Gaudi::XYZVector, N>& points)
{
    // check that there are hits at all
    if (0ul == expected.to_ulong()) return;
    // buffer for (fast) conversion of integer (0 to 99) to string
    std::string numstr(2, ' '), titletmp, nametmp;
    for (unsigned i = 0; i < N; ++i) {
	// check that we expect a hit there before we plot
	if (!expected[i]) continue;
	// if we expect a hit but do not have coordinates, we skip as well
	if (points[i].x() != points[i].x() || 
		points[i].y() != points[i].y() ||
		points[i].z() != points[i].z())
		continue;
	if (2*points[i].x() <= points[i].x() || 
		2*points[i].y() <= points[i].y() ||
		2*points[i].z() <= points[i].z())
		continue;
	// convert i to decimal
	numstr[0] = '0' + ((i / 10) % 10);
	numstr[1] = '0' + (i % 10);
	// plot efficiency denominator and numerator
	nametmp = namepfx + numstr + "den";
	titletmp = titlepfx + " layer " + numstr + " hit efficiency denominator;x [mm];y [mm]";
	plot2D(points[i].x(), points[i].y(),
		nametmp, titletmp,
		xmin, xmax, ymin, ymax, nxbins, nybins);
	if (!ontrack[i]) continue;
	nametmp = namepfx + numstr + "num";
	titletmp = titlepfx + " layer " + numstr + " hit efficiency numerator;x [mm];y [mm]";
	plot2D(points[i].x(), points[i].y(),
		nametmp, titletmp,
		xmin, xmax, ymin, ymax, nxbins, nybins);
    }
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode HitEffPlotter::execute()
{
    if(msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

    const LHCb::Tracks* tracks = get<LHCb::Tracks>(m_inputCollection);
    BOOST_FOREACH(const LHCb::Track* tr, *tracks) {
	// skip tracks that do not look good
	if ((tr->p() < 2e3) || ((tr->chi2() / double(tr->nDoF())) > 25.)) continue;

	std::vector<LHCb::LHCbID> ids;
	ids.reserve(2 * tr->nLHCbIDs());

	std::bitset<LHCb::HitPattern::NumVelo> velo[4];
	m_veloExpectation->expectedInfo(*tr, velo);
	m_ttExpectation->collect(*tr, ids);
	m_itExpectation->collect(*tr, ids);
	m_otExpectation->collect(*tr, ids);

	LHCb::HitPattern expectedpat(ids), ontrackpat(tr->lhcbIDs()); 
	expectedpat.setVeloRA(velo[0]);
	expectedpat.setVeloRC(velo[1]);
	expectedpat.setVeloPhiA(velo[2]);
	expectedpat.setVeloPhiC(velo[3]);

	// get intersection points for each layer
	TrackExpectedHitsXYZTool::HitPatternXYZ xyzpat;
	StatusCode sc = m_xyzExpectation->execute(*tr, xyzpat);
	if( sc.isFailure() ) 
		return Warning( "XYZ Hit Expectation failed", StatusCode::SUCCESS, 0 );
 
	// plot Velo
	plot("veloR", "Velo R sensor",
		100u, -75., 75., 100u, -75., 75.,
		expectedpat.veloRA(), ontrackpat.veloRA(), xyzpat.VeloRA);
	plot("veloR", "Velo R sensor",
		100u, -75., 75., 100u, -75., 75.,
		expectedpat.veloRC(), ontrackpat.veloRC(), xyzpat.VeloRC);
	plot("veloPhi", "Velo #phi sensor",
		100u, -75., 75., 100u, -75., 75.,
		expectedpat.veloPhiA(), ontrackpat.veloPhiA(), xyzpat.VeloPhiA);
	plot("veloPhi", "Velo #phi sensor",
		100u, -75., 75., 100u, -75., 75.,
		expectedpat.veloPhiC(), ontrackpat.veloPhiC(), xyzpat.VeloPhiC);
	// plot TT
//   if ( m_useUT ) 
//     plot("ut", "UT", 100u, -1500., 1500., 100u, -1500., 1500.,
//          expectedpat.ut(), ontrackpat.ut(), xyzpat.UT);
//   else
  plot("tt", "TT", 100u, -1500., 1500., 100u, -1500., 1500.,
       expectedpat.tt(), ontrackpat.tt(), xyzpat.TT);
	// plot IT
	plot("it", "IT", 20u, -750, 750., 15u, -300., 300.,
		expectedpat.itAC(), ontrackpat.itAC(), xyzpat.ITAC);
	plot("it", "IT", 20u, -750, 750., 15u, -300., 300.,
		expectedpat.itTopBottom(), ontrackpat.itTopBottom(), xyzpat.ITTopBot);
	// plot OT
	plot("ot", "OT", 40u, -3200, 3200., 45u, -2700., 2700.,
		expectedpat.ot1stMonoLayer(), ontrackpat.ot1stMonoLayer(), xyzpat.OT);
	plot("ot", "OT", 40u, -3200, 3200., 45u, -2700., 2700.,
		expectedpat.ot2ndMonoLayer(), ontrackpat.ot2ndMonoLayer(), xyzpat.OT);
    }

    return StatusCode::SUCCESS;
}


// $Id: $
// STL
#include <array>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>
#include <tuple>
#include <map>
#include <unordered_map>

// Boost
#include <boost/range/iterator_range_core.hpp>

// from Gaudi
#include <GaudiKernel/ToolFactory.h>
#include <GaudiKernel/SystemOfUnits.h>
#include <GaudiKernel/boost_allocator.h>
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/ParsersFactory.h"

// from LHCb
#include <Kernel/ILHCbMagnetSvc.h>

// Muon Detector
#include <MuonDet/DeMuonDetector.h>

// Hlt1Muons
#include <Hlt1Muons/Candidate.h>

// Parsers for std::unordered_map types we need
namespace {
   using CorrectMap = std::unordered_map<std::string, std::vector<double>>;
   using WindowMap  = std::unordered_map<unsigned int, std::pair<double, double>>;
}

namespace Gaudi {
  namespace Parsers {
    // Parser grammar and parse function for CorrectMap
    template <typename Iterator, typename Skipper>
    struct Grammar_<Iterator, CorrectMap, Skipper > {
      // In this case, the type is a mapping type, so it requires the MapGrammar.
      // For other grammars see GaudiKernel/GrammarsV2.h
      typedef MapGrammar<Iterator, CorrectMap, Skipper> Grammar;
    };
    StatusCode parse(CorrectMap& result, const std::string& input) {
        return parse_(result, input);
    }

    // Parser grammar and parse function for WindowMap
    template <typename Iterator, typename Skipper>
    struct Grammar_<Iterator, WindowMap, Skipper > {
      typedef MapGrammar<Iterator, WindowMap, Skipper> Grammar;
    };
    StatusCode parse(WindowMap& result, const std::string& input) {
      return parse_(result, input);
    }
  }
}

// We also need to be able to print an object of our type as a string that both
// Python and our parser can understand,
namespace std {
  // This is an example valid for any mapping type.
  ostream& operator<<(ostream& s, const CorrectMap& m) {
    bool first = true;
    s << '{';
    // this is needed to make a TCK, because it ensures the output is sorted
    // and fromString followed by toString gives the same result
    const map<std::string, std::vector<double>> m1(m.begin(), m.end());
    for(const auto& i: m1) {
      if (first) first = false;
      else s << ", ";
      Gaudi::Utils::toStream(i.first, s) << ": ";
      Gaudi::Utils::toStream(i.second, s);
    }
    s << '}';
    return s;
  }
  ostream& operator<<(ostream& s, const WindowMap& m) {
    bool first = true;
    s << '{';
    // this is needed to make a TCK, because it ensures the output is sorted
    // and fromString followed by toString gives the same result
    const map<unsigned int, std::pair<double, double>> m1(m.begin(), m.end());
    for (auto& i : m1) {
      if (first) first = false;
      else s << ", ";
      Gaudi::Utils::toStream(i.first, s) << ": ";
      Gaudi::Utils::toStream(i.second, s);
    }
    s << '}';
    return s;
  }
}

// local
#include "MatchVeloTTMuon.h"

// from MuonID
#include "MuonID/CommonMuonHitManager.h"
#include "MuonID/CommonMuonStation.h"
#include <MuonID/CommonMuonHit.h>

//-----------------------------------------------------------------------------
// Implementation file for class : MatchVeloTTMuon
//
// 2010-12-02 : Roel Aaij, Vasileios Syropoulos
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_COMPONENT( MatchVeloTTMuon )

namespace {
  using std::array;
  using std::make_tuple;
  using std::map;
  using std::to_string;
  using std::vector;
  using std::string;
  using std::pair;
  using std::less;
  using std::make_pair;
  using Gaudi::Units::GeV;
  static const std::array<unsigned int,4> g_order{{ 2u, 3u, 4u, 1u }};
}

//=============================================================================
MatchVeloTTMuon::MatchVeloTTMuon( const string& type, const string& name,
                                  const IInterface* parent )
  : base_class( type, name, parent )
  , m_hitManager{nullptr}
  , m_fieldSvc{nullptr}
{
  declareInterface<ITracksFromTrack>( this );

  // Stuff for Velo seeds.
  declareProperty("Window", m_window = {{1u, {100, 200}},
                                        {2u, {200, 400}},
                                        {3u, {300, 500}},
                                        {4u, {400, 600}}});

  declareProperty("MinMomentum", m_minMomentum =    3 * Gaudi::Units::GeV);
  declareProperty("MaxPt",             m_maxPt = 1000 * Gaudi::Units::MeV);
  declareProperty("KickScale",     m_kickScale = 1255 * Gaudi::Units::MeV);
  declareProperty("KickOffset",   m_kickOffset = 175 * Gaudi::Units::MeV);

  // Focal plane parametrisation
  declareProperty("MagnetPlaneParA", m_z0 =  5.412 * Gaudi::Units::m);
  declareProperty("MagnetPlaneParB", m_z1 = -3.567 * Gaudi::Units::m);
  declareProperty("MagnetCorParA",   m_c0 =     25.17 * Gaudi::Units::mm);
  declareProperty("MagnetCorParB",   m_c1 =   -701.5  * Gaudi::Units::mm);

  // Corrections for errors should be of the form:
  // {'x'  : [a, b, c, d], 'y'  : [e, f, g, h],
  //  'mx' : [c0, c1, c2], 'my' : [c0, c1, c2]}
  // where:
  //  - Corrections on the error are applied by multiplying 2 * d{x,y} with the
  //    correction for M2-M5,
  //  - mx and my are coefficients of a polynomial in 1/p for the magnet hit,
  //    that gives the error on the hit in the magnet.
  //  x, y, mx and my must be present. x and y must contain 4 values, mx and my three.
  declareProperty("ErrorCorrections", m_errCorrect = {{"mx", {1., 0., 0.}},
                                                      {"my", {1., 0., 0.}},
                                                      {"x",  {1., 1. , 1., 1.}},
                                                      {"y",  {1., 1. , 1., 1.}}});

  // General Stuff
  declareProperty("MaxChi2DoFX",   m_maxChi2DoFX = 20);
  declareProperty("MaxMissedHits", m_maxMissed = 1);
  declareProperty("SetQOverP",     m_setQOverP = false);
  declareProperty("FitY",          m_fitY = false, "Do a fit in Y instead of using the"
                  "Velo track extrapolation to calculate the chi^2");
}

//=============================================================================
StatusCode MatchVeloTTMuon::initialize()
{
  StatusCode sc = GaudiHistoTool::initialize();
  if ( sc.isFailure() ) return sc;

  // Check if properties containing corrections are correct.
  map<string, size_t> cor{{"x", 4}, {"y", 4}, {"mx", 3}, {"my", 3}};
  for (auto e : cor) {
     if (!m_errCorrect.count(e.first)) {
        return Error(string{"No entry "} + e.first + " in ErrorCorrection.", StatusCode::FAILURE);
     } else if (m_errCorrect[e.first].size() != e.second) {
        return Error(string{"Correction entry "} + e.first + " has size "
                     + to_string(m_errCorrect[e.first].size()) + ", while it should be "
                     + to_string(e.second) , StatusCode::FAILURE);
     }
  }

  auto reorder = [this](array<double, 5>& values, CorrectMap::key_type key) -> void {
     // The magnet hit is corrected when it is created, so don't shift or scale later.
     values[0] = 1.;
     std::copy(std::begin(m_errCorrect[key]), std::end(m_errCorrect[key]), std::begin(values) + 1);
  };
  reorder(m_corX, {"x"});
  reorder(m_corY, {"y"});

  // Print the coeffients.
  auto printCor = [this](CorrectMap::key_type key) {
     info() << key << ": ";
     for (auto e : m_errCorrect[key])
        info() << e << " ";
  };
  info() << "Corrections  to  errors: "; printCor("x"); printCor("y"); info() << endmsg;
  info() << "Coefficients for errors: "; printCor("mx"); printCor("my"); info() << endmsg;

  // init hit manager
  m_hitManager = tool<CommonMuonHitManager>( "CommonMuonHitManager" );

  // init magnetic field
  m_fieldSvc = svc<ILHCbMagnetSvc>( "MagneticFieldSvc", true );

  // init muon detector
  m_det = getDet<DeMuonDetector>( "/dd/Structure/LHCb/DownstreamRegion/Muon" );

  // fill local arrays of pad sizes and region sizes
  m_padSizeX.resize( nStations * nRegions );
  m_padSizeY.resize( nStations * nRegions );

  for ( unsigned int s = 0; s < nStations; ++s ) {
    for ( unsigned int r = 0; r < nRegions; ++r ) {
      m_padSizeX[s * nRegions + r] = m_det->getPadSizeX( s, r );
      m_padSizeY[s * nRegions + r] = m_det->getPadSizeY( s, r );
    }
  }

  return sc;
}

//=============================================================================
StatusCode MatchVeloTTMuon::finalize()
{
  i_clean();
  return GaudiHistoTool::finalize();
}

//=============================================================================
StatusCode MatchVeloTTMuon::tracksFromTrack( const LHCb::Track& seed,
                                             vector<LHCb::Track*>& tracks ) const
{
  // Clean start
  i_clean();
  counter("#seeds") += 1;
  // Find seeds
  unsigned int seedStation = g_order[0] ;
  i_findSeeds( seed, seedStation );

  // Add hits to seeds
  for ( Candidate& c : m_seeds ) i_addHits( c );

  // create histograms
  if ( msgLevel( MSG::DEBUG ) ) {
    debug() << "Found " << m_seeds.size() << " seeds." << endmsg;
    for ( const Candidate& c : m_seeds ) {
      debug() << "Found candidate with chi2/DoF " << c.chi2DoF() << endmsg;
    }
  }

  if ( produceHistos() ) {
    plot( m_seeds.size(), "NSeedHits", -0.5, 50.5, 51 );
    for ( const Candidate& c : m_seeds ) {
      if (c.fitted()) {
        plot( c.chi2DoF(), "Chi2DoFX", 0, 100, 100 );
      }
    }
  }

  // in this case, we only care whether a good seed exists for the specified track...
  if ( !m_setQOverP ) {
    if ( std::any_of( begin(m_seeds), end(m_seeds), [=](const Candidate& c) {
             return c.fitted() && c.chi2DoF() < m_maxChi2DoFX ;
         }) ) {
      // There is a good enough candidate, put the seed into the output unmodified.
      tracks.push_back( const_cast<LHCb::Track*>( &seed ) );
      counter("#matched") += 1;
    }
  } else {
    auto last = std::partition(begin(m_seeds), end(m_seeds), []( const Candidate& c) { return c.fitted(); });
    auto best = std::min_element( begin(m_seeds), last,
                                  []( const Candidate& lhs, const Candidate& rhs ) {
                                     return lhs.chi2DoF() < rhs.chi2DoF();
                                  } );
    if ( best != last && best->chi2DoF() < m_maxChi2DoFX ) {
      std::unique_ptr<LHCb::Track> out{seed.clone()};
      out->addToAncestors( seed );
      out->addInfo( 35, best->slope() - best->tx() );
      double down = m_fieldSvc->isDown() ? -1 : 1;
      double q = down * ( ( best->slope() < best->tx() ) - ( best->slope() > best->tx() ) );
      for (const auto& state : out->states()){
        state->setQOverP( q / best->p() );
      }
      tracks.push_back( out.release() );
      counter("#matched") += 1;
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
void MatchVeloTTMuon::i_findSeeds( const LHCb::Track& seed,
                                   const unsigned int seedStation ) const
{
  // Make a Candidate from the track
  Candidate veloSeed{ &seed };

  // extrapolation from TT to M3 station - Same extrapolation as in the VeloMuonMatch.
  // dSlope is the maximum deflection
  double dSlope = dtx( m_minMomentum );
  const CommonMuonStation& station = m_hitManager->station( seedStation );
  const double zStation = station.z();

  // parametrization of the z-position of the magnet’s focal plane as a function of
  // the direction of the velo track ty^2
  const double zMagnet = m_z0 + m_z1 * veloSeed.ty2();
  double xMagnet = 0., errXMagnet = 0.;
  veloSeed.xStraight( zMagnet, xMagnet, errXMagnet );

  double dz = ( zStation - zMagnet );
  double xMin = 0. ;
  double xMax = 0. ;

  double xWindow = 0., yWindow = 0.;
  std::tie(xWindow, yWindow) = m_window.at(seedStation);

  if (seed.hasTT()){
    //Use VeloTT info to get the charge of the seed.
    double tan = 0;
    double xM3 = 0., errXM3 = 0.;
    veloSeed.xStraight( zStation, xM3, errXM3 );
    const int mag = m_fieldSvc->isDown() ? -1 : 1;
    const int charge = seed.charge();
    if (charge * mag < 0 ){
      tan = ( veloSeed.tx() + dSlope ) / ( 1 - veloSeed.tx() * dSlope );
    }else {
      tan = ( veloSeed.tx() - dSlope ) / ( 1 + veloSeed.tx() * dSlope );
    }
    xMin = xMagnet + dz * tan ;
    xMax = xM3 ;
    if (xMin>xMax){
      std::swap(xMin,xMax);
    }

    xMin -= xWindow;
    xMax += xWindow;
  } else {
    //if I cannot estimate the charge, do as in old VeloMuonMatch
    const double tanMin = ( veloSeed.tx() - dSlope ) / ( 1 + veloSeed.tx() * dSlope );
    xMin = xMagnet + dz * tanMin - xWindow;
    const double tanMax = ( veloSeed.tx() + dSlope ) / ( 1 - veloSeed.tx() * dSlope );
    xMax = xMagnet + dz * tanMax + xWindow;
  }

  // Calculate window in y. In y I just extrapolate in a straight line.
  double yMuon = 0., yRange = 0;
  veloSeed.yStraight( zStation, yMuon, yRange );
  yRange += yWindow;

  const double yMin = yMuon - yRange;
  const double yMax = yMuon + yRange;

  // debug info: print hits in M3
  if ( UNLIKELY( msgLevel( MSG::DEBUG ) ) ) {
    debug() << "window: (" << xMin << "," << yMin << ") -> (" << xMax << "," << yMax << ")" << endmsg;
    debug() << "Hits in seed station:" << endmsg;
    for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
      for ( const auto& hit : m_hitManager->hits( xMin, xMax, seedStation, r ) ) {
        debug() << hit.x() << " " << hit.y() << endmsg;
      }
    }
  }

  // Momentum-dependent correction on zMagnet as a function of 1/p in 1/GeV
  auto correct = [](double p_inv, const CorrectMap& cor, CorrectMap::key_type key,
                    double def) -> double {
     if (!cor.count(key)) return def;
     double r = 0.;
     auto coeffs = cor.at(key);
     for (size_t i = 0; i < coeffs.size(); ++i) {
        r += coeffs[i] * pow(p_inv, i);
     }
     return r;
  };

  // get hits inside window, and make M3 seeds for a given velo seed
  for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
    if ( !station.overlaps( r, xMin, xMax, yMin, yMax ) ) continue;
    for (const auto& hit : m_hitManager->hits( xMin, xMax, seedStation, r ) ) {
      if ( hit.y() > yMax || hit.y() < yMin ) continue;
      m_seeds.emplace_back ( veloSeed );
      Candidate& seed = m_seeds.back();

      // Initial slope and momentum estimate
      auto zm = zMagnet;
      auto slope = ( hit.x() - xMagnet ) / ( hit.z() - zm);
      auto p = momentum( slope - seed.tx() );

      // Momentum-dependent correction on zMagnet as a function of p in GeV
      zm += m_c0 + m_c1 * Gaudi::Units::GeV / p;

      double xMagnet = 0., errXMagnet = 0.;
      seed.xStraight( zm, xMagnet, errXMagnet );
      double yMagnet = 0., errYMagnet = 0.;
      seed.yStraight( zm, yMagnet, errYMagnet );

      // update momentum estimate of seed
      seed.slope() = ( hit.x() - xMagnet ) / ( hit.z() - zm );
      seed.p() = momentum( seed.slope() - seed.tx() );

      // Filter seeds with too high p_T
      if (seed.p()*seed.sinTrack() > m_maxPt ){
        m_seeds.pop_back();
      } else {
         LHCb::MuonTileID id;
         // Set the X and Y errors such that the error correction is a straigt multiplication
         // to get the desired resolution.
         // Calculate the error on the hit in the magnet from a parameterisation obtained from MC
         // as a function of the first momentum estimate based on the single seed hit.
         auto dx = 0.5 * correct(GeV / seed.p(), m_errCorrect, "mx", 1.);
         auto dy = 0.5 * correct(GeV / seed.p(), m_errCorrect, "my", 1.);
         m_magnetHits.emplace_back( new CommonMuonHit( id, xMagnet, dx, yMagnet, dy, zm, 0., false ) );
         seed.addHit( m_magnetHits.back().get() );
         seed.addHit( &hit );
      }
    }
  }
}

//=============================================================================
void MatchVeloTTMuon::i_addHits( Candidate& seed ) const
{
  // First hit is in magnet
  const double zMagnet = seed.hits()[0]->z();
  const double xMagnet = seed.hits()[0]->x();

  unsigned int nMissed = 0;
  for ( unsigned int i = 1; i <g_order.size() && nMissed <= m_maxMissed; ++i ) {
    // find candidate hits
    unsigned int s = g_order[i] ;

    // Get the station we're looking at.
    const CommonMuonStation& station = m_hitManager->station( s );
    const double zStation = station.z();

    double xWindow = 0., yWindow = 0.;
    std::tie(xWindow, yWindow) = m_window.at(s);

    // Calculate window in x and y for this station
    double yMuon = 0., yRange = 0;
    seed.yStraight( zStation, yMuon, yRange );
    yRange = yWindow;

    const double yMin = yMuon - yRange;
    const double yMax = yMuon + yRange;

    const double xMuon = ( zStation - zMagnet ) * seed.slope() + xMagnet;
    const double xRange = xWindow;

    const double xMin = xMuon - xRange;
    const double xMax = xMuon + xRange;

    // Look for the closest hit inside the search window
    const CommonMuonHit* closest = nullptr;
    double minPull = 0;
    double x = 0., dx = 0., y = 0., dy = 0.;

    for ( unsigned int r = 0; r < station.nRegions(); ++r ) {
      if ( !station.overlaps(r, xMin, xMax, yMin, yMax ) ) continue;
      for ( const auto& hit : m_hitManager->hits( xMin, xMax,  s, r ) ) {
        std::tie(y, dy) = hity(hit, true);
        if ( y > yMax || y < yMin ) continue;

        std::tie(x, dx) = hitx(hit, true);
        seed.yStraight( hit.z(), yMuon, yRange );
        auto resy = yMuon - hit.y();
        auto resx = (xMagnet + (hit.z() - zMagnet) * seed.slope()) - hit.x();
        double pull = sqrt(resx * resx + resy * resy) / sqrt(4 * dx * dx + 4 * dy * dy);
        if ( !closest || pull < minPull ) {
          closest = &hit;
          minPull = pull;
        }
      }
    }

    if ( closest ) {
      seed.addHit( closest );
    } else {
      ++nMissed;
    }
  }

  // If a new candidate is good, fit it.
  if ( nMissed <= m_maxMissed ) i_fitCandidate( seed );
}

//=============================================================================
void MatchVeloTTMuon::i_fitCandidate( Candidate& candidate ) const
{
  const CommonConstMuonHits& hits = candidate.hits();
  auto magnetHit = hits[0];

  // Result of our fit
  using FitResult = std::tuple<double, double, double>;
  using fun = std::function<pair<double, double>(const MatchVeloTTMuon&, const CommonMuonHit&, bool)>;

  // Calculate straight line fit using linear-least-squares
  // Code inspired by numerical recipes, section 15.2
  auto fit = [this, magnetHit, &hits](const fun& hitInfo) -> FitResult {
     auto info = [this, &hitInfo](const CommonMuonHit* hit) { return hitInfo(*this, *hit, true); };

     // Calculate some sums
    double sw = 0., sz = 0., sc = 0.;
     for ( auto hit : hits ) {
        auto c = info(hit);
        double w = 1. / ( c.second * c.second );
      sw += w;
        sz += (hit->z() - magnetHit->z()) * w;
      sc   += c.first * w;
    }
    double zow = sz / sw;

     // Calculate the estimate for the slope
    double st = 0.;
    double b = 0.;
     for ( auto hit : hits ) {
        auto c = info(hit);
        double tmp = ( hit->z() - magnetHit->z() - zow ) / c.second;
      st += tmp * tmp;
        b += tmp * c.first / c.second;
    }
    b /= st;
    double a = ( sc - sz * b ) / sw;

     // Calculate the chi^2
    double chi2 = 0.;
     for ( auto hit : hits ) {
        auto c = info(hit);
        double tmp = ( c.first - a - b * (hit->z() - magnetHit->z()) ) / c.second;
      chi2 += tmp * tmp;
    }

     return FitResult{a, b, chi2};
  };

  // In y, if we assume the velo extrapolation is correct the calculate the residuals
  // and chi^2 can be calculated with respect to that.
  const auto calcChi2y = [this, &candidate]() -> double {
     double chi2 = 0;
     double yExt = 0, err = 0, yh = 0, dy = 0;
     // Skip the magnet hit. It's approximated error can only add noise
     for (auto hit : boost::make_iterator_range(std::begin(candidate.hits()) + 1, std::end(candidate.hits()))) {
        if (hit->tile().key() == 0) continue;
        candidate.yStraight(hit->z(), yExt, err);
        std::tie(yh, dy) = hity(*hit, true);
        chi2 += pow((yh - yExt) / dy, 2);
}
     return chi2;
  };

  // Do the fit in X and also fit Y, or get the chi^2 from the extrapolation.
  auto fitX = fit(&MatchVeloTTMuon::hitx);
  FitResult fitY;
  if (m_fitY) {
     fitY = fit(&MatchVeloTTMuon::hity);
  } else {
     fitY = make_tuple(magnetHit->y(), candidate.ty(), calcChi2y());
  }

  // Total chi^2
  candidate.nDoF() = m_fitY ? 2 * (hits.size() - 2) : 2 * hits.size() - 2;
  candidate.chi2() = std::get<2>(fitX) + std::get<2>(fitY);
  candidate.slope() = std::get<1>(fitX);
  candidate.p() = momentum(candidate.slope() - candidate.tx() );
  candidate.fitted() = true;

  // Some plots for monitoring purposes
  if ( produceHistos() ) {
     // Calculate final left or right
     auto plotResidual = [this, magnetHit](const CommonMuonHit* hit,
                                           const fun& hitFun, std::tuple<double, double, double> info,
                                           string title) {
        double c = 0., e = 0.;
        for (auto entry : {make_pair(false, ""), make_pair(true, "_corrected")}) {
           std::tie(c, e) = hitFun(*this, *hit, entry.first);
           auto res = c - (std::get<0>(info) + std::get<1>(info) * (hit->z() - magnetHit->z()));
           auto suf = entry.second;
           plot(res, title + "_residual" + suf, -500, 500, 100);
           plot(e, title + "_error" + suf, -500, 500, 100);
           plot(res / e, title + "_pull" + suf, -5, 5, 100);
        }
     };

     for (auto hit : candidate.hits()) {
        string title = (hit->tile().key() == 0) ? "magnet" : string{"M"} + to_string(hit->station() + 1);
        plotResidual(hit, &MatchVeloTTMuon::hitx, fitX, title + "_x");
        plotResidual(hit, &MatchVeloTTMuon::hity, fitY, title + "_y");
  }
}
  }

//=============================================================================
void MatchVeloTTMuon::i_clean() const
{
  // delete leftover seeds
  m_magnetHits.resize(0);
  m_seeds.clear(); // leaves capacity invariant, hence we latch onto the max size
}

//=============================================================================
void MatchVeloTTMuon::findSeeds( const LHCb::Track& seed, const unsigned int seedStation )
{
  i_findSeeds( seed, seedStation );
}

//=============================================================================
void MatchVeloTTMuon::addHits( Candidate& seed )
{
  i_addHits( seed );
}

//=============================================================================
void MatchVeloTTMuon::fitCandidate( Candidate& seed ) const
{
  i_fitCandidate(seed);
}

//=============================================================================
void MatchVeloTTMuon::clean()
{
  i_clean();
}

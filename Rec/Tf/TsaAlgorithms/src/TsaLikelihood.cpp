// Tsa
#include "TsaLikelihood.h"


// Boost
// Boost
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
#include "boost/lexical_cast.hpp"
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

// gsl
#include "gsl/gsl_cdf.h"

using namespace LHCb;
using namespace boost::lambda;
using namespace boost::assign;
//using namespace boost;

using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( Likelihood )

Likelihood::Likelihood(const std::string& type,
                       const std::string& name,
                       const IInterface* parent):
  GaudiTool(type, name, parent),
  m_parabolaFit(0)
{

  declareProperty("LikCut", m_likCut = -40.0);
  declareProperty("ITEff", m_itEff = 0.995);
  declareProperty("OTEff", m_otEff = 0.85);
  {
    std::vector<double> tmp = list_of(5.335)(0.005433)(7.551)(0.04098);
    declareProperty("f1Par", m_f1Par = tmp);
  }
  {
    std::vector<double> tmp = list_of(6.884)(35.49)(5.62)(6.928);
    declareProperty("f2Par", m_f2Par = tmp);
  }
  {
    std::vector<double> tmp = list_of(1.0)(1.0)(1./2.)(1./2.)(1.0)(1.0)(1.0);
    declareProperty("weights", m_weights = tmp);
  }
  declareProperty("outlierCut", m_outlierCut = 6.0);

  declareInterface<ITsaSeedStep>(this);
}

Likelihood::~Likelihood(){
  // destructer
}

StatusCode Likelihood::finalize()
{
	delete m_parabolaFit;
	m_parabolaFit = 0;

	return GaudiTool::finalize();
}

StatusCode Likelihood::initialize()
{
 const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize",sc);
  }

  if (m_weights.size() < 7)
    return Error( "Failed to initialize: Not enough weights for "
		    "likelihood contributions.", StatusCode::FAILURE );

  m_parabolaFit = new SeedParabolaFit(TsaConstants::z0, m_outlierCut);

  m_expectedHits = tool<IOTExpectedHits>("Tf::Tsa::OTExpectedHits");
  m_expectedITHits = tool<IITExpectedHits>("Tf::Tsa::ITExpectedHits");

  m_xLayers[0] = 1;
  m_xLayers[1] = 4;
  m_uLayers[0] = 2;
  m_uLayers[1] = 3;

  // precalculate some logs
  m_logITEff = log(m_itEff);
  m_logOTEff = log(m_otEff);
  m_logITInEff = log(1-m_itEff);
  m_logOTInEff = log(1-m_otEff);

  m_f1Norm = 1.0/(exp(m_f1Par[0]) + exp(m_f1Par[2]));
  m_f2Norm = 1.0/(exp(m_f2Par[0]) + exp(m_f2Par[2]));

  return sc;
}

StatusCode Likelihood::execute(LHCb::State& , std::vector<SeedTrack*>& seeds, std::vector<SeedHit*>*  ){
  return execute(seeds);
}

StatusCode Likelihood::execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*>*  ){

  //-------------------------------------------------------------------------
  //  Calc the likelihood
  //-------------------------------------------------------------------------

  //debug() << "Been passed " << seeds.size() << " SeedTracks" << endmsg;

  for ( std::vector<SeedTrack*>::iterator it = seeds.begin(); seeds.end() != it; ++it ) {
    SeedTrack* seed = *it;
    if ( !seed ) { 
      Warning( "Been passed a NULL SeedTrack pointer", StatusCode::SUCCESS,0 ).ignore();
      if ( msgLevel(MSG::DEBUG) )debug() << "Been passed a NULL SeedTrack pointer" <<endmsg;
      continue; 
    }
   
    if ( seed->live() == false ) continue;
   

    int sect = seed->sector();

    std::vector<SeedPnt>& pnts = seed->xPnts();
       for ( std::vector<SeedPnt>::iterator it = pnts.begin(); pnts.end() != it; ++it ) {
      SeedPnt & pnt = *it;
      //double dy = seed->y(pnt.hit()->z(),TsaConstants::z0) - pnt.hit()->clus()->yMid();
      // CRJ : Update for ateam (need to check this really is the same thing)
      const double dy = seed->y(pnt.hit()->z(),TsaConstants::z0) - pnt.hit()->yMid();
      const double z = pnt.hit()->z() + TsaConstants::tilt*dy;
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose () << "   -> " << pnt.hit()->lhcbID()
                   << " x=" << pnt.hit()->x()
                   << " y=" << pnt.hit()->y()
                   << " z=" << pnt.hit()->z()
                   << " seedY=" << seed->y(pnt.hit()->z(),TsaConstants::z0)
                   << " yMid=" << pnt.hit()->yMid()
                   << " dy=" << dy
                   << endmsg;
      }
      pnt.setZ( z );
    }

    double sx = seed->xSlope(8600.,TsaConstants::z0);
    double csth = sqrt(1.0 + sx*sx);
    // if the fit fails or we dont have enough measurements stop
    const int fitResult = m_parabolaFit->refit(seed,csth);
      if ( fitResult < 1 || seed->nx() < 4 || seed->ny() < 3)
    {
 
      seed->setLive( false );
      continue;
    }



    // double check...
    double nXHit = 0; double nYHit = 0;
    std::vector<SeedPnt> upnts = seed->usedPnts();
    for ( std::vector<SeedPnt>::const_iterator itP = upnts.begin(); upnts.end() != itP; ++itP ) {
      itP->hit()->tfHit()->hit()->isX() ? ++nXHit : ++nYHit; 
    }
    if (nXHit < 4 || nYHit < 3) {
      Warning("Invalid track: not enough hits ", StatusCode::SUCCESS,0).ignore();
      if ( msgLevel(MSG::DEBUG) ) debug() << "Invalid track: not enough hits " << endmsg;
      seed->setLive(false); 
      continue;
    }


    int nxExp = 0;
    int nyExp = 0;
    int nxFound = 0;
    int nyFound = 0;
    double xLik = 0.;
    double yLik = 0.;

    // Fill seed track parameters into format required by tool to determine expected hits
    Line aLine( seed->sy(), seed->y0() );
    Parabola aParab( seed->tx(), seed->sx(), seed->x0() );
    std::vector<SeedPnt>& ypnts = seed->yPnts();

    if (sect<3){
      // IT case
      expectationIT(aLine, aParab, pnts, m_xLayers, sect, nxFound, nxExp);
      expectationIT(aLine, aParab, ypnts, m_uLayers,sect, nyFound, nyExp);
    }
    else if (sect < 5){
      // OT case
      expectationOT(aLine, aParab, pnts, m_xLayers, sect, nxFound, nxExp, xLik);
      expectationOT(aLine, aParab, ypnts, m_uLayers,sect ,nyFound, nyExp, yLik);
    }
    else{
      // mixed case
      expectationMixed(aLine, aParab, pnts, m_xLayers, sect, nxFound, nxExp, xLik);
      expectationMixed(aLine, aParab, ypnts, m_uLayers,sect ,nyFound, nyExp, yLik);
    }

    const bool reject = ( nxExp < 4 || nyExp < 4 ||
                          ( double(nxFound) / nxExp < 0.5 && double(nyFound) / nyExp < 0.5) );
   
    if ( reject ) {
      seed->setLive( false );
    } else {
      const double prx = log( gsl_cdf_chisq_Q( seed->xChi2(), seed->nx()-3 ));
      const double pry = log( gsl_cdf_chisq_Q( seed->yChi2(), seed->ny()-2 ));
      double lik = 0.0;
      if (m_weights[0] > 0.0) lik += m_weights[0] * xLik;
      if (m_weights[1] > 0.0) lik += m_weights[1] * yLik;
      if (m_weights[2] > 0.0) lik += m_weights[2] * prx;
      if (m_weights[3] > 0.0) lik += m_weights[3] * pry;

      int n = nxExp + nyExp;
      int r = nxFound + nyFound;
      if ( r > n ) r = n;
      double fnum = 1.;
      double fden = 1.;
      for ( int i = 0; i < n-r; ++i ) {
        fnum *= double( n-i );
        fden *= double( i+1 );
      }

      // Add binomial term to likelihood according to number of found hits on seed
      if (m_weights[4] > 0.0) lik += m_weights[4] * binomialTerm(sect,fnum/fden,n,r);

      // weight up tracks from the primary interaction point
      const double arg1 = fabs( seed->y0() );
      const double arg2 = fabs( 53000.0*seed->tx() - seed->dth() );

      const double f1 = (exp(m_f1Par[0] - m_f1Par[1]*arg1) + exp(m_f1Par[2] - m_f1Par[3]*arg1)) * m_f1Norm;
      const double f2 = (exp(m_f2Par[0] - m_f2Par[1]*arg2) + exp(m_f2Par[2] - m_f2Par[3]*arg2)) * m_f2Norm;

      if (m_weights[5] > 0.0) lik += m_weights[5] * log(f1);
      if (m_weights[6] > 0.0) lik += m_weights[6] * log(f2);
      if ( lik < m_likCut ) seed->setLive( false );

      seed->setLik( lik );
    }

   
  }

  return StatusCode::SUCCESS;
}

void Likelihood::expectationOT(const Line& aLine, const Parabola& aParab,
                               std::vector<SeedPnt>& pnts, int* layers,
                               const int& sect, int& nFound,
                               int& nExp, double& lik) const{

  // number of expected hits in OT

  for ( int station = 1; station < 4; ++station ) {
    for ( int iL = 0 ; iL < 2; ++iL ) {

      LHCb::OTChannelID testChan = channelHint(station,layers[iL]-1,pnts);
      typedef std::vector<IOTExpectedHits::OTPair> OTPairs;
      OTPairs output;
      StatusCode sc = m_expectedHits->collect(aParab,aLine,testChan,output, sect);
      if (sc.isFailure()){
        Warning("Failed to calculate expected hits",StatusCode::SUCCESS,0).ignore();
        if ( msgLevel(MSG::DEBUG) )debug() << "Failed to calculate expected hits" <<endmsg;
      }
     
      for (OTPairs::iterator iter = output.begin() ;iter != output.end(); ++iter ){
        std::vector<SeedPnt>::iterator pntIter  = std::find_if(pnts.begin(),
                                                               pnts.end(),
                                                               bind(&SeedPnt::sameStraw, _1, iter->first));
        ++nExp;
        if (pntIter != pnts.end() ) {
          ++nFound;
        }
        else {
          lik += log( 0.261 + exp( 5.1 * fabs(iter->second) - 11.87 ) );
        } // if
      }  // pairs
    } // iL
  } // station
}



void Likelihood::expectationIT(const Line& aLine, const Parabola& aParab,
                               std::vector<SeedPnt>& pnts, int* layers, const int& sect,
                               int& nFound,
                               int& nExp) const{

  // number of expected hits in IT

  for ( int station = 1; station < 4; ++station )
  {
    for ( int iL = 0 ; iL < 2; ++iL )
    {
      LHCb::STChannelID testChan(LHCb::STChannelID::typeIT, station,layers[iL],0,0,0);
      typedef std::vector<IITExpectedHits::ITPair> ITPairs;
      ITPairs output;
      StatusCode sc = m_expectedITHits->collect(aParab,aLine,testChan,output,sect);
      if (sc.isFailure()){
        Warning("Failed to calculate expected hits",StatusCode::SUCCESS,0).ignore();
        if ( msgLevel(MSG::DEBUG) )debug() << "Failed to calculate expected hits" <<endmsg;
      }
      int old = -1;
     
      for (ITPairs::iterator iter = output.begin(); iter != output.end(); ++iter )
      {
        if ( int(iter->second) != old)
        {
          old = int(iter->second);
          ++nExp;
        }
        std::vector<SeedPnt>::iterator pntIter = std::find_if(pnts.begin(),
                                                              pnts.end(),
                                                              bind(&SeedPnt::sameChan, _1, iter->first));
        if ( pntIter != pnts.end() )
        {
          ++nFound;
        }
      } // output
    } // iL
  } // station
}

void Likelihood::expectationMixed(const Line& aLine, const Parabola& aParab,
                                  std::vector<SeedPnt>& pnts, int* layers, const int& sect,
                                  int& nFound,
                                  int& nExp, double& lik) const{

  // split into IT and OT part
  std::vector<SeedPnt> itPnts; itPnts.reserve(20);
  std::vector<SeedPnt> otPnts; otPnts.reserve(20);

  for (std::vector<SeedPnt>::const_iterator iterPnt = pnts.begin(); iterPnt != pnts.end(); ++iterPnt){
    (*iterPnt).hit()->OT() ? otPnts.push_back(*iterPnt) : itPnts.push_back(*iterPnt);
  } // iterPnt

  // OT part

  if (otPnts.empty() == false) {
    const double yTest = aLine.value(8600);
    unsigned int otsect;
    yTest < 0.0 ? otsect = 3 : otsect = 4;
    expectationOT(aLine, aParab, otPnts, layers, sect, nFound, nExp, lik);
  }

  // IT part - try all sectors for now...
  if (itPnts.empty() == false){
    for (int i= 0; i < 3; ++i) expectationIT(aLine, aParab, itPnts, layers, i , nFound, nExp);
  } //i
}

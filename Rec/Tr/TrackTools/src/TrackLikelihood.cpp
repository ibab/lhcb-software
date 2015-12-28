// from GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/Track.h"
#include "Event/VeloCluster.h"
#include "Event/VeloMeasurement.h"
#include "Event/VeloLiteMeasurement.h"
#include "Event/STCluster.h"
#include "Event/STMeasurement.h"

// local
#include "TrackLikelihood.h"

#include <algorithm>

#include "Kernel/LHCbID.h"

#include "gsl/gsl_math.h"
#include "gsl/gsl_rng.h"
#include "gsl/gsl_randist.h"
#include "gsl/gsl_cdf.h"

#include "TrackInterfaces/IHitExpectation.h"
#include "TrackInterfaces/IVeloExpectation.h"

using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( TrackLikelihood )

//=============================================================================
//
//=============================================================================
TrackLikelihood::TrackLikelihood(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent):
  GaudiTool(type, name, parent),
  m_veloExpectation(0),
  m_ttExpectation(0),
  m_utExpectation(0),
  m_itExpectation(0),
  m_otExpectation(0)

{
  // constructer
  declareProperty("veloREff", m_veloREff = 0.965);
  declareProperty("veloPhiEff", m_veloPhiEff = 0.94);

  declareProperty("itEff", m_itEff = 0.995);
  declareProperty("otEff", m_otEff = 0.93);
  declareProperty("ttEff", m_ttEff = 0.99);
  declareProperty("utEff", m_utEff = 0.99);

  declareProperty("itHighEff", m_itHighEff = 0.94);
  declareProperty("veloHighEff1", m_veloHighEff1 = 0.99);
  declareProperty("veloHighEff2", m_veloHighEff2 = 0.995);
  declareProperty("ttHighEff", m_ttHighEff = 0.896);
  declareProperty("utHighEff", m_utHighEff = 0.896);

  declareProperty("useVelo", m_useVelo = true);
  declareProperty("useTT", m_useTT = true);
  declareProperty("useUT", m_useUT = false);
  declareProperty("useIT", m_useIT = true);
  declareProperty("useOT", m_useOT = true);
  declareProperty("chiWeight", m_chiWeight = 1.0);

  declareInterface<ITrackManipulator>(this);

}

//=============================================================================
//
//=============================================================================
TrackLikelihood::~TrackLikelihood(){
  // destructer
}

StatusCode TrackLikelihood::initialize() {
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  if (m_useVelo) m_veloExpectation = tool<IVeloExpectation>("VeloExpectation");
  if (m_useTT) m_ttExpectation = tool<IHitExpectation>("TTHitExpectation");
  if (m_useUT) m_utExpectation = tool<IHitExpectation>("UTHitExpectation");
  if (m_useIT) m_itExpectation = tool<IHitExpectation>("ITHitExpectation");
  if (m_useOT) m_otExpectation = tool<IHitExpectation>("OTHitExpectation");

  return StatusCode::SUCCESS;
}

StatusCode TrackLikelihood::execute(LHCb::Track& aTrack) const{

  // chi^2 contribution
  double lik = 0.0;
  if ( aTrack.probChi2() < 1e-50 ) lik = -150.0;
  else lik = log(aTrack.probChi2())/m_chiWeight;

  if (m_useVelo && aTrack.hasVelo()){
    lik += addVelo(aTrack);
  }

  if (m_useTT && aTrack.hasTT()){
    lik += addTT(aTrack);
  }

  if (m_useUT && aTrack.hasUT()){
    lik += addUT(aTrack);
  }

 if (aTrack.hasT()){
   if (m_useOT) lik += addOT(aTrack);
   if (m_useIT) lik += addIT(aTrack);
  }

  // add the likelihood to track (overwrites previous value)
   aTrack.setLikelihood(lik);

  return StatusCode::SUCCESS;
}

double TrackLikelihood::addVelo(LHCb::Track& aTrack) const{

  double lik = 0.0;

  // pick up the LHCbIDs + measurements
  const std::vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();
  LHCb::Track::MeasurementContainer meas = aTrack.measurements();

  // get the number of expected hits in the velo
  unsigned int nR = 0;
  std::vector<LHCb::LHCbID> veloHits; veloHits.reserve(ids.size());
  std::copy_if(ids.begin(), ids.end(), std::back_inserter(veloHits), [](const LHCb::LHCbID& id) { return id.isVelo(); } );
  for (std::vector<LHCb::LHCbID>::const_iterator iterV = veloHits.begin(); iterV != veloHits.end();  ++iterV){
    if (iterV->veloID().isRType() == true) ++nR;
  } //iterV

  // get the number of hits
  IVeloExpectation::Info expectedVelo = m_veloExpectation->expectedInfo(aTrack);
  lik += binomialTerm(nR, expectedVelo.nR ,m_veloREff);
  lik += binomialTerm(veloHits.size() - nR, expectedVelo.nPhi ,m_veloPhiEff);


  std::vector<const LHCb::Measurement*> veloRHits; veloRHits.reserve(meas.size());
  std::copy_if(meas.begin(), meas.end(), std::back_inserter(veloRHits),
                 [](const LHCb::Measurement* m) { return m->checkType(Measurement::VeloR); } );
  std::copy_if(meas.begin(), meas.end(), std::back_inserter(veloRHits),
                 [](const LHCb::Measurement* m) { return m->checkType(Measurement::VeloLiteR); } );

  // loop and count # with high threshold in R
  unsigned int nHigh = 0;
  for (std::vector<const LHCb::Measurement*>::iterator iterM = veloRHits.begin();
    iterM != veloRHits.end(); ++iterM){
    const LHCb::VeloMeasurement* veloMeas = dynamic_cast<const LHCb::VeloMeasurement*>(*iterM);
    if( veloMeas ){
      if (veloMeas->highThreshold() == true) ++nHigh;
    }else{
      const LHCb::VeloLiteMeasurement* veloLiteMeas = dynamic_cast<const LHCb::VeloLiteMeasurement*>(*iterM);
      if (veloLiteMeas->highThreshold() == true) ++nHigh;
    }
  }

  std::vector<const LHCb::Measurement*> veloPhiHits; veloPhiHits.reserve(meas.size());
  std::copy_if(meas.begin(), meas.end(), std::back_inserter(veloPhiHits),
                 [](const LHCb::Measurement* m) { return m->checkType(Measurement::VeloPhi); } );
  std::copy_if(meas.begin(), meas.end(), std::back_inserter(veloPhiHits),
                 [](const LHCb::Measurement* m) { return m->checkType(Measurement::VeloLitePhi); } );

  // loop and count # with high threshold in phi
  for (std::vector<const LHCb::Measurement*>::iterator iterM = veloPhiHits.begin();
    iterM != veloPhiHits.end(); ++iterM){
    const LHCb::VeloMeasurement* veloMeas = dynamic_cast<const LHCb::VeloMeasurement*>(*iterM);
    if( veloMeas ){
      if (veloMeas->cluster()->highThreshold() == true) ++nHigh;
    }else{
      const LHCb::VeloLiteMeasurement* veloLiteMeas = dynamic_cast<const LHCb::VeloLiteMeasurement*>(*iterM);
      if (veloLiteMeas->highThreshold() == true) ++nHigh;
    }
  }

  // add term to lik
  double prob1 = log(gsl_ran_binomial_pdf(nHigh, m_veloHighEff1, veloRHits.size() + veloPhiHits.size()));
  double prob2 = log(gsl_ran_binomial_pdf(nHigh, m_veloHighEff2, veloRHits.size() + veloPhiHits.size()));
  lik += std::max(prob1,prob2);

  return lik;
}

double TrackLikelihood::addTT(LHCb::Track& aTrack) const{

  double lik = 0.0;

  // pick up the LHCbIDs + measurements
  const std::vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();
  LHCb::Track::MeasurementContainer meas = aTrack.measurements();

   // get the number of expected hits in TT
  const unsigned int nTTHits = std::count_if(ids.begin(), ids.end(),[](const LHCb::LHCbID& id) { return id.isTT(); } );
  const unsigned int nExpectedTT = m_ttExpectation->nExpected(aTrack);
  if (nExpectedTT > 2) {

    lik += binomialTerm(nTTHits, nExpectedTT ,m_ttEff);

    // spillover information for TT
    std::vector<const LHCb::Measurement*> ttHits; ttHits.reserve(meas.size());
    std::copy_if(meas.begin(), meas.end(), std::back_inserter(ttHits),
                 [](const LHCb::Measurement* m) { return m->checkType(Measurement::TT); } );

    // loop and count # with high threshold
    unsigned int nHigh = 0;
    for (std::vector<const LHCb::Measurement*>::iterator iterM = ttHits.begin();
       iterM != ttHits.end(); ++iterM){
      const LHCb::STMeasurement* stMeas = dynamic_cast<const LHCb::STMeasurement*>(*iterM);
      if (stMeas->cluster()->highThreshold() == true) ++nHigh;
    }

    double spillprob = gsl_ran_binomial_pdf(nHigh, m_ttHighEff, ttHits.size());
    lik += log(spillprob);
  }


  return lik;
}

double TrackLikelihood::addUT(LHCb::Track& aTrack) const{

  double lik = 0.0;

  // pick up the LHCbIDs + measurements
  const std::vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();
  LHCb::Track::MeasurementContainer meas = aTrack.measurements();

   // get the number of expected hits in UT
  const unsigned int nUTHits = std::count_if(ids.begin(), ids.end(),[](const LHCb::LHCbID& id) { return id.isUT(); } );
  const unsigned int nExpectedUT = m_utExpectation->nExpected(aTrack);
  if (nExpectedUT > 2) {

    lik += binomialTerm(nUTHits, nExpectedUT ,m_utEff);

    // spillover information for UT
    std::vector<const LHCb::Measurement*> utHits; utHits.reserve(meas.size());
    std::copy_if(meas.begin(), meas.end(), std::back_inserter(utHits),
                 [](const LHCb::Measurement* m) { return m->checkType(Measurement::UT); });

    // loop and count # with high threshold
    unsigned int nHigh = 0;
    for (std::vector<const LHCb::Measurement*>::iterator iterM = utHits.begin();
       iterM != utHits.end(); ++iterM){
      const LHCb::STMeasurement* stMeas = dynamic_cast<const LHCb::STMeasurement*>(*iterM);
      if (stMeas->cluster()->highThreshold() == true) ++nHigh;
    }

    double spillprob = gsl_ran_binomial_pdf(nHigh, m_utHighEff, utHits.size());
    lik += log(spillprob);
  }


  return lik;
}

double TrackLikelihood::addOT(LHCb::Track& aTrack) const{

  double lik = 0.0;

  // pick up the LHCbIDs + measurements
  const std::vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();

   // OT
  std::vector<LHCb::LHCbID> otHits; otHits.reserve(ids.size());
  std::copy_if(ids.begin(), ids.end(), std::back_inserter(otHits),
               [](const LHCb::LHCbID& id) { return id.isOT(); } );
  if (otHits.size() > 0u) {
    IHitExpectation::Info otInfo = m_otExpectation->expectation(aTrack);
    lik += binomialTerm(otHits.size(),otInfo.nExpected, m_otEff);
    lik += otInfo.likelihood;
  }

  return lik;
}

double TrackLikelihood::addIT(LHCb::Track& aTrack) const{

  double lik = 0.0;

  // pick up the LHCbIDs + measurements
  const std::vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();
  LHCb::Track::MeasurementContainer meas = aTrack.measurements();


  // IT
  std::vector<LHCb::LHCbID> itHits; itHits.reserve(ids.size());
  std::copy_if(ids.begin(), ids.end(), std::back_inserter(itHits),
               [](const LHCb::LHCbID& id) { return id.isIT(); } );
  if (itHits.size() > 0u) {
    unsigned int nIT = m_itExpectation->nExpected(aTrack);
    lik += binomialTerm(itHits.size(),nIT, m_itEff);

    // spillover information for IT
    std::vector<const LHCb::Measurement*> itHits; itHits.reserve(meas.size());
    std::copy_if(meas.begin(), meas.end(), std::back_inserter(itHits),
                 [](const LHCb::Measurement* m) { return m->checkType(Measurement::IT); } );

    // loop and count # with high threshold
    unsigned int nHigh = 0;
    for (std::vector<const LHCb::Measurement*>::iterator iterM = itHits.begin();
       iterM != itHits.end(); ++iterM){
      const LHCb::STMeasurement* stMeas = dynamic_cast<const LHCb::STMeasurement*>(*iterM);
      if (stMeas->cluster()->highThreshold() == true) ++nHigh;
    }

    double spillprob = gsl_ran_binomial_pdf(nHigh, m_itHighEff, itHits.size());
    lik += log(spillprob);

  }

  return lik;
}

double TrackLikelihood::binomialTerm(const unsigned int& found,
                                     const unsigned int& expected,
		                     const double& eff) const{

  unsigned int n = expected;
  unsigned int r = found;
  if ( r > n ) r = n;
  double fnum = 1.;
  double fden = 1.;
  for ( unsigned int i = 0; i < n-r; ++i ) {
    fnum *= double( n-i );
    fden *= double( i+1 );
  }

  return log( fnum/fden) + r*log(eff) + (n-r)*log(1 - eff);
}

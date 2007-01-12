// $Id: MCHitMonitor.cpp,v 1.4 2007-01-12 10:10:52 cattanem Exp $
//
// This File contains the implementation of the MCHitMonitor
//
//   Author:Matthew Needham
//   Created: 18-05-1999


// Standard C++ Library
#include <string>

// BOOST
#include "boost/lexical_cast.hpp"

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// LHCb Event
#include "Event/MCParticle.h"
#include "Event/MCHit.h"

// Units
#include "GaudiKernel/SystemOfUnits.h"

// Tools
#include "Kernel/IMCParticleSelector.h"

#include "MCHitMonitor.h"

// Needed for the creation of MCHitMonitor objects.
#include "GaudiKernel/AlgFactory.h"
DECLARE_ALGORITHM_FACTORY( MCHitMonitor );

//_________________________________________________
/// MCHitMonitor
///
/// An MCHitMonitor is a Gaudi top level Algorithm that checks the 
/// output of the event tracks fitter


MCHitMonitor::MCHitMonitor(const std::string& name,
		     ISvcLocator* pSvcLocator):
  GaudiHistoAlg(name, pSvcLocator)
{
  /// MCHitMonitor constructor
  this->declareProperty("mcPathString", m_MCHitPath="");
  this->declareProperty("zTolerance",m_ZTolerance = 20.0*Gaudi::Units::cm);
  this->declareProperty("zStations",m_Zstations);
  this->declareProperty("xMax",m_Xmax =400.0*Gaudi::Units::cm );
  this->declareProperty("yMax",m_Ymax = 400.0*Gaudi::Units::cm);
  this->declareProperty("selectorName", m_selectorName = "MCParticleSelector" );
}

MCHitMonitor::~MCHitMonitor()
{
  /// destructor
}

StatusCode MCHitMonitor::initialize()
{
  // initialize  
  
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }
  
  m_selector = tool<IMCParticleSelector>(m_selectorName, m_selectorName, this);

  // initialize histograms
  sc = this->initHistograms();

  return StatusCode::SUCCESS;  
}

StatusCode MCHitMonitor::initHistograms()
{
  /// Intialize histograms
  std::string tPath = this->histoPath()+"/"; 
  
 // make histograms per stations
 IHistogram1D* aHisto1D = 0;
 IHistogram2D* aHisto2D = 0;

 int nStation = m_Zstations.size();
 for (int iStation = 0 ;iStation<nStation; ++iStation){

   aHisto1D = book(100+iStation,
                   "time of Flight"+boost::lexical_cast<std::string>(100+iStation),
                   0.0*Gaudi::Units::ns,100.0*Gaudi::Units::ns, 100);
   m_timeOfFlightHistos.push_back(aHisto1D);

   // x vs y plots
   aHisto2D = histoSvc()->book(
                          tPath+boost::lexical_cast<std::string>(200+iStation),
                       "x vs y"+boost::lexical_cast<std::string>(200+iStation),
                          200, -m_Xmax/Gaudi::Units::cm,m_Xmax/Gaudi::Units::cm,
                          200, -m_Ymax/Gaudi::Units::cm,m_Ymax/Gaudi::Units::cm);
   m_XvsYHistos.push_back(aHisto2D);

   aHisto1D  = book(300+iStation,"lossHisto"+boost::lexical_cast<std::string>(300+iStation),
                    0., 250.,250);

   m_EnergyLossHistos.push_back(aHisto1D);   

   aHisto1D  = book(iStation+400,"pathHisto"+boost::lexical_cast<std::string>(iStation+400),
                    0.,10., 10);

   m_pathHistos.push_back(aHisto1D);   

   aHisto1D  = book(500+iStation,"bgHisto"+boost::lexical_cast<std::string>(500+iStation),
                    0.,10000., 1000);

   m_betaGammaHistos.push_back(aHisto1D);   

 }//loop stations

 return StatusCode::SUCCESS;
}
 
StatusCode MCHitMonitor::execute()
{
  /// Executes MCHitMonitor for one event.
 
  LHCb::MCHits* hitsCont = get<LHCb::MCHits>(m_MCHitPath);

  plot((double)hitsCont->size(),1 ,"num hits", 0.,5000, 100);

  // loop over hits fill some histograms
  LHCb::MCHits::const_iterator iHit = hitsCont->begin() ;
  for ( ; iHit != hitsCont->end(); ++iHit ) {
    fillHistograms(*iHit);
  } // loop hits

  return StatusCode::SUCCESS;
}


StatusCode MCHitMonitor::fillHistograms(LHCb::MCHit* aHit){

  // Initialize

  // check 1 can find pointer to MCparticle
  const LHCb::MCParticle* aParticle = aHit->mcParticle();

  if (!aParticle){
    err() << "failed to find MCParticle" <<endreq;
    return StatusCode::FAILURE;
  }

  // p 
  plot(aParticle->p()/Gaudi::Units::GeV, 4, "pMag", 200,0.,50);


  // average of entrance and exit...
  Gaudi::XYZPoint mcHitPoint = aHit->midPoint();

  plot(aHit->pathLength(),"path length", 0., 200., 200);

  // bin in regions of z ~ stations
  const int iStation = getStationID(mcHitPoint.z());

  if (iStation<0) {
     err() << "failed to get station number for z " << mcHitPoint.z() << endreq;
     return StatusCode::FAILURE;
  } 

  if (m_selector->accept(aHit->mcParticle()) == true){

    // loss
    m_EnergyLossHistos[iStation]->fill(aHit->energy(),1.0);

    // dE/dX
    m_timeOfFlightHistos[iStation]->fill(aHit->time()/Gaudi::Units::ns,1.0);
   
    // path length
    m_pathHistos[iStation]->fill(aHit->pathLength()/Gaudi::Units::micrometer);

    // BetaGamm
    m_betaGammaHistos[iStation]->fill(aParticle->beta()*aParticle->gamma());;

  }

  // scatter plot of x-y of hit
  m_XvsYHistos[iStation]->fill( mcHitPoint.x()/Gaudi::Units::cm,
                                mcHitPoint.y()/Gaudi::Units::cm, 1.0 );
  
  return StatusCode::SUCCESS;

}

int MCHitMonitor::getStationID(const double z) const {

  // loop and pick up the best z
  int iStation = -1;
  double zBest = 99999.;
  double zDiff;
  for (unsigned int iPos = 0; iPos< m_Zstations.size(); ++iPos){
    zDiff = fabs(m_Zstations[iPos] - z);  
    if (zDiff<zBest){
      zBest = zDiff;
      iStation = (int)iPos;
    }
  } // iPos

  // check match reasonable
  if (zBest>m_ZTolerance ) iStation = -1;

  return iStation;
}

//
// This File contains the implementation of the MCHitMonitor
//
//   Author:Matthew Needham
//   Created: 18-05-1999


// Standard C++ Library
#include <string>

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// LHCb Event
#include "Event/MCParticle.h"
#include "Event/MCHit.h"

// Units
#include "GaudiKernel/SystemOfUnits.h"

// Tools
#include "MCInterfaces/IMCParticleSelector.h"

#include "MCHitMonitor.h"

#include "LHCbMath/ModeFunctions.h"

// Needed for the creation of MCHitMonitor objects.
DECLARE_ALGORITHM_FACTORY( MCHitMonitor )

//_________________________________________________
/// MCHitMonitor
///
/// An MCHitMonitor is a Gaudi top level Algorithm that checks the 
/// output of the event tracks fitter


MCHitMonitor::MCHitMonitor(const std::string& name,
		     ISvcLocator* pSvcLocator):
  GaudiHistoAlg(name, pSvcLocator), m_selector(0)
{
  /// MCHitMonitor constructor
  this->declareProperty("mcPathString", m_MCHitPath="");
  this->declareProperty("zTolerance",m_ZTolerance = 20.0*Gaudi::Units::cm);
  this->declareProperty("zStations",m_Zstations);
  this->declareProperty("xMax",m_Xmax =400.0*Gaudi::Units::cm );
  this->declareProperty("yMax",m_Ymax = 400.0*Gaudi::Units::cm);
  this->declareProperty("selectorName", m_selectorName = "MCParticleSelector" );
  this->declareProperty("tMax", m_TMax =  100.0*Gaudi::Units::ns);
  this->declareProperty("tMin", m_TMin =  0.0*Gaudi::Units::ns);
  this->declareProperty("eMax", m_EMax =  1.0*Gaudi::Units::MeV);
  this->declareProperty("maxPath", m_MaxPath = 5.0*Gaudi::Units::cm);
  this->declareProperty("minPathLength", m_minPathLength = 0.1*Gaudi::Units::mm); 
  this->declareProperty("nToCollect", m_nToCollect = 20000);

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
  this->initHistograms();

  m_energyVec.reserve(m_nToCollect);

  return StatusCode::SUCCESS;  
}

void MCHitMonitor::initHistograms()
{
  /// Intialize histograms
  std::string tPath = this->histoPath()+"/"; 
  
 // make histograms per stations
 IHistogram1D* aHisto1D = 0;
 IHistogram2D* aHisto2D = 0;

 int nStation = m_Zstations.size();
 for (int iStation = 0 ;iStation<nStation; ++iStation){

   aHisto1D = book(100+iStation,
                   "time of Flight"+std::to_string(100+iStation),
                   m_TMin,m_TMax, 200);
   m_timeOfFlightHistos.push_back(aHisto1D);


   // x vs y plots
   aHisto2D = histoSvc()->book(
                          tPath+std::to_string(200+iStation),
                       "x vs y"+std::to_string(200+iStation),
                          200, -m_Xmax/Gaudi::Units::cm,m_Xmax/Gaudi::Units::cm,
                          200, -m_Ymax/Gaudi::Units::cm,m_Ymax/Gaudi::Units::cm);
   m_XvsYHistos.push_back(aHisto2D);

   aHisto1D  = book(300+iStation,"lossHisto"+std::to_string(300+iStation),
                    0., m_EMax,250);

   m_EnergyLossHistos.push_back(aHisto1D);   

 }//loop stations

 return;
}
 
StatusCode MCHitMonitor::execute()
{
  /// Executes MCHitMonitor for one event.
 
  const LHCb::MCHits* hitsCont = get<LHCb::MCHits>(m_MCHitPath);

  plot((double)hitsCont->size(),1 ,"num hits", 0.,5000, 100);
  counter("numberHits") += hitsCont->size();

  // loop over hits fill some histograms
  LHCb::MCHits::const_iterator iHit = hitsCont->begin() ;
  for ( ; iHit != hitsCont->end(); ++iHit ) {
    fillHistograms(*iHit).ignore();
  } // loop hits

  return StatusCode::SUCCESS;
}

StatusCode MCHitMonitor::finalize(){

  std::sort(m_energyVec.begin(), m_energyVec.end());
  const double shorth = ModeFunctions::shorth(m_energyVec.begin(), m_energyVec.end());
  const double halfWidth = ModeFunctions::halfSampleWidth(m_energyVec.begin(), m_energyVec.end());
  info() << "*** Summary ***" << endmsg;
  info() << "#hits per event: " << counter("numberHits").flagMean() << endmsg;
  info() << "Mean beta * gamma: " << counter("betaGamma").flagMean() << endmsg; 
  info() << "Most Probable deposited charge: " << shorth << endmsg;
  info() << "Half Sample width " << halfWidth << endmsg;

  return GaudiHistoAlg::finalize();
}


StatusCode MCHitMonitor::fillHistograms(const LHCb::MCHit* aHit) const{

  // Initialize

  // check 1 can find pointer to MCparticle
  const LHCb::MCParticle* aParticle = aHit->mcParticle();
  if (!aParticle) return Error("failed to find MCParticle");

  const LHCb::MCVertex* vertex = aParticle->originVertex();
  if (vertex){
    if (vertex->type() == LHCb::MCVertex::DeltaRay) ++counter("DeltaRay");
  }

  // p 
  plot(aParticle->p()/Gaudi::Units::GeV, 4, "pMag", 0.,100, 200);


  // average of entrance and exit...
  Gaudi::XYZPoint mcHitPoint = aHit->midPoint();

  plot(aHit->pathLength(),"path length", 0., m_MaxPath, 200);

  // bin in regions of z ~ stations
  const int iStation = getStationID(mcHitPoint.z());

  if (iStation<0) {
     err() << "failed to get station number for z " << mcHitPoint.z() << endmsg;
     return StatusCode::FAILURE;
  } 

  if (m_selector->accept(aHit->mcParticle()) == true){

    // loss
    m_EnergyLossHistos[iStation]->fill(aHit->energy(),1.0);

    // dE/dX
    m_timeOfFlightHistos[iStation]->fill(aHit->time()/Gaudi::Units::ns,1.0);
 
  }

  // scatter plot of x-y of hit
  m_XvsYHistos[iStation]->fill( mcHitPoint.x()/Gaudi::Units::cm,
                                mcHitPoint.y()/Gaudi::Units::cm, 1.0 );


  if (aHit->pathLength() > m_minPathLength) m_energyVec.push_back(aHit->energy());
 
  counter("betaGamma") += aParticle->beta()*aParticle->gamma() ;
 
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

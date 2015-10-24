//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudi
#include "VeloEffChecker.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "Event/MCHit.h"
#include "Event/VeloCluster.h"



#include "VeloDet/DeVelo.h"
#include "VeloDet/DeVeloSensor.h"

#include "AIDA/IHistogram2D.h"

// linkers
#include "Linker/LinkerTool.h"
using namespace LHCb;
using namespace Gaudi::Units;

DECLARE_ALGORITHM_FACTORY( VeloEffChecker )

VeloEffChecker::VeloEffChecker(const std::string& name,
                       ISvcLocator* pSvcLocator):
  GaudiHistoAlg(name, pSvcLocator),
  m_table(0),
  m_tracker(0),
  m_effOffset(1000),
  m_inEffOffset(2000)
{
  // constructor
  declareProperty("pMin", m_pMin = 1.0*GeV);
  declareProperty("tMax", m_tMax = 5.0*ns);

}

VeloEffChecker::~VeloEffChecker()
{
  // destructor
}
StatusCode VeloEffChecker::initialize()
{
  // Initializes TsaInitialization at the begin of program execution.

  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize");
  }

  setHistoTopDir("Velo/");

  m_tracker = getDet<DeVelo>(DeVeloLocation::Default);

  return StatusCode::SUCCESS;
}

StatusCode VeloEffChecker::execute(){


  AsctTool associator(evtSvc(),VeloClusterLocation::Default+"2MCHits" );
  m_table = associator.inverse();
  if (!m_table) {
    return Error("Failed to find table");
  }

  // histos per particle
  MCHits* hits = get<MCHits>(MCHitLocation::Velo);

  for (MCHits::iterator iterHit = hits->begin();
       iterHit != hits->end(); ++iterHit){
    if (((*iterHit)->p() > m_pMin )&& ((*iterHit)->time() < m_tMax )){
      if (inActive(*iterHit) == true) fillEffHistos(*iterHit);
    }

  } // iterHit


  return StatusCode::SUCCESS;
}


StatusCode VeloEffChecker::finalize(){

  info() << " -------Efficiency %--------- " << endmsg;

  const Histo2DMapID& histos = histo2DMapID();

  double avg = 0;
  unsigned int n = 0;
  unsigned int missed = 0;

  for ( Histo2DMapID::const_iterator entry = histos.begin() ;
        histos.end() != entry ; ++entry  )
  {
    // histogram ID
    AIDA::IHistogram2D* h  = entry->second ;
    if ( !entry->first.numeric() ) { continue ; }
    const int id = entry->first.numericID() ;
    if ( ( 0 == h ) || ( id >= m_effOffset ) )  { continue ;}

    // find the eff histo...
    Histo2DMapID::const_iterator effEntry = histos.find( id + m_effOffset) ;

    if (effEntry != histos.end())
    {
      int nAcc = h->allEntries();
      int nFound = effEntry->second->allEntries();
      double eff = 100.0*(double)nFound/(double)nAcc;
      double err = sqrt((eff*(100.0-eff))/(double)nAcc);
      info() << "Sensor " << entry->first << " Eff " << eff << " +/- "<< err << endmsg;
      ++n;
      avg += eff;
    }
    else {
      info() << "Sensor " << entry->first << "0 +/- 0" << endmsg;
      ++missed;
    }
  }

  info() << " Avg: " << avg/n << " Missed " <<  missed << endmsg;
  info() << " ---------------- " << endmsg;


  return GaudiHistoAlg::finalize();
}


void VeloEffChecker::fillEffHistos(const MCHit* aHit) {

  Gaudi::XYZPoint mPoint = aHit->midPoint();
  int sensorID = aHit->sensDetID();
  plot2D(mPoint.x()/cm,mPoint.y()/cm,sensorID,
          "x_vs_y_MCHit"+std::to_string(sensorID) , -5.0, 5.0, -5.0, 5.0 , 200, 200);

  // check if there is an associated cluster...
  Range range = m_table->relations(aHit);
  if ((range.empty() == false)){
    const VeloCluster* first = (*range.begin()).to();
     if (first->highThreshold())
     plot2D(mPoint.x()/cm,mPoint.y()/cm,sensorID + m_effOffset ,
            "x vs y cluster"+std::to_string(sensorID), -5., 5., -5., 5. , 200, 200);
  }
  else {
      plot2D(mPoint.x()/cm,mPoint.y()/cm, sensorID+ m_inEffOffset,
             "x vs y none"+std::to_string(sensorID), -5., 5., -5., 5. , 200, 200);
  }

}

bool VeloEffChecker::inActive(const MCHit* aHit) const{

  bool ok = true;
  const DeVeloSensor* sensor = m_tracker->sensor(aHit->sensDetID());
  if (sensor != 0){
    Gaudi::XYZPoint tPoint = sensor->globalToLocal(aHit->midPoint());
    StatusCode sc = sensor->isInActiveArea(tPoint);
    if (sc.isFailure()) ok = false;
  }
  return ok;
}

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"

// OTDet
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

/// BOOST
#include "boost/lexical_cast.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/foreach.hpp"

// local
#include "OTTimeMonitor.h"

/** @file OTTimeMonitor.cpp 
 *
 *  Implementation of OTTimeMonitor algorithm.
 *  @author J. van Tilburg & Jacopo Nardulli
 *  @date   20-07-2004
 */

using namespace LHCb;
using namespace boost;
using namespace boost::lambda;
using boost::lexical_cast;

/// Declaration of algorithm factory
DECLARE_ALGORITHM_FACTORY( OTTimeMonitor )

OTTimeMonitor::OTTimeMonitor(const std::string& name, 
			     ISvcLocator* pSvcLocator) :
  GaudiHistoAlg(name, pSvcLocator),
  m_decoder("OTRawBankDecoder"),
  m_tracker(NULL),
  m_nStations(0),
  m_firstStation(0),
  m_nEvents(0),
  m_nTimesHisto(NULL),
  m_occPerModuleHisto(NULL),
  m_calTimePerModulePr(NULL)
{
  // constructor
  declareProperty( "RawBankDecoder", m_decoder ) ;
}

OTTimeMonitor::~OTTimeMonitor() {
  // destructor
}

StatusCode OTTimeMonitor::initialize() {

  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  m_decoder.retrieve().ignore();
    
  /// set path
  if("" == histoTopDir()) setHistoTopDir("OT/");

  // Get OT Geometry from XML
  m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  m_nStations = m_tracker->nStation();
  m_firstStation = m_tracker->firstStation();

  // intialize histos
  initHistograms();

  return StatusCode::SUCCESS;
}

StatusCode OTTimeMonitor::execute() {

  ++m_nEvents;
  
  // multiplicity
  m_nTimesHisto->fill(m_decoder->totalNumberOfHits());
  
  // loop over all modules
  BOOST_FOREACH(const DeOTModule* module, m_tracker->modules()) {
    LHCb::OTChannelID modid = module->elementID();
    LHCb::OTLiteTimeRange ottimes = m_decoder->decodeModule(modid);
    BOOST_FOREACH(const LHCb::OTLiteTime& ottime, ottimes)
      fillHistograms( ottime, *module )  ;
  }
  
  return StatusCode::SUCCESS;
}
  
StatusCode OTTimeMonitor::finalize()
{
  double scalePerEvent = 1.0;  
  if( 0 != m_nEvents ) scalePerEvent = 1.0/double(m_nEvents);
  m_occPerModuleHisto->scale(scalePerEvent);
 
  m_decoder.release().ignore();
  return GaudiHistoAlg::finalize();
}

  
void OTTimeMonitor::initHistograms() {
 
  // Intialize histograms
  // number of Times in container
  m_nTimesHisto = book(1, "Number of OTTimes", 0., 20000., 200);

  // average drifttime per module
  size_t numMod = LHCb::OTChannelID::NumSequentialModule ;
  // occupancy per module
  m_occPerModuleHisto = book(30, "Occupancy per module",-0.5,numMod-0.5,numMod) ;
  m_calTimePerModulePr = bookProfile1D(31, "Average drifttime per module",
					 -0.5,numMod-0.5,numMod) ;

  // histograms per station
  int id;
  IHistogram1D* aHisto1D;
  unsigned int iStation = m_firstStation;
  for (; iStation <= m_nStations; ++iStation) {
    std::string stationToString = boost::lexical_cast<std::string>(iStation);
    
    // Calibrated time spectra
    id = 300 + iStation;
    aHisto1D = book(id, "Calibrated time in station "+stationToString,
		    -50.0*Gaudi::Units::ns, 175.0*Gaudi::Units::ns, 3*64);
    m_calTimeHistos.push_back(aHisto1D);
    
    // Tdc time spectra
    id = 100 + iStation;
    aHisto1D = book(id, "TDC counts in station "+stationToString,
		    -0.5, 255.5, 256 ) ;
    m_tdcTimeHistos.push_back( aHisto1D ) ;
  } // loop station
  
}

void OTTimeMonitor::fillHistograms(const LHCb::OTLiteTime& ottime,
				   const DeOTModule& module) {
  
  // Times and occupancy per station
  OTChannelID channel = ottime.channel();
  const int iStation = channel.station();
  
  // calibrated time for every station
  m_calTimeHistos[iStation-m_firstStation]->fill(ottime.calibratedTime());
  // raw time for every station
  m_tdcTimeHistos[iStation-m_firstStation]->fill(double(channel.tdcTime()));
  double weight = 1. / module.nChannels() ;
  m_occPerModuleHisto->fill( double(channel.sequentialUniqueModule()), weight ) ;
  m_calTimePerModulePr->fill( double(channel.sequentialUniqueModule()),ottime.calibratedTime() ) ;
}

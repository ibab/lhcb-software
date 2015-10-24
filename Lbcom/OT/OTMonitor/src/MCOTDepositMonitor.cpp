// from STD
#include <algorithm>

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/IRegistry.h"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// OTDet
#include "OTDet/DeOTDetector.h" 

// MCEvent
#include "Event/MCHit.h"
#include "Event/MCOTDeposit.h"

// MathCore
#include "GaudiKernel/Point3DTypes.h"

/// BOOST
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"

// local
#include "MCOTDepositMonitor.h"

/** @file MCOTDepositMonitor.cpp 
 *
 * Implementation of the MCOTDepositMonitor class
 *
 * @author M. Needham
 * @date   19-09-2000
 * @author adapted by J.Nardulli
 * @date   20-07-2004
 */

using namespace boost;
using namespace boost::lambda;

namespace MonitorHelpers {
  const std::string& findSpill( const LHCb::MCHit* aHit ) {
    const ObjectContainerBase* parentCont = aHit->parent();
    const IRegistry*                  reg = parentCont->registry();
    return ( reg->identifier() );
  }

  bool isSpill( const std::string& spill, const LHCb::MCHit* aHit ) {
    return ( findSpill( aHit ) == "/Event" + spill + LHCb::MCHitLocation::OT );
  }

  bool isSpillOver( const LHCb::MCOTDeposit* aDeposit ) {
    const LHCb::MCHit* aHit = aDeposit->mcHit();
    return aHit ? ( !isSpill( "/", aHit ) && aDeposit->isSignal() ): false ;
  }

  bool isCurrent( const LHCb::MCOTDeposit* aDeposit ) {
    const LHCb::MCHit* aHit = aDeposit->mcHit();
    return aHit ? ( isSpill( "/", aHit ) && aDeposit->isSignal() ) : false ;
  }
}


/// Declaration of algorithm factory
DECLARE_ALGORITHM_FACTORY( MCOTDepositMonitor )

MCOTDepositMonitor::MCOTDepositMonitor( const std::string& name, ISvcLocator* pSvcLocator )
  : GaudiHistoAlg(name, pSvcLocator)
  , m_det(0)
{
  // constructor
}

MCOTDepositMonitor::~MCOTDepositMonitor() {
  // destructor
}

StatusCode MCOTDepositMonitor::initialize() {

  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // set path
  if ( "" == histoTopDir() ) setHistoTopDir("OT/");

  // Get OT Geometry from XML
  m_det = getDet<DeOTDetector>( DeOTDetectorLocation::Default );

  // initialize histos
  initHistograms();

  return StatusCode::SUCCESS;
}

StatusCode MCOTDepositMonitor::execute() {

  // retrieve MCOTDeposits
  const LHCb::MCOTDeposits* deps = get<LHCb::MCOTDeposits>(LHCb::MCOTDepositLocation::Default);

  // number of deposits
  const unsigned nTotalDeposits        = deps->size();
  const unsigned nSignalDeposits       = std::count_if( deps->begin(), deps->end(), bind( &LHCb::MCOTDeposit::isSignal     , _1 ) );
  const unsigned nNoiseDeposits        = std::count_if( deps->begin(), deps->end(), bind( &LHCb::MCOTDeposit::isNoise      , _1 ) );
  const unsigned nXTalkDeposits        = std::count_if( deps->begin(), deps->end(), bind( &LHCb::MCOTDeposit::isXTalk      , _1 ) );
  const unsigned nDoublePulseDeposits  = std::count_if( deps->begin(), deps->end(), bind( &LHCb::MCOTDeposit::isDoublePulse, _1 ) );
  const unsigned nSpillOver            = std::count_if( deps->begin(), deps->end(), bind( &MonitorHelpers::isSpillOver     , _1 ) );
  const unsigned nCurrent              = std::count_if( deps->begin(), deps->end(), bind( &MonitorHelpers::isCurrent       , _1 ) );

  plot( double( nTotalDeposits       ), 
    10, "Total number of deposits"                         , -0.0005, 20000.5, 201 );
  plot( double( nSignalDeposits      ), 
    11, "Number of signal  deposits (including spill over)", -0.0005, 1.0005, 100 );
  double data;

  if( 0 < nSignalDeposits )
  { data = double( nCurrent ) / double( nSignalDeposits ); }
  else
  { data = -1.; }
  plot( data, 12, "Fraction of current deposits over signal and spill"   , -0.0005, 1.0005, 100 );

  if( 0 < nSignalDeposits )
  { data = double( nSpillOver ) / double( nSignalDeposits ); }
  else
  { data = -1.; }
  plot( data, 13, "Fraction of spill deposits over signal and spill", -0.0005, 1.0005, 100 );

  if( 0 <  nNoiseDeposits + nSignalDeposits )
  { data = double( nNoiseDeposits ) / double( nNoiseDeposits + nSignalDeposits ); }
  else
  { data = -1.; }
  plot( data, 14, "Fraction of noise deposits"       , -0.0005,     0.4, 100 );

  if( 0 <  nXTalkDeposits + nSignalDeposits )
  { data = double( nXTalkDeposits ) / double( nXTalkDeposits + nSignalDeposits ); }
  else
  { data = -1.; }
  plot( data, 15, "Fraction of XTalk deposits"       , -0.0005,     0.4, 100 );

  if( 0 <  nDoublePulseDeposits + nSignalDeposits )
  { data = double( nDoublePulseDeposits ) / double( nDoublePulseDeposits + nSignalDeposits ); }
  else
  { data = -1.; }
  plot( data, 16, "Fraction of double pulse deposits", -0.0005, 0.4, 100 );

  if( 0 < nTotalDeposits )
  { data = double( nSpillOver + nNoiseDeposits + nXTalkDeposits + nDoublePulseDeposits ) / double( nTotalDeposits ); }
  else
  { data = -1.; }
  plot( data, 17, "Total fraction of background", -0.0005, 1.0005, 100 );

  /// Histos per deposit
  std::for_each( deps->begin(), deps->end(), bind(&MCOTDepositMonitor::fillHistograms, this, _1) );

  return StatusCode::SUCCESS;
}

void MCOTDepositMonitor::initHistograms() {
  for( DeOTStation* station: m_det->stations() )
  {
    const unsigned    stationID       = station->stationID();
    const std::string stationToString = std::to_string( stationID );

    /// Deposit spectrum
    m_spectrumHistos[ depSpectrum( stationID ) ] = book( depSpectrum( stationID ) ,
      "Deposit spectrum for T" + stationToString,
      -50.0*Gaudi::Units::ns, 
      200.0*Gaudi::Units::ns, 
      250);
    /// noise spectrum

    m_spectrumHistos[ noiseSpectrum( stationID ) ] = book( noiseSpectrum( stationID ) ,
      "Noise spectrum for T" + stationToString,
      -50.0*Gaudi::Units::ns, 
      200.0*Gaudi::Units::ns, 
      250);

    /// xtalk spectrum
    m_spectrumHistos[ xTalkSpectrum( stationID ) ] = book( xTalkSpectrum( stationID ) ,
      "XTalk spectrum for T" + stationToString,
      -50.0*Gaudi::Units::ns, 
      200.0*Gaudi::Units::ns, 
      250);

    /// double pulse spectrum
    m_spectrumHistos[ dPulseSpectrum( stationID ) ] = book( dPulseSpectrum( stationID ) ,
      "Double pulse spectrum for T" + stationToString,
      -50.0*Gaudi::Units::ns, 
      200.0*Gaudi::Units::ns, 
      250);

    /// y vs x
    m_hitDistHistos[ stationID ]     = book2D( 500 + stationID,
      "Hit distribution y(cm) vs x(cm) for station " + stationToString,
      -3500./Gaudi::Units::cm,3500./Gaudi::Units::cm, 700,
      -3500./Gaudi::Units::cm,3500./Gaudi::Units::cm, 700 );
  }
}

void MCOTDepositMonitor::fillHistograms( const LHCb::MCOTDeposit* aDeposit ) {
  /// Get channel
  const LHCb::OTChannelID& channel = aDeposit->channel();

  /// # deposits per station 
  unsigned stationID = channel.station();
  plot( double( stationID ), 20, "Number of deposits per station", 0.5, 3.5, 3 );
  /// # deposits per layer
  unsigned layerID   = 10u*channel.station() + channel.layer();
  plot( double( layerID ),   21, "Number of deposits per layer"  , 9.5, 33.5, 24 );
  /// # deposits per quarter
  unsigned quarterID = 100u*channel.station() + 10u*channel.layer() + channel.quarter();
  plot( double( quarterID ), 22, "Number of deposits per quarter", 99.5, 333.5, 234 );
  /// # deposits per module
  unsigned moduleID  = 1000u*channel.station() + 100u*channel.layer() + 10u*channel.quarter() + channel.module();
  plot( double( moduleID ),  23, "Number of deposits per module" , 1000.5, 3339.5, 2339 );

  m_spectrumHistos[ depSpectrum( stationID ) ]->fill( aDeposit->time() );
  if ( aDeposit->isNoise()       ) m_spectrumHistos[ noiseSpectrum( stationID )  ]->fill( aDeposit->time() );
  if ( aDeposit->isXTalk()       ) m_spectrumHistos[ xTalkSpectrum( stationID )  ]->fill( aDeposit->time() );
  if ( aDeposit->isDoublePulse() ) m_spectrumHistos[ dPulseSpectrum( stationID ) ]->fill( aDeposit->time() );

  const LHCb::MCHit* aHit = aDeposit->mcHit();
  if ( aHit ) {
    /// retrieve entry + exit points and take average
    const Gaudi::XYZPoint& mcHitPoint = aHit->midPoint();
    //fill y vs x scatter plots    
    m_hitDistHistos[ stationID ]->fill(mcHitPoint.x()/Gaudi::Units::cm, mcHitPoint.y()/Gaudi::Units::cm);
    if ( layerID ==  10 ) plot2D( mcHitPoint.x()/Gaudi::Units::cm, mcHitPoint.y()/Gaudi::Units::cm, 600,
      "Hit distribution y(cm) vs x(cm) for T1X1", 
      -3500./Gaudi::Units::cm,3500./Gaudi::Units::cm, 
      -3500./Gaudi::Units::cm,3500./Gaudi::Units::cm, 700, 700 );

    if ( aDeposit->isSignal() ) plot( aDeposit->driftDistance(), 1, "Drift distance", -0.005, 3.0, 100 );

    /// OK so we only do this for T3, but in the end we want to do it for all stations
    if ( stationID == 3 && aDeposit->isSignal() ) {
      plot( aDeposit->time(), 30, "Spectrum of spills in T3",
        -50.0*Gaudi::Units::ns,
        200.0*Gaudi::Units::ns,
        250);
      if      ( MonitorHelpers::isSpill( "/",          aHit ) ) plot( aDeposit->time(), 31, "Spectrum of current spill in T3",
        -50.0*Gaudi::Units::ns,
        200.0*Gaudi::Units::ns,
        250);
      else if ( MonitorHelpers::isSpill( "/PrevPrev/", aHit ) ) plot( aDeposit->time(), 32, "Spectrum of PrevPrev spill in T3",
        -50.0*Gaudi::Units::ns,
        200.0*Gaudi::Units::ns,
        250);
      else if ( MonitorHelpers::isSpill( "/NextNext/", aHit ) ) plot( aDeposit->time(), 33, "Spectrum of NextNext spill in T3",
        -50.0*Gaudi::Units::ns,
        200.0*Gaudi::Units::ns,
        250);
      else if ( MonitorHelpers::isSpill( "/Prev/",     aHit ) ) plot( aDeposit->time(), 34, "Spectrum of Prev spill in T3",
        -50.0*Gaudi::Units::ns,
        200.0*Gaudi::Units::ns,
        250);
      else if ( MonitorHelpers::isSpill( "/Next/",     aHit ) ) plot( aDeposit->time(), 35, "Spectrum of Next spill in T3",
        -50.0*Gaudi::Units::ns,
        200.0*Gaudi::Units::ns,
        250);
    }
  }
}

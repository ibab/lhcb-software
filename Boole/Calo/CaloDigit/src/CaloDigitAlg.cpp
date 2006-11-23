// $Id: CaloDigitAlg.cpp,v 1.14 2006-11-23 14:15:52 cattanem Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"

// Event 
#include "Event/MCTruth.h"

// CaloEvent
#include "Event/MCCaloDigit.h"
#include "Event/CaloAdc.h"
#include "Event/L0CaloAdc.h"
#include "Event/L0PrsSpdHit.h"

// local
#include "CaloDigitAlg.h"

// ============================================================================
/** @file CaloDigitAlg.cpp
 *
 *    Calorimeter Digitisation, including noise, and Zero suppression.
 *
 *    @author: Olivier Callot
 *    @date:   5 June 2000
 */
// ============================================================================

DECLARE_ALGORITHM_FACTORY( CaloDigitAlg );

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
CaloDigitAlg::CaloDigitAlg( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator            )
  , m_inputPrevData     ( ""    )
  , m_rndmSvc           (  0    )
  , m_coherentNoise     ( 0.3   )
  , m_incoherentNoise   ( 1.2   )
  , m_gainError         ( 0.01  )
  , m_fracPrev          ( 0.20  )
  , m_pedShift          ( 0.00  )
  , m_pePerMeV          ( 0.    )
  , m_deadCellFraction  ( 0.00  )
  , m_triggerEtScale    ( 20 * Gaudi::Units::MeV )
  , m_triggerThreshold  ( 0. )
  , m_zSupThreshold     ( -100000 )
{
  //** Declare the algorithm's properties which can be set at run time and
  //** their default values
  declareProperty("DetectorName"       , m_detectorName     ) ;
  declareProperty("InputData"          , m_inputData        ) ;
  declareProperty("OutputData"         , m_outputData       ) ;
  declareProperty("InputPrevData"      , m_inputPrevData    ) ;
  declareProperty("CoherentNoise"      , m_coherentNoise    ) ;
  declareProperty("IncoherentNoise"    , m_incoherentNoise  ) ;
  declareProperty("GainError"          , m_gainError        ) ;
  declareProperty("FractionPrevious"   , m_fracPrev         ) ;
  declareProperty("PedestalShift"      , m_pedShift         ) ;
  declareProperty("PhotoElectronPerMeV", m_pePerMeV         ) ;
  declareProperty("DeadCellFraction"   , m_deadCellFraction ) ;
  declareProperty("TriggerThreshold"   , m_triggerThreshold ) ;  
  declareProperty("TriggerEtScale"     , m_triggerEtScale   ) ;
  declareProperty("ZSupThreshold"      , m_zSupThreshold    ) ;
  
  std::string begName = name.substr( 0, 8 );

  //== Get the previous MC Calo Digit contained according to rootOnTES and context
  std::string prevRoot = "none";
  if ( "Prev1/" == rootOnTES() ) {
    prevRoot = "Prev2/";
  } else if ( "" == rootOnTES() ) {
    if ( "TAE" == context() ) {
      prevRoot = "Prev1/";
    } else {
      prevRoot = "Prev";
    }
  } else if ( "Next1/" == rootOnTES() ) {
    prevRoot = "";
  } else if ( "Next2/" == rootOnTES() ) {
    prevRoot = "Next1/";
  }

  if ( "SpdDigit" == begName ) {
    m_detectorName     = DeCalorimeterLocation::Spd;
    m_inputData        = rootOnTES() + LHCb::MCCaloDigitLocation::Spd;
    m_outputData       = rootOnTES() + LHCb::CaloAdcLocation::Spd;
    if ( "none" != prevRoot ) m_inputPrevData = prevRoot + LHCb::MCCaloDigitLocation::Spd;
    m_pePerMeV         = 10.;
    m_coherentNoise    = 0.0;
    m_incoherentNoise  = 0.0;
    m_gainError        = 0.0;
    m_triggerName      = rootOnTES() + LHCb::L0PrsSpdHitLocation::Spd;
    m_triggerThreshold = 0.1 * Gaudi::Units::MeV;
    m_triggerIsBit     = true;
    m_zSupThreshold    = 10;  //== No ADC for SPD, only trigger bit...
  } else if ( "PrsDigit" == begName ) {
    m_detectorName     = DeCalorimeterLocation::Prs;
    m_inputData        = rootOnTES() + LHCb::MCCaloDigitLocation::Prs;
    m_outputData       = rootOnTES() + LHCb::CaloAdcLocation::Prs;
    if ( "none" != prevRoot ) m_inputPrevData = prevRoot + LHCb::MCCaloDigitLocation::Prs;
    m_pePerMeV         = 10.;
    m_coherentNoise    = 0.0;
    m_incoherentNoise  = 1.0;
    m_triggerName      = rootOnTES() + LHCb::L0PrsSpdHitLocation::Prs;
    m_triggerThreshold = 10. * Gaudi::Units::MeV;
    m_triggerIsBit     = true;
    m_zSupThreshold    = 15;
  } else if ( "EcalDigi" == begName ) {
    m_detectorName     = DeCalorimeterLocation::Ecal;
    m_inputData        = rootOnTES() + LHCb::MCCaloDigitLocation::Ecal;
    m_outputData       = rootOnTES() + LHCb::CaloAdcLocation::FullEcal;
    m_pedShift         = 0.40;
    m_triggerName      = rootOnTES() + LHCb::L0CaloAdcLocation::Ecal;
    m_triggerIsBit     = false;
 
    m_corrArea.push_back( 1.00 );
    m_corrArea.push_back( 1.04 );
    m_corrArea.push_back( 1.08 );
    
 } else if ( "HcalDigi" == begName ) {
    m_detectorName     = DeCalorimeterLocation::Hcal;
    m_inputData        = rootOnTES() + LHCb::MCCaloDigitLocation::Hcal;
    m_outputData       = rootOnTES() + LHCb::CaloAdcLocation::FullHcal;
    m_pedShift         = 0.40;
    m_triggerName      = rootOnTES() + LHCb::L0CaloAdcLocation::Hcal;
    m_triggerIsBit     = false;

    m_corrArea.push_back( 1.00 );
    m_corrArea.push_back( 1.05 );
  }

};

//=============================================================================
// Standard destructor
//=============================================================================
CaloDigitAlg::~CaloDigitAlg() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CaloDigitAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if( sc.isFailure() ) return sc;

  // Retrieve the calorimeter we are working with.
  m_calo = getDet<DeCalorimeter>( m_detectorName );

  //*** Initialize the random number service

  m_rndmSvc = svc< IRndmGenSvc>( "RndmGenSvc" , true );

  //** Number of Cells and other detector parameters

  m_numberOfCells    = m_calo->numberOfCells();
  m_activeToTotal    = m_calo->activeToTotal() ;
  m_saturatedAdc     = m_calo->adcMax();
  m_maxAdcValue      = (int) ( 0.95 * m_saturatedAdc );
  if( m_maxAdcValue == 0 ) { m_maxAdcValue = 1; }    // For SPD, one bit output

  m_pedestalShift = m_pedShift * (m_incoherentNoise + m_coherentNoise);

  if ( 0 != m_incoherentNoise + m_coherentNoise ) {
    info() << format( "Noise:%5.1f +%5.1f (coherent) counts.",
                      m_incoherentNoise, m_coherentNoise ) << endreq;
  }
  if ( 0 != m_deadCellFraction ) info() << m_deadCellFraction << " of the cells as dead (gain=0)." << endreq;
  if ( 0 != m_gainError        ) info() << format( "Gain error %4.1f%%", m_gainError*100. ) << endreq;
  if ( 0 != m_pedestalShift    ) info() << format( "Pedestal shift %5.2f", m_pedestalShift ) << endreq;
  if ( "" != m_inputPrevData   ) info() << format( "Subtract %6.4f of previous BX ", m_fracPrev) << endreq;
  if ( -100 < m_zSupThreshold )  info() << "ZSupThreshold " << m_zSupThreshold << endreq;
  if ( 0 < m_corrArea.size() ) {
    info() << "Trigger correction factor per area : ";
    for ( unsigned int kk = 0 ; m_corrArea.size() > kk ; kk++ ) {
      info() << m_corrArea[kk] << " ";
    }
    info() << endreq;
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitAlg::execute() {

  bool isDebug = (msgLevel() <= MSG::DEBUG );

  //*** get the input data
  
  LHCb::MCCaloDigits* mcDigits = get<LHCb::MCCaloDigits>( m_inputData );
  
  //=== Get the previous BX's data if needed
  LHCb::MCCaloDigits* prevDigits = 0;
  if ( "" != m_inputPrevData ) {
    if ( exist<LHCb::MCCaloDigits>( m_inputPrevData ) ) prevDigits = get<LHCb::MCCaloDigits>( m_inputPrevData );
  }

  //***  prepare and register the output container it into the Transient Store!

  LHCb::CaloAdcs* adcs = new LHCb::CaloAdcs();
  put( adcs, m_outputData );
  LHCb::L0CaloAdcs* trigBank = NULL;
  LHCb::L0PrsSpdHits* bitsBank = NULL;
  if ( "" != m_triggerName ) {
    if ( m_triggerIsBit ) {
      bitsBank = new LHCb::L0PrsSpdHits();
      put( bitsBank, m_triggerName );
    } else {
      trigBank = new LHCb::L0CaloAdcs();
      put( trigBank, m_triggerName );
    }
  }


  debug() << "Processing " << mcDigits->size() << " mcDigits." 
          << endreq;

  // == prepare buffers for energies and tags. Size is only real cells.
  // == fill with MCCaloDigit, using the calibration activeToTotal

  std::vector<double> caloEnergies ( m_numberOfCells, 0.0 ) ;
  for( LHCb::MCCaloDigits::const_iterator mcDig = mcDigits->begin() ;
       mcDigits->end() != mcDig ; ++mcDig ) {
    if( 0 != *mcDig ) {
      int index = m_calo->cellIndex( (*mcDig)->cellID() );
      caloEnergies[index] = (*mcDig)->activeE() * m_activeToTotal ;
    }
  }

  //== generate coherent noise for this event, add the pedestal shift
  Rndm::Numbers CoherentNoise( rndmSvc() , 
                               Rndm::Gauss( 0.0 , m_coherentNoise ) );
  double offset = CoherentNoise() + m_pedestalShift;

  //== generate 2 sequences of random numbers, for gain error and noise
  std::vector<double> gainErrors ( m_numberOfCells, 1.0 );
  if( 0 < m_gainError      ) {
    std::generate( gainErrors.begin() , gainErrors.end () ,
                   Rndm::Numbers( rndmSvc(), Rndm::Gauss( 1.0 , m_gainError ) ) );
  }

  std::vector<double> incoherentNoise   ( m_numberOfCells, 0.0 );
  if( 0 < m_incoherentNoise ) {
    std::generate( incoherentNoise.begin(), incoherentNoise.end() ,
                   Rndm::Numbers( rndmSvc(), Rndm::Gauss( 0.0 , m_incoherentNoise ) ) );
  }

  //== Add dead cells : Set the gain to zero in this cell
  if ( 0 != m_deadCellFraction ) {
    std::vector<double> probaDead ( m_numberOfCells, 1. );
    std::generate( probaDead.begin() , probaDead.end () ,
                   Rndm::Numbers( rndmSvc() , Rndm::Flat( 0.0, 1.0 ) ) );
    for ( unsigned int kk = 0; probaDead.size() > kk; kk++ ) {
      if ( m_deadCellFraction > probaDead[kk] ) {
        gainErrors[kk] = 0.;
      }
    }
  }  

  //== loop over all cells in the detector

  for( std::vector<double>::iterator cell = caloEnergies.begin() ;
       cell != caloEnergies.end() ; ++cell ) {

    const unsigned int index = cell - caloEnergies.begin() ;
    const LHCb::CaloCellID   id    = m_calo->cellIdByIndex( index );

    //== extract the PMT gain for the given channel , translate the energy to
    //   ADC scale, take into account coherent and incoherent noise and
    //   saturation to get adc count and transform again into energy.

    double energy = *cell ;    //Get the energy from the array

    //== generate the photo-electron fluctuation...
    if ( 0. < energy && 0. < m_pePerMeV ) {
      Rndm::Numbers nPe ( rndmSvc(), Rndm::Poisson( energy * m_pePerMeV) );
      energy = nPe() / m_pePerMeV;
    }

    double gain     = m_calo->cellGain( id ) ;
    double adcValue = gainErrors[index] * energy / gain +  incoherentNoise[index] + offset;
    if ( 0 != prevDigits ) {

      // Correct for spill-over in Prs/Spd by a fixed fraction.
      // Do it on signal, as Spd has a single bit adc...
 
      LHCb::MCCaloDigit* prevMc = prevDigits->object( id );
      if ( 0 != prevMc ) {
        double prevEnergy = prevMc->activeE() * m_activeToTotal;

        //== generate the photo-electron fluctuation on the previous BX signal
        if ( 0. < prevEnergy && 0. < m_pePerMeV ) {
          Rndm::Numbers pe ( rndmSvc(), Rndm::Poisson( prevEnergy * m_pePerMeV));
          prevEnergy = pe() / m_pePerMeV;
        }

        double cor = m_fracPrev * prevEnergy / gain;
        if ( isDebug && .5 < cor ) {
          debug() << id << format( " adc%7.1f correct%7.1f => %7.1f",
                                   adcValue, cor, adcValue-cor ) 
                  << endreq;
        }
        adcValue -= cor;
      }
    }
  
    int  intAdc  = (int) floor( adcValue + 0.5 );
    if ( intAdc > m_maxAdcValue) { intAdc = m_saturatedAdc ; } 
    if ( m_zSupThreshold <= intAdc ) {
      LHCb::CaloAdc* adc = new LHCb::CaloAdc( id, intAdc );
      adcs->insert( adc ) ;
    }
    
    int trigVal   = 0;
    if ( m_triggerIsBit ) {
      if ( m_triggerThreshold < intAdc * gain ) {
        LHCb::L0PrsSpdHit* myHit = new LHCb::L0PrsSpdHit( id );
        bitsBank->insert( myHit );
      }
    } else {
      double et = intAdc * gain * m_corrArea[ id.area() ] * m_calo->cellSine( id );
      trigVal = (int)floor( et / m_triggerEtScale + .5 );
      if ( 255 < trigVal ) trigVal = 255;
      if ( 0   > trigVal ) trigVal = 0;
      if ( 0 < trigVal ) {
        if ( isDebug ) {
          debug() << id << format( " adc%6d et%8.1f trigAdc%4d",
                                   intAdc, et, trigVal ) 
                  << endreq;
        }
        LHCb::L0CaloAdc* trigAdc = new LHCb::L0CaloAdc( id, trigVal );
        trigBank->insert( trigAdc );
      }
    }
  }
  int trigSize = 0;
  if ( m_triggerIsBit ) {
    trigSize = bitsBank->size();
  } else {
    trigSize = trigBank->size();
  }

  debug() << format( "Have digitized and stored %5d adcs and %5d trigger from %5d MCDigits.",
                     adcs->size(), trigSize, mcDigits->size() )
          << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================

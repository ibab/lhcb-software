// $Id: CaloDigitAlg.cpp,v 1.7 2005-01-12 09:14:33 ocallot Exp $

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"

// Event 
#include "Event/MCTruth.h"
// Calo/CaloKernel
#include "CaloKernel/CaloVector.h"

// CaloEvent
#include "Event/MCCaloDigit.h"
#include "Event/CaloDigit.h"

// local
#include "CaloDigitAlg.h"

// ============================================================================
/** @file CaloDigitAlg.cpp
 *
 *    Calorimeter Digitisation, including noise, and Zero suppression.
 *
 *    @author: Olivier Callot
 *    @Date:   5 June 2000
 */
// ============================================================================


static const AlgFactory<CaloDigitAlg>          Factory ;
const       IAlgFactory& CaloDigitAlgFactory = Factory ;


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
  
  if ( "SpdDigit" == name ) {
    m_detectorName    = "/dd/Structure/LHCb/Spd";
    m_inputData       = MCCaloDigitLocation::Spd;
    m_outputData      = CaloDigitLocation::FullSpd;
    m_inputPrevData   = "Prev/"+MCCaloDigitLocation::Spd;
    m_pePerMeV        = 10.;
    m_coherentNoise   = 0.0;
    m_incoherentNoise = 0.0;
    m_gainError       = 0.0;
  } else if ( "PrsDigit" == name ) {
    m_detectorName    = "/dd/Structure/LHCb/Prs";
    m_inputData       = MCCaloDigitLocation::Prs;
    m_outputData      = CaloDigitLocation::FullPrs;
    m_inputPrevData   = "Prev/"+MCCaloDigitLocation::Prs;
    m_pePerMeV        = 10.;
    m_coherentNoise   = 0.0;
    m_incoherentNoise = 1.0;
  } else if ( "EcalDigit" == name ) {
    m_detectorName    = "/dd/Structure/LHCb/Ecal";
    m_inputData       = MCCaloDigitLocation::Ecal;
    m_outputData      = CaloDigitLocation::FullEcal;
    m_pedShift        = 0.40;
} else if ( "HcalDigit" == name ) {
    m_detectorName    = "/dd/Structure/LHCb/Hcal";
    m_inputData       = MCCaloDigitLocation::Hcal;
    m_outputData      = CaloDigitLocation::FullHcal;
    m_pedShift        = 0.40;
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

  info() << format( "Noise:%5.1f +%5.1f (coherent) counts.",
                    m_incoherentNoise, m_coherentNoise );
  info() << format( " Gain error:%4.1f%%, pedestal shift:%5.2f",
                    m_gainError * 100., m_pedestalShift );
  if ( "" != m_inputPrevData ) {
    info() << " Subtract " << m_fracPrev << " of previous BX.";
  }
  if ( 0 != m_deadCellFraction ) {
    info() << ", " << m_deadCellFraction << " of the cells as dead (gain=0).";
  }
  info() << endreq;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitAlg::execute() {

  bool isDebug = (msgLevel() <= MSG::DEBUG );

  //*** get the input data
  
  MCCaloDigits* mcDigits = get<MCCaloDigits>( m_inputData );
  
  //=== Get the previous BX's data if needed
  MCCaloDigits* prevDigits = 0;
  if ( "" != m_inputPrevData )  prevDigits = get<MCCaloDigits>( m_inputPrevData );

  //***  prepare and register the output container it into the Transient Store!

  CaloDigits* digits = new CaloDigits();
  put( digits, m_outputData );

  debug() << "Processing " << mcDigits->size() << " mcDigits." 
          << endreq;

  // == prepare buffers for energies and tags. Size is only real cells.
  // == fill with MCCaloDigit, using the calibration activeToTotal

  std::vector<double> caloEnergies ( m_numberOfCells, 0.0 ) ;
  for( MCCaloDigits::const_iterator mcDig = mcDigits->begin() ;
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
    const CaloCellID   id    = m_calo->cellIdByIndex( index );

    //== extract the PMT gain for the given channel , translate the energy to
    //   ADC scale, take into account coherent and incoherent noise and
    //   saturation to get adc count and transform again into energy.

    double energy = *cell ;    //Get the energy from the array

    //== generate the photo-electron fluctuation...
    if ( 0. < energy && 0. < m_pePerMeV ) {
      Rndm::Numbers nPe ( rndmSvc(), Rndm::Poisson( energy * m_pePerMeV) );
      energy = nPe() / m_pePerMeV;
    }

    double gain   = m_calo->cellGain( id ) ;
    double adc    = gainErrors[index] * energy / gain ;
    adc          += incoherentNoise[index] + offset;
    if ( 0 != prevDigits ) {

      // Correct for spill-over in Prs/Spd by a fixed fraction.
      // Do it on signal, as Spd has a single bit adc...
 
      MCCaloDigit* prevMc = prevDigits->object( id );
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
                                   adc, cor, adc-cor ) 
                  << endreq;
        }
        adc = adc - cor;
      }
    }
    energy =  gain * ( adcCount(adc) - m_pedestalShift ) ;
    CaloDigit* digit = new CaloDigit( id, energy );
    digits->insert( digit ) ;
  }

  debug() << format( "Have digitized and stored %5d digits from %5d MCDigits.",
                     digits->size(), mcDigits->size() )
          << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================

// $Id: CaloDigitAlg.cpp,v 1.1.1.1 2002-11-04 10:29:51 ocallot Exp $
// STL
#include <string>
#include <stdio.h>
// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/ObjectVector.h"
// Event 
#include "Event/MCTruth.h"
// Calo/CaloKernel
#include "CaloKernel/CaloVector.h"
#include "CaloKernel/CaloException.h"
// CaloEvent
#include "Event/MCCaloDigit.h"
#include "Event/CaloDigit.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// local
#include "CaloDigitAlg.h"

// ============================================================================
/** @file CaloDigitAlg.cpp
 *
 *    Calorimeter Digitisation, including noise, and Zero suppression.
 *    (1) modified by Vanya Belyaev (end of July 2000),
 *    (2) modified by Olivier Callot, September 2000
 *
 *    @author: Olivier Callot
 *    @Date:   5 June 2000
 */
// ============================================================================

// MANDATORY!!!
static const AlgFactory<CaloDigitAlg>          Factory ;
const       IAlgFactory& CaloDigitAlgFactory = Factory ;


//=============================================================================
// Standard creator, initializes variables
//=============================================================================
CaloDigitAlg::CaloDigitAlg( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : CaloAlgorithm ( name , pSvcLocator            )
  , m_inputPrevData     ( ""    )
  , m_rndmSvc           (  0    )
  , m_coherentNoise     ( 0.3   )
  , m_incoherentNoise   ( 1.2   )
  , m_gainError         ( 0.01  )
  , m_fracPrev          ( 0.15  )
  , m_pedShift          ( 0.00  )

{
  //** Declare the algorithm's properties which can be set at run time and
  //** their default values
  declareProperty("InputPrevData"      , m_inputPrevData   ) ;
  declareProperty("CoherentNoise"      , m_coherentNoise   ) ;
  declareProperty("IncoherentNoise"    , m_incoherentNoise ) ;
  declareProperty("GainError"          , m_gainError       ) ;
  declareProperty("FractionPrevious"   , m_fracPrev        ) ;
  declareProperty("PedestalShift"      , m_pedShift        ) ;
  declareProperty("PhotoElectronPerMeV", m_pePerMeV        ) ;

  if ( "SpdDigit" == name ) {
    setDetData(       "/dd/Structure/LHCb/Spd"    );
    setInputData(     MCCaloDigitLocation::Spd    );
    setOutputData(    "Raw/Spd/AllDigits"         );
    m_inputPrevData   = "Prev/"+MCCaloDigitLocation::Spd;
    m_pePerMeV        = 40.;
    m_coherentNoise   = 0.0;
    m_incoherentNoise = 0.0;
    m_gainError       = 0.0;
  } else if ( "PrsDigit" == name ) {
    setDetData(       "/dd/Structure/LHCb/Prs"    );
    setInputData(     MCCaloDigitLocation::Prs    );
    setOutputData(    "Raw/Prs/AllDigits"         );
    m_inputPrevData   = "Prev/"+MCCaloDigitLocation::Prs;
    m_pePerMeV        = 40.;
    m_coherentNoise   = 0.0;
    m_incoherentNoise = 1.0;
  } else if ( "EcalDigit" == name ) {
    setDetData(       "/dd/Structure/LHCb/Ecal"    );
    setInputData(     MCCaloDigitLocation::Ecal    );
    setOutputData(    "Raw/Ecal/AllDigits"         );
    m_pePerMeV        = 0.;
    m_pedShift        = 0.40;
  } else if ( "HcalDigit" == name ) {
    setDetData(       "/dd/Structure/LHCb/Hcal"    );
    setInputData(     MCCaloDigitLocation::Hcal    );
    setOutputData(    "Raw/Hcal/AllDigits"         );
    m_pePerMeV        = 0.;
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

  MsgStream msg(msgSvc(), name());

  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) { 
    return Error("Can not initialize the base class  CaloAlgorithm!", sc); 
  } 

  // Retrieve the calorimeter we are working with.
  m_calo = get( detSvc() , detData() , (DeCalorimeter*) 0 );
  if( 0 == m_calo ) { return StatusCode::FAILURE ; }

  //*** Initialize the random number service

  m_rndmSvc = 0;
  StatusCode status = service("RndmGenSvc" , m_rndmSvc , true );

  if ( status.isFailure() ) {
    msg << MSG::ERROR << "Can not initialize the RndmGenSvc"
        << status << endreq;
  }
  if( 0 == rndmSvc()      ) {
    msg << MSG::ERROR << "Can not initialize the RndmGenSvc" << endreq;
    status = StatusCode::FAILURE ;
  }

  //** Number of Cells and other detector parameters

  m_numberOfCells    = m_calo->numberOfCells();
  m_activeToTotal    = m_calo->activeToTotal() ;
  m_saturatedAdc     = m_calo->adcMax();
  m_maxAdcValue      = (int) ( 0.95 * m_saturatedAdc );
  if( m_maxAdcValue == 0 ) { m_maxAdcValue = 1; }    // For SPD, one bit output

  m_pedestalShift = m_pedShift * (m_incoherentNoise + m_coherentNoise);
  if( status.isSuccess() ) {
    msg << MSG::INFO 
        << format( "Noise:%5.1f +%5.1f (coherent) counts.",
                   m_incoherentNoise, m_coherentNoise );
    msg << format( " Gain error:%4.1f%%, pedestal shift:%5.2f",
                   m_gainError * 100., m_pedestalShift );
    if ( "" != m_inputPrevData ) {
      msg << " Subtract " << m_fracPrev << " of previous BX.";
    }
    msg << endreq;
  }
  return status;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitAlg::execute() {

  MsgStream msg( msgSvc(), name() );
  bool debug = (msg.level() <= MSG::DEBUG );

  //*** get the input data

  MCCaloDigits* mcDigits = get( eventSvc(), inputData(), mcDigits );

  //=== Get the previous BX's data if needed
  MCCaloDigits* prevDigits = 0;
  if ( "" != m_inputPrevData ) {
    SmartDataPtr<MCCaloDigits> mcPrev ( eventSvc(), m_inputPrevData );
    if( 0 == mcPrev ) {
      msg << MSG::ERROR << "Unable to retrieve input container=" 
          << m_inputPrevData << endreq;
      return StatusCode::FAILURE;
    }
    prevDigits = mcPrev;
  }

  //***  prepare and register the output container it into the Transient Store!

  CaloDigits* digits = new CaloDigits();
  StatusCode sc = put( digits, outputData() );
  if( sc.isFailure() ) { return sc ; }

  msg << MSG::DEBUG << "Processing " << mcDigits->size() << " mcDigits." 
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
                   Rndm::Numbers( rndmSvc() ,
                                  Rndm::Gauss( 1.0 , m_gainError ) ) );
  }

  std::vector<double> incoherentNoise   ( m_numberOfCells, 0.0 );
  if( 0 < m_incoherentNoise ) {
    std::generate( incoherentNoise.begin () , incoherentNoise.end   () ,
                   Rndm::Numbers( rndmSvc() ,
                                  Rndm::Gauss( 0.0 , m_incoherentNoise ) ) );
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
      Rndm::Numbers nPe ( rndmSvc() , 
                          Rndm::Poisson( energy * m_pePerMeV) );
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
          Rndm::Numbers pe ( rndmSvc() , 
                             Rndm::Poisson( prevEnergy * m_pePerMeV));
          prevEnergy = pe() / m_pePerMeV;
        }

        double cor = m_fracPrev * prevEnergy / gain;
        if ( debug && .5 < cor ) {
          msg << MSG::DEBUG << id 
              << format( " adc%7.1f correct%7.1f => %7.1f",
                         adc, cor, adc-cor ) 
              << endreq;
        }
        adc = adc - cor;
      }
    }
    energy =  gain * ( adcCount(adc) - m_pedestalShift ) ;
    CaloDigit* digit = new CaloDigit( id, energy );
    digits->add( digit ) ;
  }

  msg << MSG::DEBUG
      << format( "Have digitized and stored %5d digits from %5d MCDigits.",
                 digits->size(), mcDigits->size() )
      << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloDigitAlg::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << " >>> Finalize" << endreq;

  if( 0 != m_rndmSvc ) { m_rndmSvc->release() ; m_rndmSvc = 0 ; }

  return CaloAlgorithm::finalize();
}

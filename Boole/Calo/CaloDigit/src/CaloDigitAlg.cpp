// $Id: CaloDigitAlg.cpp,v 1.21 2008-06-30 15:51:51 odescham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "GaudiUtils/Aida2ROOT.h"

// Event 
#include "Event/MCTruth.h"

// CaloEvent
#include "Event/MCCaloDigit.h"
#include "Event/CaloAdc.h"
#include "Event/L0CaloAdc.h"
#include "Event/L0PrsSpdHit.h"

// ROOT
#include "TH1D.h"
#include "TF1.h"

// local
#include "CaloDigitAlg.h"

#include <boost/assign.hpp>
using namespace boost::assign;

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
  : GaudiHistoAlg ( name , pSvcLocator            )
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
  
  m_advancedNoise[0].clear();
  m_advancedNoise[1].clear();
  m_advancedNoise[2].clear();
  m_noiseHist[0] = NULL;
  m_noiseHist[1] = NULL;
  m_noiseHist[2] = NULL;

  std::string begName = name.substr( 0, 8 );

  //== Get the previous MC Calo Digit contained according to rootInTES and context
  std::string prevRoot = "none";
  if ( "Prev1/" == rootInTES() ) {
    prevRoot = "Prev2/";
  } else if ( "" == rootInTES() ) {
    if ( "TAE" == context() ) {
      prevRoot = "Prev1/";
    } else {
      prevRoot = "Prev/";
    }
  } else if ( "Next1/" == rootInTES() ) {
    prevRoot = "";
  } else if ( "Next2/" == rootInTES() ) {
    prevRoot = "Next1/";
  }

  if ( "SpdDigit" == begName ) {
    m_detectorName     = DeCalorimeterLocation::Spd;
    m_inputData        = LHCb::MCCaloDigitLocation::Spd;
    m_outputData       = LHCb::CaloAdcLocation::Spd;
    if ( "none" != prevRoot ) m_inputPrevData = prevRoot + LHCb::MCCaloDigitLocation::Spd;
    m_pePerMeV         = 10.;
    m_coherentNoise    = 0.0;
    m_incoherentNoise  = 0.0;
    m_gainError        = 0.0;
    m_triggerName      = LHCb::L0PrsSpdHitLocation::Spd;
    m_triggerThreshold = 0.1 * Gaudi::Units::MeV;
    m_triggerIsBit     = true;
    m_zSupThreshold    = 10;  //== No ADC for SPD, only trigger bit...
  } else if ( "PrsDigit" == begName ) {
    m_detectorName     = DeCalorimeterLocation::Prs;
    m_inputData        = LHCb::MCCaloDigitLocation::Prs;
    m_outputData       = LHCb::CaloAdcLocation::Prs;
    if ( "none" != prevRoot ) m_inputPrevData = prevRoot + LHCb::MCCaloDigitLocation::Prs;
    m_pePerMeV         = 10.;
    m_coherentNoise    = 0.0;
    m_incoherentNoise  = 1.0;
    m_triggerName      = LHCb::L0PrsSpdHitLocation::Prs;
    m_triggerThreshold = 10. * Gaudi::Units::MeV;
    m_triggerIsBit     = true;
    m_zSupThreshold    = 15;
  } else if ( "EcalDigi" == begName ) {
    m_detectorName     = DeCalorimeterLocation::Ecal;
    m_inputData        = LHCb::MCCaloDigitLocation::Ecal;
    m_outputData       = LHCb::CaloAdcLocation::FullEcal;
    m_pedShift         = 0.40;
    m_triggerName      = LHCb::L0CaloAdcLocation::Ecal;
    m_triggerIsBit     = false;
 
    m_corrArea.push_back( 1.00 );
    m_corrArea.push_back( 1.04 );
    m_corrArea.push_back( 1.08 );
    
    m_monitorNoise = true;
    m_useAdvancedNoise = true;

    m_advancedNoise[0] +=
      list_of( 0.618503 )(   0.2671 )( 1.30606 ),
      list_of( 0.288860 )(   0.8269 )( 2.55909 ),
      list_of( 0.081979 )(   3.5597 )( 5.67153 ),
      list_of( 0.005044 )(  20.3323 )( 5.70424 ),
      list_of( 0.004536 )( -11.3676 )( 5.51563 ),
      list_of( 0.001078 )( -28.2179 )( 5.42412 );

    m_advancedNoise[1] +=
      list_of( 0.579320 )(   0.2466 )( 1.31034 ),
      list_of( 0.304418 )(   0.8750 )( 2.54487 ),
      list_of( 0.103213 )(   2.9349 )( 5.80636 ),
      list_of( 0.006098 )(  21.9765 )( 7.15202 ),
      list_of( 0.005976 )(  -9.1017 )( 6.18245 ),
      list_of( 0.000975 )( -26.2710 )( 5.48446 );

    m_advancedNoise[2] +=
      list_of( 0.557991 )(   0.2394 )( 1.36762 ),
      list_of( 0.312109 )(   0.9972 )( 2.85723 ),
      list_of( 0.120438 )(   2.5708 )( 7.18906 ),
      list_of( 0.007780 )(  27.4646 )( 11.3398 ),
      list_of( 0.001681 )( -37.0023 )( 13.6182 );

 } else if ( "HcalDigi" == begName ) {
    m_detectorName     = DeCalorimeterLocation::Hcal;
    m_inputData        = LHCb::MCCaloDigitLocation::Hcal;
    m_outputData       = LHCb::CaloAdcLocation::FullHcal;
    m_pedShift         = 0.40;
    m_triggerName      = LHCb::L0CaloAdcLocation::Hcal;
    m_triggerIsBit     = false;

    m_corrArea.push_back( 1.00 );
    m_corrArea.push_back( 1.05 );
  }

  declareProperty("MonitorNoise", m_monitorNoise,
    "Switch on/off noise monitoring");

  declareProperty("UseAdvancedNoise", m_useAdvancedNoise,
    "Switch on/off advanced noise");

  declareProperty("AdvancedNoiseOuter", m_advancedNoise[0],
    "Advanced noise for outer area.");
  declareProperty("AdvancedNoiseMiddle", m_advancedNoise[1],
    "Advanced noise for middle area.");
  declareProperty("AdvancedNoiseInner", m_advancedNoise[2],
    "Advanced noise for inner area.");

};

//=============================================================================
// Standard destructor
//=============================================================================
CaloDigitAlg::~CaloDigitAlg() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CaloDigitAlg::initialize() {

  StatusCode sc = GaudiHistoAlg::initialize();
  if( sc.isFailure() ) return sc;

  // Retrieve the calorimeter we are working with.
  m_calo = getDet<DeCalorimeter>( m_detectorName );

  //*** Initialize the random number service

  m_rndmSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true );

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

  // define the names of the monitoring histograms
  std::vector<std::string> areas; areas.push_back("outer"); areas.push_back("middle"); areas.push_back("inner");
  
  // configure advanced noise parameters
  if(m_useAdvancedNoise)
  {
    // check number of parameters
    for(int area = 0; area < 3; area++)
    {
      if(m_advancedNoise[area].empty())
        return Error("Advanced noise is configured incorectly (see options file).", StatusCode::FAILURE);
      for(int i = 0, size = m_advancedNoise[area].size(); i < size; i++)
      {
        if(m_advancedNoise[area][i].size() != 3)
          return Error("Advanced noise is configured incorectly (see options file).", StatusCode::FAILURE);
      }
    }

    const double nsigma = 6.0;

    // define monitoring histograms limits and calculate integrals
    for(int area = 0; area < 3; area++)
    {
      const std::vector< std::vector<double> >& advancedNoise = m_advancedNoise[area];

      int low = 0xFFFF; // low limit of the monitoring histogram
      int high = -0xFFFF; // high limit of the monitoring histogram

      debug() << "advanced noise parameters for " << areas[area] << " area:" << endreq;

      double& noiseIntegral = m_noiseIntegral[area];
      noiseIntegral = 0;
      
      for(int i = 0, size = advancedNoise.size(); i < size; i++)
      {
        debug() << "gaus " << i + 1 << ": " <<
          "[ integral = " << advancedNoise[i][GAUS_INTEGRAL] <<
          ", mean = " << advancedNoise[i][GAUS_MEAN] <<
          ", sigma = " << advancedNoise[i][GAUS_SIGMA] << " ]" << endreq;

        // check value of the sigma
        if(advancedNoise[i][GAUS_SIGMA] <= 0)
          return Error("Sigma is negative.", StatusCode::FAILURE);

        // calculate integral
        noiseIntegral += advancedNoise[i][GAUS_INTEGRAL];

        // calculate histograms limits
        int low_ = (int)(advancedNoise[i][GAUS_MEAN] - nsigma * advancedNoise[i][GAUS_SIGMA]);
        low = low_ < low ? low_ : low;
        int high_ = (int)(advancedNoise[i][GAUS_MEAN] + nsigma * advancedNoise[i][GAUS_SIGMA]);
        high = high_ > high ? high_ : high;
      }

      // noise integral have to be greater then zero
      if(noiseIntegral == 0)
        return Error("Noise integral is equal to zero (see options file).", StatusCode::FAILURE);

      // create monitoring histogram
      if(m_monitorNoise) m_noiseHist[area] = book(areas[area].c_str(), low, high, high - low);
    }
  }
  else if(m_monitorNoise)
  {
    // create monitoring histograms
    for(int area = 0; area < 3; area++)
    {
      m_noiseHist[area] = book1D(areas[area].c_str(), -10, 10, 200);
    }
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitAlg::execute() {

  bool isDebug = (msgLevel() <= MSG::DEBUG );

  // random numbers for advanced noise generation
  Rndm::Numbers noiseFlatRndm(rndmSvc(), Rndm::Flat(0.0, 1.0));
  Rndm::Numbers noiseGausRndm(rndmSvc(), Rndm::Gauss(0.0, 1.0));

  //*** get the input data
  
  LHCb::MCCaloDigits* mcDigits = get<LHCb::MCCaloDigits>( m_inputData );
  
  //=== Get the previous BX's data if needed
  LHCb::MCCaloDigits* prevDigits = 0;
  if ( "" != m_inputPrevData ) {
    if ( exist<LHCb::MCCaloDigits>( m_inputPrevData, IgnoreRootInTES ) ) 
      prevDigits = get<LHCb::MCCaloDigits>( m_inputPrevData, IgnoreRootInTES );
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
  if( false == m_useAdvancedNoise && 0 < m_incoherentNoise )
  {
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
    double adcValue;

    // if use advanced noise then generate noise value accordint to description of the noise spectra
    if(m_useAdvancedNoise)
    {
      // generate advanced noise

      double noise = 0;

      // select noise spectrum corresponding to the cell area
      const std::vector< std::vector<double> >& advancedNoise = m_advancedNoise[id.area()];
      
      // generate random flat value in the range ( 0 : noise spectrum integral )
      // this value is used to select Gaussian which will be generated (the Gaussian is selected according to its component fraction)
      double selector = m_noiseIntegral[id.area()] * noiseFlatRndm();

      // find Gaussian to generate
      for(int i = 0, size = advancedNoise.size(); i < size; i++)
      {
        // substract from the selector value current Gaussian integral
        selector -= advancedNoise[i][GAUS_INTEGRAL];
        
        // if it became less then zero or it is the last Gaussian (this must not happen) then we generate it
        if(selector < 0 || i == size - 1)
        {
          noise = advancedNoise[i][GAUS_MEAN] +
            advancedNoise[i][GAUS_SIGMA] * noiseGausRndm();
          break;
        }
      }

      adcValue = gainErrors[index] * energy / gain + noise;

      // fill monitoring histograms
      if(m_monitorNoise && m_noiseHist[id.area()] != NULL)
        fill(m_noiseHist[id.area()], noise, 1.0);
    }
    else
    {
      // generate standard noise
      
      adcValue = gainErrors[index] * energy / gain + incoherentNoise[index] + offset;

      // fill monitoring histograms
      if(m_monitorNoise && m_noiseHist[id.area()] != NULL)
        fill(m_noiseHist[id.area()], incoherentNoise[index] + offset, 1.0);
    }

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
          debug() << id << format( "energy%8.1f adc%6d et%8.1f trigAdc%4d",
                                   energy, intAdc, et, trigVal ) 
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
//  Finalize
//=============================================================================
StatusCode CaloDigitAlg::finalize()
{
  // if monitor advanced noise then fit histogram and check chi2
  if(m_useAdvancedNoise && m_monitorNoise)
  {
    std::vector<std::string> areas; areas.push_back("outer"); areas.push_back("middle"); areas.push_back("inner");
    for(int area = 0; area < 3; area++)
    {
      AIDA::IHistogram1D* aidaHist = m_noiseHist[area];
      if(0 == aidaHist) continue;

      const double noiseIntegral = m_noiseIntegral[area];
      const std::vector< std::vector<double> >& advancedNoise = m_advancedNoise[area];

      // get root histogram
      TH1D* rootHist = Gaudi::Utils::Aida2ROOT::aida2root(aidaHist);

      double* pars = new double[3 * advancedNoise.size()];
      if(pars == 0)
      {
        continue;
      }

      // form function
      std::ostringstream stream;
      for(int i = 0, size = advancedNoise.size(); i < size; i++)
      {
        pars[3 * i + 0] = (rootHist->GetEntries() / noiseIntegral) * advancedNoise[i][GAUS_INTEGRAL] / (advancedNoise[i][GAUS_SIGMA] * sqrt(2 * 3.1415927));
        pars[3 * i + 1] = advancedNoise[i][GAUS_MEAN];
        pars[3 * i + 2] = advancedNoise[i][GAUS_SIGMA];
        if(i != 0) stream << " + ";
        stream << "gaus(" << i * 3 << ")";
      }

      // create root function
      TF1 func("noiseFunc", stream.str().c_str(), rootHist->GetXaxis()->GetXmin(), rootHist->GetXaxis()->GetXmax());
      func.SetParameters(pars);

      // fix all parameter of the function except component fractions (integrals)
      for(int i = 0, size = 3 * advancedNoise.size(); i < size; i++)
      {
        if(i % 3 != 0) func.FixParameter(i, pars[i]);
      }

      // fit noise spectra
      rootHist->Fit("noiseFunc", "INQ");

      // print obtained component fractions and chisquare
      info() << "noise fit (" << areas[area] << " area):" << endreq;
      info() << "  chisquare = " << func.GetChisquare() << "/" << func.GetNDF() << endreq;
      for(int i = 0, size = 3 * advancedNoise.size(); i < size; i++)
      {
        if(i % 3 == 0) info() << "  amplitude for gaus #" << 1 + i / 3 << " = " << func.GetParameter(i) << " +- " << func.GetParError(i) << " ( " << pars[i] << " )" << endreq;
      }

      delete[] pars;
    }
  }
  return GaudiHistoAlg::finalize();
}

//=============================================================================

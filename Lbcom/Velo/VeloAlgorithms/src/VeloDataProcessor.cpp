//-----------------------------------------------------------------------------
// Implementation file for class : VeloDataProcessor
//
// 14/01/2002 : Chris Parkes
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"

// local
#include "VeloDataProcessor.h"
#include "VeloSim.h"

// Velo
#include "Event/MCVeloFE.h"
#include "Event/MCVeloFE.h"
#include "Event/VeloFullDigit.h"
#include "VeloKernel/VeloDigiParams.h"
#include "VeloKernel/VeloSimParams.h"
#include "VeloAlgorithms/VeloEventFunctor.h"

// Declaration of the Algorithm Factory
static const AlgFactory<VeloDataProcessor>          Factory ;
const        IAlgFactory& VeloDataProcessorFactory = Factory ; 

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
VeloDataProcessor::VeloDataProcessor( const std::string& name,
                                      ISvcLocator* pSvcLocator)   
  : Algorithm ( name , pSvcLocator )
  , m_inputContainer       ( MCVeloFELocation::Default )
  , m_outputContainer      ( VeloFullDigitLocation::Default )
  , m_noiseConstant        (VeloSimParams::noiseConstant)
  {
  declareProperty( "InputContainer"      ,m_inputContainer  );
  declareProperty( "OutputContainer"     ,m_outputContainer );
  declareProperty( "NoiseConstant"       ,m_noiseConstant);
}

//=============================================================================
// Destructor
//=============================================================================
VeloDataProcessor::~VeloDataProcessor() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode VeloDataProcessor::initialize() {
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Initialise" << endreq;

   return StatusCode::SUCCESS;
}


StatusCode VeloDataProcessor::execute() {
    // take an MCFE make a FullDigit
 
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  //*** get the input data

  SmartDataPtr<MCVeloFEs>  MCFEs( eventSvc() , m_inputContainer );
  if( 0 == MCFEs ) {
    log << MSG::ERROR
	<< "Unable to retrieve input data container="
	<< m_inputContainer << endreq;
    return StatusCode::FAILURE;
  }

  // make digits 

   VeloFullDigits* mydigitvector=new VeloFullDigits();
   
   log <<  MSG::DEBUG << "Retrieved " << MCFEs->size() << " MCVeloFEs" << endreq;
   int icount=0;
   for ( MCVeloFEs::const_iterator MCFEIt = MCFEs->begin() ; 
        MCFEs->end() != MCFEIt ; MCFEIt++ ) {
     icount++;

    // take an MCFE make a VeloFullDigit
     VeloChannelID myKey((*MCFEIt)->key());
     VeloFullDigit* mydigit = new VeloFullDigit(myKey);
     int ADC=int(digitise(float((*MCFEIt)->charge()))); 
     mydigit->setRawADCValue(ADC);

     // following will come from data processing 
     // but currently no pedestals/CM noise applied
     mydigit->setADCValue(ADC);
     mydigit->setSubtractedPedestal(0.);
     mydigit->setSubtractedCM(0.);
     // noise - use sigma of generator distribution
     float noise=digitise(float(noiseSigma()));
     mydigit->setRawNoise(noise);
     mydigit->setNoise(noise);
     mydigitvector->insert(mydigit);  

   }

// sort VeloFullDigits into order of ascending sensor + strip
  std::stable_sort(mydigitvector->begin(),mydigitvector->end(),VeloEventFunctor::Less_by_key<const VeloFullDigit*>());

  StatusCode sc = eventSvc()->registerObject(m_outputContainer,mydigitvector);

  if ( sc ) {
   log << MSG::DEBUG << "Stored " << mydigitvector->size() << " VeloFullDigits at " 
       << m_outputContainer << endreq;
  }
  else{
    log << MSG::ERROR << "Unable to store VeloFullDigits at " 
        << m_outputContainer << endreq;
   }

   return sc;
}

StatusCode VeloDataProcessor::finalize() {

    return StatusCode::SUCCESS;
}

float VeloDataProcessor::digitise(float electrons) {
  // convert electrons to ADC counts
  float digi = float(electrons*(VeloDigiParams::ADCFullScale/VeloDigiParams::electronsFullScale));
  if (digi>(VeloDigiParams::ADCFullScale-1)) digi=float(VeloDigiParams::ADCFullScale-1.);

  return digi;
}

//=========================================================================
// sigma of noise to generate
//=========================================================================
double VeloDataProcessor::noiseSigma(){
  double stripCapacitance=VeloSimParams::averageStripCapacitance;
  double noiseSigma=stripCapacitance*VeloSimParams::noiseCapacitance+
    m_noiseConstant;
  return noiseSigma;
}

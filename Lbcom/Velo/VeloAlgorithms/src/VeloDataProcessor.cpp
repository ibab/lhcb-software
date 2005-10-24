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
//#include "VeloSim.h"

// Velo
#include "Event/MCVeloFE.h"
#include "Event/MCVeloFE.h"
#include "Event/VeloFullFPGADigit.h"
#include "Event/VeloDigit.h"
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
  , m_inputContainer           ( MCVeloFELocation::Default )
  , m_outputVeloFPGADigit      ( VeloFullFPGADigitLocation::Default )
  , m_outputVeloDigit          ( VeloDigitLocation::Default )
  , m_noiseConstant            (VeloSimParams::noiseConstant)
  {
  declareProperty( "InputContainer"          ,m_inputContainer  );
  declareProperty( "OutputVeloFPGADigit"     ,m_outputVeloFPGADigit );
  declareProperty( "OutputVeloDigit"         ,m_outputVeloDigit );
  declareProperty( "NoiseConstant"           ,m_noiseConstant);
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

   VeloFullFPGADigits* fpgaDigitVec=new VeloFullFPGADigits();
   VeloDigits* veloDigitVec=new VeloDigits();
   
   log <<  MSG::DEBUG << "Retrieved " << MCFEs->size() << " MCVeloFEs" << endreq;
   int icount=0;
   for ( MCVeloFEs::const_iterator MCFEIt = MCFEs->begin() ; 
        MCFEs->end() != MCFEIt ; MCFEIt++ ) {
     icount++;

    // take an MCFE make a VeloFullFPGADigit and VeloDigit
     VeloChannelID myKey((*MCFEIt)->key());
     VeloFullFPGADigit* fpgaDigit = new VeloFullFPGADigit(myKey);
     VeloDigit* veloDigit=new VeloDigit(myKey);
     int ADC=int(digitise(float((*MCFEIt)->charge()))); 
     fpgaDigit->setRawADCValue(ADC);
     veloDigit->setADCValue(ADC);
     float noise=digitise(float(noiseSigma()));
     fpgaDigit->setRawNoise(noise);
     fpgaDigit->setNoise(noise);
     fpgaDigitVec->insert(fpgaDigit);
     veloDigitVec->insert(veloDigit);

   }

// sort VeloFullFPGADigits into order of ascending sensor + strip
  std::stable_sort(fpgaDigitVec->begin(),fpgaDigitVec->end(),
                   VeloEventFunctor::Less_by_key<const VeloFullFPGADigit*>());
  std::stable_sort(veloDigitVec->begin(), veloDigitVec->end(),
                   VeloEventFunctor::Less_by_key<const VeloDigit*>());

  StatusCode scA = eventSvc()->registerObject(m_outputVeloFPGADigit, fpgaDigitVec);
  StatusCode scB = eventSvc()->registerObject(m_outputVeloDigit, veloDigitVec);

  if ( scA&&scB ) {
   log << MSG::DEBUG << " ==> Stored " << fpgaDigitVec->size() 
       << " VeloFullFPGADigits at " << m_outputVeloFPGADigit
       << "/n and VeloDigits at " << m_outputVeloDigit << endreq;
  }
  else{
    log << MSG::ERROR << " ==> Unable to store Digits vectors!!" << endreq;
   }

   return (scA&&scB);
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

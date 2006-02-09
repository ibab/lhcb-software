//-----------------------------------------------------------------------------
// Implementation file for class : VeloDataProcessor
//
// 14/01/2002 : Chris Parkes, update Tomasz Szumlak
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "Kernel/VeloChannelID.h"

// local
#include "VeloDataProcessor.h"

// Velo
#include "Event/MCVeloFE.h"
#include "Event/VeloDigit.h"
#include "VeloAlgorithms/VeloEventFunctor.h"

// Declaration of the Algorithm Factory
static const AlgFactory<VeloDataProcessor>          Factory ;
const        IAlgFactory& VeloDataProcessorFactory = Factory ; 

//=============================================================================
// Standard creator, initializes variables
//=============================================================================
VeloDataProcessor::VeloDataProcessor( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_inputContainer ( LHCb::MCVeloFELocation::Default ),
    m_outputVeloDigit ( LHCb::VeloDigitLocation::Default ),
    m_noiseConstant ( 500. ),
    m_noiseCapacitance ( 50. ),
    m_averageStripCapacitance ( 20. ),
    m_ADCFullScale ( 256. ),
    m_electronsFullScale ( 200000. )
  {
  declareProperty("InputContainer", m_inputContainer);
  declareProperty("OutputVeloDigit", m_outputVeloDigit );
  declareProperty("NoiseConstant", m_noiseConstant);
  declareProperty("NoiseCapacitance", m_noiseCapacitance);
  declareProperty("AverageStripCapacitance", m_averageStripCapacitance);
  declareProperty("ADCFullScale", m_ADCFullScale);
  declareProperty("ElectronsFullScale", m_electronsFullScale);
}

//=============================================================================
// Destructor
//=============================================================================
VeloDataProcessor::~VeloDataProcessor() {};
//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode VeloDataProcessor::initialize() {
  //
  debug()<< " ==> VeloDataProcessor::initialize() " <<endmsg;
  //
  return (StatusCode::SUCCESS);
}
//==============================================================================
// take an MCFE make a FullDigit
//==============================================================================
StatusCode VeloDataProcessor::execute(){ 
  //
  debug()<< " ==> VeloDataProcessor::execute() " <<endmsg;
  // get the input data
  SmartDataPtr<LHCb::MCVeloFEs>  MCFEs(eventSvc(), m_inputContainer);
  if(0 == MCFEs) {
    error() << " ==> Unable to retrieve input data container="
	          << m_inputContainer <<endmsg;
    return (StatusCode::FAILURE);
  }
  // make digits
  LHCb::VeloDigits* veloDigitVec=new LHCb::VeloDigits(); 
   debug()<< "Retrieved " << MCFEs->size() << " MCVeloFEs" <<endmsg;
   int icount=0;
   for(LHCb::MCVeloFEs::const_iterator MCFEIt = MCFEs->begin() ; 
        MCFEs->end() != MCFEIt ; MCFEIt++){
     icount++;
    // take an MCFE make a VeloDigit
     LHCb::VeloChannelID myKey((*MCFEIt)->key());
     LHCb::VeloDigit* veloDigit=new LHCb::VeloDigit(myKey);
     int ADC=int(digitise(float((*MCFEIt)->charge())));
     veloDigit->setADCValue(ADC);
     double noise=digitise(double(noiseSigma()));
     veloDigit->setRawNoise(noise);
     veloDigitVec->insert(veloDigit);
   }
   //
   std::stable_sort(veloDigitVec->begin(), veloDigitVec->end(),
                   VeloEventFunctor::Less_by_key<const LHCb::VeloDigit*>());
   // StatusCode sc=eventSvc()->registerObject(m_outputVeloDigit, veloDigitVec);
   StatusCode sc=put(veloDigitVec, m_outputVeloDigit);
   //
   if(sc){
    debug()<< " ==> Stored VeloDigits at " << m_outputVeloDigit <<endmsg;
   }else{
     debug()<< " ==> Unable to store Digits vectors!!" << endreq;
   }
  //
  return (sc);
}
//==============================================================================
StatusCode VeloDataProcessor::finalize() {
  //
  debug()<< " ==> VeloDataProcessor::finazlize() " <<endmsg;
  //
  return (StatusCode::SUCCESS);
}
//==============================================================================
// convert electrons to ADC counts
//==============================================================================
float VeloDataProcessor::digitise(float electrons) {
  //
  debug()<< " ==> VeloDataProcessor::digitise() " <<endmsg;
  //
  float digi = float(electrons*(m_ADCFullScale/m_electronsFullScale));
  if(digi>(m_ADCFullScale-1)) digi=float(m_ADCFullScale-1.);
  //
  return digi;
}
//==============================================================================
// sigma of noise to generate
//==============================================================================
double VeloDataProcessor::noiseSigma(){
  //
  debug()<< " ==> VeloDataProcessor::noiseSigma() " <<endmsg;
  //
  double stripCapacitance=m_averageStripCapacitance;
  double noiseSigma=stripCapacitance*m_noiseCapacitance+m_noiseConstant;
  //
  return noiseSigma;
}

//-------------------------------------------------------------------------
// Implementation file for class : VeloDataProcessor
//
// 14/01/2002 : Chris Parkes, update Tomasz Szumlak
//-------------------------------------------------------------------------

// from Gaudi
#include "Kernel/VeloChannelID.h"
#include "Kernel/VeloEventFunctor.h"
#include "LHCbMath/LHCbMath.h"

// local
#include "VeloDataProcessor.h"

// Velo
#include "Event/MCVeloFE.h"
#include "Event/VeloDigit.h"

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloDataProcessor )

//=========================================================================
// Standard creator, initializes variables
//=========================================================================
VeloDataProcessor::VeloDataProcessor( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_inputContainer ( LHCb::MCVeloFELocation::Default ),
    m_outputVeloDigit ( LHCb::VeloDigitLocation::Default ),
    m_tanh0 ( 0. )
  {
  declareProperty("InputContainer", m_inputContainer);
  declareProperty("NonLinear", m_nonLinear = true );
  declareProperty("NLScale", m_NL_Scale = 297. );  // in ADC counts
  declareProperty("NLSlope", m_NL_Slope = 0.00226 ); // in ADC/e 
  declareProperty("NLCenter", m_NL_Center = 19000. ); // in e
  declareProperty("NLPTerm", m_NL_P_Term = 0.3032 ); // no units
  declareProperty("OutputVeloDigit", m_outputVeloDigit );
  declareProperty("ADCFullScale", m_ADCFullScale = 256. );
  declareProperty("ElectronsFullScale", m_electronsFullScale = 113216.0 );
  declareProperty("MaxADCOutput", m_maxADCOutput = 127 ); 
  declareProperty("MinADCOutput", m_minADCOutput = -127);
}

//=========================================================================
// Destructor
//=========================================================================
VeloDataProcessor::~VeloDataProcessor() {}

// Initialize
StatusCode VeloDataProcessor::initialize(){
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  m_tanh0 = tanhFunc(0); // set value for speed later

  return StatusCode::SUCCESS;
}

//=========================================================================
// take an MCFE make a FullDigit
//=========================================================================
StatusCode VeloDataProcessor::execute(){ 
  //
  debug()<< " ==> VeloDataProcessor::execute() " <<endmsg;
  // get the input data
  LHCb::MCVeloFEs* MCFEs = get<LHCb::MCVeloFEs>( m_inputContainer );

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
     int ADC;
     if( m_nonLinear){
       ADC = digitiseNonLinear((*MCFEIt)->charge());
     }else{
       ADC = digitiseLinear((*MCFEIt)->charge());
     }
     veloDigit->setADCValue(ADC);
     veloDigitVec->insert(veloDigit);
   }
   //
   std::stable_sort(veloDigitVec->begin(), veloDigitVec->end(),
                   VeloEventFunctor::Less_by_key<const LHCb::VeloDigit*>());
   // StatusCode sc=eventSvc()->registerObject(m_outputVeloDigit, veloDigitVec);
   put(veloDigitVec, m_outputVeloDigit);
   debug()<< " ==> Stored VeloDigits at " << m_outputVeloDigit <<endmsg;

   return StatusCode::SUCCESS;
}

//=========================================================================
// convert electrons to ADC counts using linear function
//=========================================================================
int VeloDataProcessor::digitiseLinear(double electrons) {
  //
  double digi = electrons*(m_ADCFullScale/m_electronsFullScale);
  int adc = static_cast<int>(LHCb::Math::round(digi));
  if( adc > m_maxADCOutput ) adc = m_maxADCOutput;
  if( adc < m_minADCOutput ) adc = m_minADCOutput;
  return adc;
}
//=========================================================================

//=========================================================================
// convert electrons to ADC counts using non linear function
//=========================================================================
int VeloDataProcessor::digitiseNonLinear(double electrons) {
  //
  double digi = m_NL_Scale * ( tanhFunc(electrons) - m_tanh0 );
  int adc = static_cast<int>(LHCb::Math::round(digi));
  if( adc > m_maxADCOutput ) adc = m_maxADCOutput;
  if( adc < m_minADCOutput ) adc = m_minADCOutput;
  return adc;
}

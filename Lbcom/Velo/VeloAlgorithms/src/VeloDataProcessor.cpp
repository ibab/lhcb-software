//-------------------------------------------------------------------------
// Implementation file for class : VeloDataProcessor
//
// 14/01/2002 : Chris Parkes, update Tomasz Szumlak
//-------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Kernel/VeloChannelID.h"
#include "Kernel/VeloEventFunctor.h"

// local
#include "VeloDataProcessor.h"

// Velo
#include "Event/MCVeloFE.h"
#include "Event/VeloDigit.h"

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloDataProcessor );

//=========================================================================
// Standard creator, initializes variables
//=========================================================================
VeloDataProcessor::VeloDataProcessor( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_inputContainer ( LHCb::MCVeloFELocation::Default ),
    m_outputVeloDigit ( LHCb::VeloDigitLocation::Default ),
    m_ADCFullScale ( 256. ),
    m_electronsFullScale ( 200000. )
  {
  declareProperty("InputContainer", m_inputContainer);
  declareProperty("OutputVeloDigit", m_outputVeloDigit );
  declareProperty("ADCFullScale", m_ADCFullScale);
  declareProperty("ElectronsFullScale", m_electronsFullScale);
}

//=========================================================================
// Destructor
//=========================================================================
VeloDataProcessor::~VeloDataProcessor() {};

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
     int ADC=int(digitise(float((*MCFEIt)->charge())));
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
// convert electrons to ADC counts
//=========================================================================
float VeloDataProcessor::digitise(float electrons) {
  //
  debug()<< " ==> VeloDataProcessor::digitise() " <<endmsg;
  //
  float digi = float(electrons*(m_ADCFullScale/m_electronsFullScale));
  if(digi>(m_ADCFullScale-1)) digi=float(m_ADCFullScale-1.);
  //
  return digi;
}
//=========================================================================

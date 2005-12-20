// $Id: STEffCalculator.cpp,v 1.2 2005-12-20 15:50:25 cattanem Exp $
//
// This File contains the definition of the OTEffCaculator -class
//
// C++ code for 'LHCb Tracking package(s)'
//
//   Author: M. Needham
//   Created: 19-09-2000

// Gaudi files
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolFactory.h"

//ITAlgorithms
#include "STEffCalculator.h"

//------------------------------------------------------------
// This tool is used to add ineffiency to IT
//------------------------------------------------------------

static ToolFactory<STEffCalculator> s_factory;
const IToolFactory& STEffCalculatorFactory = s_factory;

STEffCalculator::STEffCalculator(const std::string& type, 
                const std::string& name, const IInterface* parent): 
  GaudiTool( type, name, parent ),  
  m_GenEff(0)
{ 
  // constructor

  declareProperty("efficiency",m_eff=0.99); 
 
  // to get correct interface
  declareInterface<ISTEffCalculator>(this);
}

STEffCalculator::~STEffCalculator(){

 //destructer
}

StatusCode STEffCalculator::initialize(){

  // initialize
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

 /// initialize, flat generator...
 IRndmGenSvc* tRandNumSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
 sc = tRandNumSvc->generator(Rndm::Flat(0.,1.0),m_GenEff.pRef());
 if (sc.isFailure()){
   return Error( "failed to init generator ", sc);
 }
 release(tRandNumSvc);

 return StatusCode::SUCCESS;
}

bool STEffCalculator::accept(){
  // get a random number and compare to m_eff
  return (m_GenEff->shoot() < m_eff ? true: false );
}











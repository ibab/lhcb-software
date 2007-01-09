// $Id: STEffCalculator.cpp,v 1.4 2007-01-09 15:34:37 jvantilb Exp $

// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

// local
#include "STEffCalculator.h"

//------------------------------------------------------------
// This tool is used to add ineffiency to ST
//------------------------------------------------------------

DECLARE_TOOL_FACTORY( STEffCalculator );

STEffCalculator::STEffCalculator(const std::string& type, 
                                 const std::string& name, 
                                 const IInterface* parent): 
  GaudiTool( type, name, parent ),  
  m_GenEff(0)
{ 
  declareProperty("efficiency", m_eff = 0.99); 
 
  // to get correct interface
  declareInterface<ISTEffCalculator>(this);
}

STEffCalculator::~STEffCalculator()
{
  // destructer
}

StatusCode STEffCalculator::initialize()
{
  // initialize
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  /// initialize, flat generator...
  IRndmGenSvc* tRandNumSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = tRandNumSvc->generator( Rndm::Flat(0.,1.0), m_GenEff.pRef() );
  if (sc.isFailure()) return Error( "failed to init generator ", sc);
  release(tRandNumSvc);
  
  return sc;
}

bool STEffCalculator::accept()
{
  // get a random number and compare to m_eff
  return (m_GenEff->shoot() < m_eff ? true: false );
}

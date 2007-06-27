// $Id: OTRandomDepositCreator.cpp,v 1.16 2007-06-27 15:22:24 janos Exp $

// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"

// OTDAQ
#include "OTDAQ/IOTReadOutWindow.h"

// OTDet
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

// local
#include "OTRandomDepositCreator.h"

/** @file OTRandomDepositCreator.cpp 
 *
 *  Implementation of OT tool.
 *  
 *  @author M. Needham
 *  @date   28/02/2003
 */

using namespace LHCb;

// Declaration of the tool Factory
DECLARE_TOOL_FACTORY( OTRandomDepositCreator );

OTRandomDepositCreator::OTRandomDepositCreator(const std::string& type, 
                                               const std::string& name, 
                                               const IInterface* parent) : 
  GaudiTool( type, name, parent )
{
  declareProperty( "noiseRate", m_noiseRate = 10.0*Gaudi::Units::kilohertz );
  declareProperty( "readOutWindowToolName", m_readoutWindowToolName ="OTReadOutWindow" ),

  declareInterface<IOTRandomDepositCreator>(this);
}

OTRandomDepositCreator::~OTRandomDepositCreator()
{
  //destructor
}

StatusCode OTRandomDepositCreator::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize OTRandomDepositCreator", sc );

  // get interface to generator
  IRndmGenSvc* randSvc = svc<IRndmGenSvc>("RndmGenSvc", true);
  sc = randSvc->generator(Rndm::Flat(0.0,1.0),m_genDist.pRef()); 
  if ( sc.isFailure() ) {
    return Error("Failed to generate random number distribution",sc);
  }
  sc = release(randSvc);
  if (sc.isFailure()) {
    return Error("Failed to release RndmGenSvc", sc);
  }

  // Get OT geometry
  m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default ); 

  // Get channel deadtime
  m_deadTime = m_tracker->deadTime();

  // pointer to OTReadoutWindow tool
  IOTReadOutWindow* readoutTool = tool<IOTReadOutWindow>(m_readoutWindowToolName);
   // calculate window start and size
  std::vector<double> startGates = readoutTool->startReadOutGate();
  std::vector<double>::iterator iterG = startGates.begin();
  while (iterG !=  startGates.end()) {
    m_windowOffSet.push_back(*iterG - m_deadTime);
    ++iterG;
  } // iterG
  m_windowSize = readoutTool->sizeOfReadOutGate() + m_deadTime;
  // release tool 
  release(readoutTool);
  
  m_nMaxChanInModule = m_tracker->nMaxChanInModule();
  m_nNoise = nNoiseHits();

  return StatusCode::SUCCESS;  
}

void OTRandomDepositCreator::createDeposits(MCOTDepositVec* depVector) const 
{
  // get number of modules 
  std::vector<DeOTModule*> otModules =  m_tracker->modules();
  unsigned int nModules = otModules.size();
  
  for (unsigned(iDep) = 0; iDep < m_nNoise; ++iDep) {
    double randomNum = m_genDist->shoot();
    unsigned int moduleNum = unsigned((nModules*randomNum));
    DeOTModule* aModule = otModules[moduleNum];
    
    unsigned int strawID = unsigned(m_nMaxChanInModule*m_genDist->shoot())+1u;

    if (strawID <= aModule->nChannels()) {
      unsigned int stationID = aModule->elementID().station();
      double time = m_windowOffSet[stationID-1u] + (m_genDist->shoot() * m_windowSize);  
      OTChannelID aChan(stationID, aModule->elementID().layer(), aModule->elementID().quarter(),
			aModule->elementID().module(), strawID);
      MCOTDeposit* newDeposit = new MCOTDeposit(0, aChan, time, 0, 0);
      depVector->push_back(newDeposit);
    }
  } // iDep
}

unsigned int OTRandomDepositCreator::nNoiseHits() const
{ 
  // number of hits to generate
  return unsigned((nChannels()*m_windowSize*m_noiseRate));
}

unsigned int OTRandomDepositCreator::nChannels() const 
{
  /// return maximum number of channels
  return unsigned(m_nMaxChanInModule*(m_tracker->modules()).size());
}

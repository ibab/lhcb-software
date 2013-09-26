// Gaudi files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "LHCbMath/LHCbMath.h"

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
DECLARE_TOOL_FACTORY( OTRandomDepositCreator )

OTRandomDepositCreator::OTRandomDepositCreator(const std::string& type, 
                                               const std::string& name, 
                                               const IInterface* parent) : 
  GaudiTool( type, name, parent ),
  m_rgs(0),
  m_tracker(0),
  m_modules(),
  m_nModules(0u),
  m_nMaxChanInModule(0u),
  m_nChannels(0u),
  m_windowSize(0u),
  m_deadTime(0.0),
  m_nNoise(0u),
  m_windowOffSet()
{
  declareInterface<IOTRandomDepositCreator>(this);
  declareProperty( "NoiseRate"            , m_noiseRate             = 10.0*Gaudi::Units::kilohertz );
  declareProperty( "ReadOutWindowToolName", m_readoutWindowToolName = "OTReadOutWindow"            );
}

OTRandomDepositCreator::~OTRandomDepositCreator()
{
  //destructor
}

StatusCode OTRandomDepositCreator::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize OTRandomDepositCreator", sc );

  /// Random number from a flat distribution between 0 and 1
  /// for cross talk and double pulses
  sc = m_flat.initialize( randSvc() , Rndm::Flat( 0.0, 1.0) );
  if ( sc.isFailure() ) return Error( "Failed to initialize flat random number generator", sc );

  // Get OT geometry
  m_tracker   = getDet<DeOTDetector>(DeOTDetectorLocation::Default ); 
  m_modules   = m_tracker->modules();
  m_nModules  = m_modules.size();
  /// Get the possible maximum number of channels.
  /// Can not be more than num modules times 128
  m_nMaxChanInModule = m_tracker->nMaxChanInModule();
  m_nChannels        = m_nMaxChanInModule*m_nModules; //m_tracker->nChannels();

  /// Get channel deadtime
  /// This either needs to be an option or go into the conditions database
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

  /// Determine the number of noise hits to generate
  m_nNoise = unsigned( m_nChannels*m_windowSize*m_noiseRate );
  
  info() << "******** Noise Settings ********" << endmsg
         << "Number of channels: " << m_nChannels << endmsg 
         << "Noise rate: " << m_noiseRate << endmsg
         << "Width of read out gate in ns" << readoutTool->sizeOfReadOutGate() << endmsg
         << "Dead time : " << m_deadTime  << endmsg
         << "Number of deposits to generate: " << m_nNoise << endmsg
         << "********************************" << endmsg;

  return StatusCode::SUCCESS;  
}

IRndmGenSvc* OTRandomDepositCreator::randSvc() const {
  if( 0 == m_rgs ) m_rgs = svc<IRndmGenSvc>( "RndmGenSvc", true );
  return m_rgs;
}

void OTRandomDepositCreator::createDeposits(OTDeposits& deposits) const 
{
  for ( unsigned iDep = 0; iDep < m_nNoise; ++iDep ) {
    /// Get a random module
    /// Random number between 0 and 1
    const double modRanNum = m_flat();
    /// Start counting modules from 0 and not 1
    /// i.e. get a number between 0 and 431 and not between 1 and 432.
    /// Since we're randomly accessing the vector of modules
    unsigned int moduleNum = unsigned(m_nModules*modRanNum);
    DeOTModule*  module    = m_modules[moduleNum];
    
    /// Get a random straw in module
    /// Random number between 0 and 1
    const double strawRanNum = m_flat();
    /// Straw numbering starts from 1, but we want a random "straw number" between 0 and n.
    /// So the straw id is random straw number + 1u
    unsigned int strawID     = unsigned(m_nMaxChanInModule*strawRanNum) + 1u;
    
    if (strawID <= module->nChannels()) {
      const unsigned int stationID  = module->elementID().station();
      const double time             = m_windowOffSet[stationID-1u] + (m_flat() * m_windowSize);
      MCOTDeposit* newDeposit = new MCOTDeposit( MCOTDeposit::Noise,
                                                 0, 
                                                 OTChannelID( stationID, 
                                                              module->elementID().layer(), 
                                                              module->elementID().quarter(),
                                                              module->elementID().module(), 
                                                              strawID ), 
                                                 time, 
                                                 0, 
                                                 0 );
      deposits.push_back(newDeposit);
    }
  } // iDep
}

// $Id: STErrorMonitor.cpp,v 1.8 2009-11-09 17:55:51 mtobin Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from STTELL1Event
#include "Event/STTELL1BoardErrorBank.h"

// from STKernel
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/ISTReadoutTool.h"

// Boost
#include "boost/lexical_cast.hpp"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// local
#include "STErrorMonitor.h"

using namespace LHCb;
using namespace STDAQ;
using namespace STBoardMapping;

//-----------------------------------------------------------------------------
// Implementation file for class : STErrorMonitor
//
// 2009-03-20 : Jeroen Van Tilburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STErrorMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STErrorMonitor::STErrorMonitor( const std::string& name,
                                ISvcLocator* pSvcLocator) :
  ST::HistoAlgBase(name, pSvcLocator)
{
  declareSTConfigProperty("ErrorLocation", m_errorLocation, 
                          STTELL1BoardErrorBankLocation::TTErrors );
  declareProperty("ExpertHisto", m_expertHisto = true );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode STErrorMonitor::initialize() 
{  
  // Initialize ST::HistoAlgBase
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return sc;

  // Get the maximum number of Tell1s to determine number of histogram bin
  m_maxTell1s = (this->readoutTool())->SourceIDToTELLNumberMap().size();

  // Book histogram
  m_1d_errorBanks = book1D("Error banks per Tell1", 0.5, m_maxTell1s+0.5, m_maxTell1s);

  // Get the tell1 mapping from source ID to tell1 number
  std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();
  for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
    unsigned int tellID = (*itT).second;
    // Create a title for the histogram
    std::string strTellID  = boost::lexical_cast<std::string>(tellID);
    HistoID histoID        = "error-types_$tell" + strTellID;
    std::string histoTitle = "Error types tell" + strTellID;
    m_errorHistos[tellID] = book2D(histoID, histoTitle, 0, noptlinks, 0, 10, nports*noptlinks, 10);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STErrorMonitor::execute()
{
  // Get the error banks
  if ( !exist<STTELL1BoardErrorBanks>(m_errorLocation) ) {
    return Error("No STTELL1BoardErrorBanks in TES. Run the STErrorDecoding "
                 "first.", StatusCode::FAILURE);
  }
  STTELL1BoardErrorBanks* errors = get<STTELL1BoardErrorBanks>(m_errorLocation);

  // Loop over the error banks
  STTELL1BoardErrorBanks::const_iterator iterBank = errors->begin();
  for ( ; iterBank != errors->end();  ++iterBank){

    // Print out the error bank information
    if (msgLevel(MSG::DEBUG)) debug() << **iterBank << endmsg;

    // Get the number of the tell1
    unsigned int sourceID = (*iterBank)->Tell1ID();
    unsigned int tellNum = (this->readoutTool())->SourceIDToTELLNumber(sourceID);
    
    // Plot the number of error banks versus sequential tell number
    m_1d_errorBanks->fill(tellNum);

    if ( !m_expertHisto ) continue;

    // Convert the tell1 number to a string
    std::string strTellNum  = boost::lexical_cast<std::string>(tellNum);

    // Loop over the ErrorInfo objects (one for each FPGA-PP)
    const std::vector<STTELL1Error*> errorInfo = (*iterBank) -> errorInfo();
    std::vector<STTELL1Error*>::const_iterator errorIt = errorInfo.begin();    
    for( ; errorIt != errorInfo.end(); ++errorIt) {

      // Get the PP number [0:3]
      unsigned int pp = errorIt - errorInfo.begin();

      // Get the majority vote for this PP
      unsigned int pcn = (*errorIt)->pcnVote();

      // Loop over the Beetles and ports
      for (unsigned int beetle = 0u; beetle < nBeetlesPerPPx; ++beetle) {
        for (unsigned int port = 0u; port < nports; ++port) {
          // Fill 2D histogram with the error types versus Beetle port
          unsigned int errorType = (*errorIt) -> linkInfo(beetle, port, pcn);
          double portBin = double(port)/double(nports)+beetle+pp*nBeetlesPerPPx;
          m_errorHistos[tellNum]->fill(portBin, errorType);
        }
      }
    }
  }  //iterBanks

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode STErrorMonitor::finalize()
{
  return ST::HistoAlgBase::finalize();// must be called after all other actions
}
//=============================================================================

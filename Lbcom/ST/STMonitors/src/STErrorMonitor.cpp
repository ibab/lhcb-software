// Include files 

// from STTELL1Event
#include "Event/STTELL1BoardErrorBank.h"

// from STKernel
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"
#include "Kernel/ISTReadoutTool.h"

// AIDA
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
// ROOT
#include "GaudiUtils/Aida2ROOT.h"
#include "TH2D.h"
#include "TAxis.h"

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
DECLARE_ALGORITHM_FACTORY( STErrorMonitor )

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

  m_debug = msgLevel( MSG::DEBUG );
  m_verbose = msgLevel( MSG::VERBOSE );
  if(m_verbose) m_debug = true;

  // Get the maximum number of Tell1s to determine number of histogram bin
  m_maxTell1s = (this->readoutTool())->SourceIDToTELLNumberMap().size();

  // Book histograms
  m_1d_evtCounter = book1D("Event counter",0.5, 1.5, 1);
  m_1d_errorBanks = book1D("Error banks per Tell1", 0.5, m_maxTell1s+0.5, m_maxTell1s);
  m_1d_fracErrors = book1D("Fraction of ports which sent error banks",0.,1.0,100);
  m_prof_fracErrors = bookProfile1D("Fraction of ports which sent error banks per TELL1", 0.5, m_maxTell1s+0.5, m_maxTell1s);
  m_2d_errorTypes = book2D("ErrorTypes","Error types per TELL1", 0.5, m_maxTell1s+0.5, m_maxTell1s,0.,10., 10);
  labelHistoErrorTypes( m_2d_errorTypes );

  // Get the tell1 mapping from source ID to tell1 number
  std::map<unsigned int, unsigned int>::const_iterator itT = (this->readoutTool())->SourceIDToTELLNumberMap().begin();

  m_activePorts = 0;
  for(; itT != (this->readoutTool())->SourceIDToTELLNumberMap().end(); ++itT) {
    unsigned int tellID = (*itT).second;
    // Create a title for the histogram
    std::string strTellID  = std::to_string(tellID);
    HistoID histoID        = "error-types_$tell" + strTellID;
    std::string histoTitle = "Error types " + detType() + "TELL" + strTellID;
    m_errorHistos[tellID] = book2D(histoID, histoTitle, 0., noptlinks, nports*noptlinks, 0., 10., 10);
    labelHistoErrorTypes( m_errorHistos[tellID] );
    // Work out the total number of links for the detector (over-estimate for TT as some links not enabled)
    m_activePorts += nports*noptlinks;
  }

  if(detType() == "TT") m_activePorts -= 128;//< Very nastily hardcoded for now.

  if(detType() == "UT") m_activePorts -= 128;//< Very nastily hardcoded for now.

  if(m_debug) debug() << "Number of active ports in " << detType() << " is " << m_activePorts << endmsg;

  // Get the tell1 mapping from source ID to tell1 number
  m_nTELL1s = readoutTool()->nBoard();

  return StatusCode::SUCCESS;
}
//==============================================================================
// Set the labels for the error type histograms
//==============================================================================
void STErrorMonitor::labelHistoErrorTypes( AIDA::IHistogram2D* histo ) {

  TH2D *h = Gaudi::Utils::Aida2ROOT::aida2root( histo );
  if ( 0 != h ) {
    // get the axis:
    TAxis* axis = h->GetYaxis() ;     
    if( 0 != axis ) {       
      axis->SetBinLabel( 1, "None" );
      axis->SetBinLabel( 2, "CorruptedBank" );
      axis->SetBinLabel( 3, "OptLinkDisabled" );
      axis->SetBinLabel( 4, "TlkLinkLoss" );
      axis->SetBinLabel( 5, "OptLinkNoClock" );
      axis->SetBinLabel( 6, "SyncRAMFull" );
      axis->SetBinLabel( 7, "SyncEvtSize" );
      axis->SetBinLabel( 8, "OptLinkNoEvent" );
      axis->SetBinLabel( 9, "PseudoHeader" );
      axis->SetBinLabel(10, "WrongPCN" );
    }       
  }
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode STErrorMonitor::execute()
{

  m_1d_evtCounter->fill(1.);
  // Get the error banks
  const STTELL1BoardErrorBanks* errors = getIfExists<STTELL1BoardErrorBanks>(m_errorLocation);
  if ( NULL == errors ) {
    return Error("No STTELL1BoardErrorBanks in TES. Run the STErrorDecoding "
                 "first.", StatusCode::FAILURE);
  }

  // Number of links with error banks
  unsigned int nErrors = 0;
  m_nErrorBanksPerTELL1.resize(m_nTELL1s,0);

  // Loop over the error banks
  STTELL1BoardErrorBanks::const_iterator iterBank = errors->begin();
  for ( ; iterBank != errors->end();  ++iterBank){

    // Print out the error bank information
    if (m_debug) debug() << **iterBank << endmsg;

    // Get the number of the tell1
    unsigned int sourceID = (*iterBank)->Tell1ID();
    unsigned int tellNum = (this->readoutTool())->SourceIDToTELLNumber(sourceID);
    
    // Plot the number of error banks versus sequential tell number
    m_1d_errorBanks->fill(tellNum);

    if ( !m_expertHisto ) continue;

    // Convert the tell1 number to a string
    //std::string strTellNum  = std::to_string(tellNum);

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
          if(errorType > 0) {
            nErrors++;
            m_nErrorBanksPerTELL1[tellNum-1] += 1;
            m_2d_errorTypes->fill(tellNum,errorType);
          }
        }
      }
    }
  }  //iterBanks

  double fracErrors = (double)nErrors / m_activePorts;
  if(fracErrors > 1E-5) m_1d_fracErrors->fill(fracErrors);
  std::vector<unsigned int>::iterator it=m_nErrorBanksPerTELL1.begin();
  int iTell=1;
  for(; it != m_nErrorBanksPerTELL1.end(); ++it, ++iTell) {
    m_prof_fracErrors->fill(iTell, (*it)/(nports*noptlinks));
  }

  return StatusCode::SUCCESS;
}
//=============================================================================

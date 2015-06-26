
//-----------------------------------------------------------------------------
/** @file RichDataDecodingErrorMoni.cpp
 *
 *  Implementation file for algorithm class : Rich::DAQ::DataDecodingErrorMoni
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichDataDecodingErrorMoni.h"

// namespace
using namespace Rich::DAQ;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( DataDecodingErrorMoni )

// Standard constructor, initializes variables
  DataDecodingErrorMoni::DataDecodingErrorMoni( const std::string& name,
                                                ISvcLocator* pSvcLocator )
    : HistoAlgBase ( name, pSvcLocator ),
      m_decoder    ( NULL ),
      m_RichSys    ( NULL ),
      m_odin       ( NULL )
{ }

// Destructor
DataDecodingErrorMoni::~DataDecodingErrorMoni() { }

//  Initialize
StatusCode DataDecodingErrorMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDDecoder", m_decoder, NULL, true );

  // RichDet
  m_RichSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  return sc;
}

const Rich::HistoAlgBase::BinLabels & DataDecodingErrorMoni::labels()
{
  static const BinLabels labels =
    { "L1 Ingress Truncated", "ODIN/Ingress BXID MisMatch",
      "HPD Inhibit", "HPD DB Lookup", "Ingress/HPD EventID MisMatch",
      "Extended HPD Header" };
  return labels;
}

StatusCode DataDecodingErrorMoni::prebookHistograms()
{
  const auto   nlabels = labels().size();
  const auto & L1s     = m_RichSys->level1HardwareIDs();
  const auto   nL1s    = L1s.size();

  BinLabels l1HardIDLabels(nL1s);
  for ( const auto& l1 : L1s )
  {
    const Rich::DAQ::Level1CopyNumber copyN = m_RichSys->copyNumber(l1);
    l1HardIDLabels[copyN.data()] = (std::string)(l1);
  }

  richProfile1D( HID("decodingErrors"),
                 "DAQ Decoding Error Rates (%)",
                 0.5, nlabels + 0.5, nlabels,
                 "DAQ Decoding Error Types", "Error Rate (%)", labels() );

  richHisto2D( HID("decodingErrorsByBoard"),
               "DAQ Decoding Errors by UKL1 Hardware ID",
               -0.5, nL1s   - 0.5, nL1s,
               0.5, nlabels + 0.5, nlabels,
               "UKL1 Hardware ID", "",
               "# of errors", l1HardIDLabels, labels() );
  
  return StatusCode::SUCCESS;
}

// Main execution
StatusCode DataDecodingErrorMoni::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  // Load the ODIN for this event
  m_odin = get<LHCb::ODIN>( LHCb::ODINLocation::Default );

  // Obtain RichSmartIDs from raw decoding
  const DAQ::L1Map & data = m_decoder->allRichSmartIDs();

  // Loop over L1 boards
  for ( const auto& L1 : data )
  {
    // All boards combined
    sc = sc && makePlots( L1.second, -1 );
    // Each L1 board on its own
    sc = sc && makePlots( L1.second, (int)L1.first.data() );
  }

  // return
  return sc;
}

AIDA::IProfile1D *
DataDecodingErrorMoni::getHisto( const int l1ID )
{
  AIDA::IProfile1D * histo(NULL);
  if ( l1ID < 0 )
  {
    histo = richProfile1D( HID("decodingErrors") );
  }
  else
  {
    std::ostringstream id,title;
    id << "L1s/decodingErrors-L1hardID" << l1ID;
    title << "L1-HardwareID " << l1ID << " DAQ Decoding Error Rates (%)";
    histo = richProfile1D( HID(id.str()), title.str(), 
                           0.5, labels().size()+0.5, labels().size(),
                           "DAQ Decoding Error Types", "Error Rate (%)", labels() );
  }
  return histo;
}

StatusCode
DataDecodingErrorMoni::makePlots( const Rich::DAQ::IngressMap & inMap,
                                  const int l1ID )
{
  StatusCode sc = StatusCode::SUCCESS;

  // Get the Histos as needed
  AIDA::IProfile1D   * h1D = getHisto(l1ID);
  AIDA::IHistogram2D * h2D = ( l1ID == -1 ? NULL :
                               richHisto2D( HID("decodingErrorsByBoard") ) );

  // Get L1 Copy Number
  Rich::DAQ::Level1CopyNumber copyN;
  if ( l1ID != -1 ) 
  {
    const Rich::DAQ::Level1HardwareID hID( l1ID );
    copyN = m_RichSys->copyNumber(hID); 
  }

  // loop over ingresses for this L1 board
  for ( const auto& In : inMap )
  {
    const Rich::DAQ::IngressInfo & ingressInfo       = In.second;
    const Rich::DAQ::L1IngressHeader & ingressHeader = ingressInfo.ingressHeader();

    // Check if all HPDs are suppressed
    fillPlots( copyN, 1, ingressHeader.hpdsSuppressed(), h1D, h2D );

    // Check BX ID between Rich and ODIN
    const bool bxIDOK = BXID(m_odin->bunchId()) == ingressHeader.bxID();
    fillPlots( copyN, 2, !bxIDOK, h1D, h2D );
    
    // Loop over HPDs in this ingress
    for ( const auto & HPD : In.second.hpdData() )
    {
      const bool inhibit = HPD.second.header().inhibit();
      // inhibited HPDs
      fillPlots(copyN, 3, inhibit, h1D, h2D );
      if ( !inhibit )
      {
        // Invalid HPD (BD lookup error)
        fillPlots(copyN, 4, !HPD.second.hpdID().isValid(), h1D, h2D );
        // Event IDs
        const bool evtIDOK = ( ingressHeader.eventID() == 
                               HPD.second.header().eventID() );
        fillPlots( copyN, 5, !evtIDOK, h1D, h2D );
        // HPD header in extended mode
        fillPlots( copyN, 6, HPD.second.header().extendedFormat(), h1D, h2D );
      }
    } // loop over HPDs

  } // ingresses

  return sc;
}

void DataDecodingErrorMoni::fillPlots( const Rich::DAQ::Level1CopyNumber& copyN,
                                       const int errorCode,
                                       const bool error,
                                       AIDA::IProfile1D * h1D,
                                       AIDA::IHistogram2D * h2D )
{
  if ( h1D ) { h1D->fill( errorCode, error ? 100.0 : 0.0 ); }
  if ( h2D && error ) { h2D->fill( copyN.data(), errorCode ); }
}


//-----------------------------------------------------------------------------
/** @file RichHPDFlashMoni.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::MC::HPDFlashMoni
 *
 *  @author Antonis Papanestis   Antonis.Papanestis@cern.ch
 *  @date   11/10/2011
 */
//-----------------------------------------------------------------------------

// STD
#include <sstream>

// RICH
#include "RichRecBase/RichDetParams.h"

// boost
#include "boost/lexical_cast.hpp"

// local
#include "RichHPDFlashMoni.h"

// namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( HPDFlashMoni )

// Standard constructor, initializes variables
  HPDFlashMoni::HPDFlashMoni( const std::string& name,
                              ISvcLocator* pSvcLocator)
    : HistoAlgBase ( name, pSvcLocator ),
      m_decoder         ( NULL ),
      m_HpdOccupancyTool( NULL ),
      m_richSys         ( NULL ),
      m_events          (  0   ),
      m_dumpedEvents    (  0   )
{
  // properties
  declareProperty( "FixedUpperThreshold",    m_fixedUpperThreshold  = 400 );
  declareProperty( "FixedLowerThreshold",    m_fixedLowerThreshold  = 100 );
  declareProperty( "AbsoluteUpperThreshold", m_absThreshold    = 200 );
  declareProperty( "RelativeUpperThreshold", m_relThreshold    = 5.0 );
  declareProperty( "SecondThreshold",        m_secondThreshold = 1.0 );

  declareProperty( "OperationMode", m_operationMode = 1 );//0=absolute, 1=relative
  declareProperty( "SkipEvents",    m_skipEvents = 50 );

  declareProperty( "DumpEvents",      m_dumpEvents = false );
  declareProperty( "MaxDumpedEvents", m_maxDumpedEvents = 100 );
  declareProperty( "StopSequence"   , m_stopSequence = true );
}

// Destructor
HPDFlashMoni::~HPDFlashMoni() { }

//  Initialize
StatusCode HPDFlashMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDDecoder", m_decoder, NULL, true );
  acquireTool( "RichSmartIDTool",    m_idTool,  NULL, true );

  m_HpdOccupancyTool =
    tool<IHPDOccupancyTool>("Rich::HPDOccupancyTool","HPDOccupancy",this);

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  return sc;
}


//=========================================================================
//
//=========================================================================
StatusCode HPDFlashMoni::prebookHistograms()
{

  // percent histograms
  m_perCentHistos.push_back(richProfile1D( HID("largeEventsR1U"), "Rich1 Up Large Event Rates (%)",
                                           0.5, 20+0.5, 20,
                                           "No of HPDs", "Large Event Rate (%)" ) );

  m_perCentHistos.push_back(richProfile1D( HID("largeEventsR1B"), "Rich1 Down Large Event Rates (%)",
                                           0.5, 20+0.5, 20,
                                           "No of HPDs", "Large Event Rate (%)" ) );

  m_perCentHistos.push_back(richProfile1D( HID("largeEventsR2A"), "Rich2 A side Large Event Rates (%)",
                                           0.5, 20+0.5, 20,
                                           "No of HPDs", "Large Event Rate (%)" ) );

  m_perCentHistos.push_back(richProfile1D( HID("largeEventsR2C"), "Rich2 C side Large Event Rates (%)",
                                           0.5, 20+0.5, 20,
                                           "No of HPDs", "Large Event Rate (%)" ) );
  // hpd map histograms
  m_HpdPanelHistos.push_back(richHisto2D( HID("Rich1Top"), "Rich1 Up", -(13+0.5), 0.5, 14,
                                          -(6+0.5), 0.5, 7, "", "" ) );
  m_HpdPanelHistos.push_back(richHisto2D( HID("Rich1Bot"), "Rich1 Down", -0.5, 13+0.5, 14,
                                          -0.5, 6+0.5, 7, "", "" ) );
  m_HpdPanelHistos.push_back(richHisto2D( HID("Rich2A"), "Rich2 A side", -0.5, 8+0.5, 9,
                                          -(15+0.5), 0.5, 16, "", "" ) );
  m_HpdPanelHistos.push_back(richHisto2D( HID("Rich2C"), "Rich2 C Side", -0.5, 8+0.5, 9,
                                          -(15+0.5), 0.5, 16, "", "" ) );


  m_goodBadEvents = richHisto1D( HID("GoodBad"), "Good / Bad events", -0.5, 1.5, 2 );
  m_thresholdHisto = richProfile1D( HID("Thresholds"), "Threshold per HPD",
                                    -0.5, 483+0.5, 484,
                                    "HPD Copy no", "Threshold" );

  return StatusCode::SUCCESS;
}


//=========================================================================
// execute
//=========================================================================
// Main execution
StatusCode HPDFlashMoni::execute()
{
  // skip some events for running occupancy to stabilise
  m_events++;
  if ( m_events < m_skipEvents ) return StatusCode::SUCCESS;

  int goodEvent(0);
  std::vector<int> aboveThreshold1(4,0);
  std::vector<int> aboveThreshold2(4,0);
  std::vector<LHCb::RichSmartID::Vector> hpdIDs(4);

  // Obtain RichSmartIDs from raw decoding
  const DAQ::L1Map & data = m_decoder->allRichSmartIDs();

  // Loop over L1 boards
  for ( Rich::DAQ::L1Map::const_iterator iL1 = data.begin();
        iL1 != data.end(); ++iL1 )
  {
    // loop over ingresses for this L1 board
    for ( Rich::DAQ::IngressMap::const_iterator iIn = (*iL1).second.begin();
          iIn != (*iL1).second.end(); ++iIn )
    {
      // Loop over HPDs in this ingress
      for ( Rich::DAQ::HPDMap::const_iterator iHPD = (*iIn).second.hpdData().begin();
            iHPD != (*iIn).second.hpdData().end(); ++iHPD )
      {
        // Valid HPD ID
        if ( !(*iHPD).second.hpdID().isValid() ) { continue; }
        // inhibited HPD ?
        if ( (*iHPD).second.header().inhibit() ) { continue; }

        // HPD info
        const LHCb::RichSmartID hpd     = (*iHPD).second.hpdID();
        //const DAQ::HPDHardwareID hardID = m_richSys->hardwareID(hpd);
        //const DAQ::Level0ID l0ID        = m_richSys->level0ID(hpd);
        //const DAQ::HPDCopyNumber copyN  = m_richSys->copyNumber(hpd);

        // Vector of SmartIDs
        const LHCb::RichSmartID::Vector & rawIDs = (*iHPD).second.smartIDs();
        const int nHits = rawIDs.size();
        const int panelIndex = hpd.rich()*2+hpd.panel();

        int threshold1(0), threshold2(0);

        if ( m_operationMode == 0 )
        {
          threshold1 = m_absThreshold;
        }
        else
        {
          // use relative threshold, but within a lower-upper range.
          const double hpdThreshold = m_relThreshold*m_HpdOccupancyTool->averageOccupancy(hpd);
          threshold1 = ( hpdThreshold > m_fixedUpperThreshold ? m_fixedUpperThreshold : hpdThreshold );
          threshold1 = ( threshold1 < m_fixedLowerThreshold ? m_fixedLowerThreshold : threshold1 );
        }

        m_thresholdHisto->fill(m_richSys->copyNumber(hpd).data(), threshold1);
        threshold2 = m_secondThreshold*threshold1;

        if ( nHits >= threshold1 )
        {
          aboveThreshold1[panelIndex]++;
          //info() << "HPD:" << hpd << " Thresh:" << threshold1 << " nHits:" << nHits << endmsg;
        }

        if ( nHits >= threshold2 )
        {
          aboveThreshold2[panelIndex]++;
          hpdIDs[panelIndex].push_back( hpd );
        }

      } // loop over HPDs
    } // ingresses
  } // L1 boards


  // now loop over the four panels to find HPDs with high occupancy
  for ( unsigned int rich=0; rich<2; ++rich)
    for ( unsigned int side=0; side<2; ++side)
    {
      int panel = rich*2+side;
      std::vector<bool> alreadyDumped(2,false);

      std::vector<int> fillProfHisto(21,0);

      const int abThr2 = ( aboveThreshold2[panel] > 20 ? 20 : aboveThreshold2[panel] );
      if ( aboveThreshold1[panel] > 0 )
      {
        goodEvent = 1;
        if ( m_dumpEvents && !alreadyDumped[rich] && m_dumpedEvents < m_maxDumpedEvents )
        {
          dumpEvent(rich);
          m_dumpedEvents++;
        }
        alreadyDumped[rich] = true;

        fillProfHisto[abThr2] = 100;

        // loop over hpds for 2D histogram
        LHCb::RichSmartID::Vector::const_iterator hpd_it;
        for ( hpd_it = hpdIDs[panel].begin(); hpd_it != hpdIDs[panel].end(); ++hpd_it)
        {
          const int col = (*hpd_it).pdCol();
          const int numInCol = (*hpd_it).pdNumInCol();

          // gymnastics for filling the histograms
          if ( panel == 0 )  // RICH1 UP
            m_HpdPanelHistos[panel]->fill( -numInCol, -col, 1 );
          else
            if ( panel == 1 ) // RICH1 DOWN
              m_HpdPanelHistos[panel]->fill( numInCol, col, 1 );
            else
              // RICH2
              m_HpdPanelHistos[panel]->fill( col, -numInCol, 1 );

        }
      }
      // fill profile histogram with zeros except one bin
      for (unsigned int j=1; j<21; ++j)
        m_perCentHistos[panel]->fill(j, fillProfHisto[j]);

    }

  // keep track of good/bad events
  m_goodBadEvents->fill(goodEvent);
  if ( goodEvent == 0 && m_stopSequence )
    setFilterPassed(false);
  else
    setFilterPassed(true);

  return StatusCode::SUCCESS;
}


//=========================================================================
//   dumpEvent
//=========================================================================
StatusCode HPDFlashMoni::dumpEvent(int thisRich)
{
  PD_LOCAL_POSITIONS_X;
  PD_LOCAL_POSITIONS_Y;

  int m_nBins( 200 );

  const std::string title =
    ( std::string("Event Snapshot :") +
      std::string(" Event ") + boost::lexical_cast<std::string>(m_events) );

  // make the hit maps
  const Rich::DAQ::L1Map & l1Map = m_decoder->allRichSmartIDs();
  for ( Rich::DAQ::L1Map::const_iterator iL1Map = l1Map.begin();
        iL1Map != l1Map.end(); ++iL1Map ) {
    const Rich::DAQ::IngressMap & ingressMap = iL1Map->second;
    for ( Rich::DAQ::IngressMap::const_iterator iIngressMap = ingressMap.begin();
          iIngressMap != ingressMap.end(); ++iIngressMap ) {
      const Rich::DAQ::HPDMap &hpdMap = iIngressMap->second.hpdData();
      for ( Rich::DAQ::HPDMap::const_iterator iHPDMap = hpdMap.begin();
            iHPDMap != hpdMap.end(); ++iHPDMap ) {
        const LHCb::RichSmartID::Vector &hitSmartIDs = iHPDMap->second.smartIDs();

        for ( LHCb::RichSmartID::Vector::const_iterator iHit = hitSmartIDs.begin();
              iHit != hitSmartIDs.end(); ++iHit )  {

          const Rich::DetectorType rich = (*iHit).rich();
          if ( rich != thisRich ) continue;

          // pixel hit coord in global coordinates
          Gaudi::XYZPoint gPos;
          const StatusCode scc = m_idTool->globalPosition( *iHit, gPos );
          if ( scc.isSuccess() )  {
            // pos in local coords
            const Gaudi::XYZPoint lPos = m_idTool->globalToPDPanel(gPos);
            // RICH info
            const std::string & RICH  = Rich::text(rich);
            plot2D( lPos.x(), lPos.y(),
                    HID(boost::lexical_cast<std::string>(m_events),rich),
                    RICH+" "+title,
                    xMinPDLoc[rich],xMaxPDLoc[rich],yMinPDLoc[rich],yMaxPDLoc[rich],
                    m_nBins, m_nBins );
          } else {
            debug() <<  "Problem with SmartID -> global position conversion" << endmsg;
          } // if success
        } // for iHit
      } // for iHPDMap
    } // for iIngressMap
  } // for L1Map

  return StatusCode::SUCCESS;
}

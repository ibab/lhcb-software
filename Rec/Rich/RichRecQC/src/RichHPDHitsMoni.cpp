
//-----------------------------------------------------------------------------
/** @file RichHPDHitsMoni.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::MC::HPDHitsMoni
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

// STD
#include <sstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichHPDHitsMoni.h"

// namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( HPDHitsMoni )

// Standard constructor, initializes variables
  HPDHitsMoni::HPDHitsMoni( const std::string& name,
                            ISvcLocator* pSvcLocator)
    : HistoAlgBase ( name, pSvcLocator ),
      m_decoder    ( NULL ),
      m_richSys    ( NULL )
{
  declareProperty( "EnablePDNumHits", m_pdNumHits = true );
  declareProperty( "EnablePDHitMaps", m_pdHitMaps = true );
}

// Destructor
HPDHitsMoni::~HPDHitsMoni() { }

// Initialize
StatusCode HPDHitsMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDDecoder", m_decoder, 0, true );

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  return sc;
}

StatusCode HPDHitsMoni::prebookHistograms()
{
  const unsigned int nHPDs = m_richSys->allPDRichSmartIDs().size();

  // # Hits per HPD
  richProfile1D( HID("PDs/NumHitsVHPDCopyNumber"),
                 "Average # Hits V HPD Copy Number",
                 -0.5, nHPDs - 0.5, nHPDs );

  return StatusCode::SUCCESS;
}

// Main execution
StatusCode HPDHitsMoni::execute()
{

  // Obtain RichSmartIDs from raw decoding
  const DAQ::L1Map & data = m_decoder->allRichSmartIDs();

  // count pixels per PD column
  typedef std::map< unsigned int, unsigned int > PDColCount;
  typedef std::map< LHCb::RichSmartID, PDColCount > RichPDColCount;
  RichPDColCount pdColPixCount;

  // HPD hit counts for profile
  HPDCountMap hpdMap;
  // initialise for all active HPDs and L1 to 0
  initMap(hpdMap);

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
        const DAQ::HPDHardwareID hardID = m_richSys->hardwareID(hpd);
        const DAQ::Level0ID l0ID        = m_richSys->level0ID(hpd);
        const DAQ::HPDCopyNumber copyN  = m_richSys->copyNumber(hpd);

        // Vector of SmartIDs
        const LHCb::RichSmartID::Vector & rawIDs = (*iHPD).second.smartIDs();

        // save hits for profiles
        hpdMap[hpd] = rawIDs.size();

        // Detector info
        std::ostringstream detInfo;
        detInfo << "CopyN=" << copyN << " L0ID=" << l0ID << " hardID=" << hardID;

        // Alice or LHCb mode ? Use to define number of pixel rows for plots
        const int nPixRows = ( (*iHPD).second.header().aliceMode() ?
                               Rich::DAQ::MaxDataSizeALICE : Rich::DAQ::MaxDataSize );

        // number of hits plot
        if ( m_pdNumHits )
        {
          std::ostringstream HPD1,hpd1;
          HPD1 << "# hits " << hpd << " " << detInfo.str();
          hpd1 << "PDs/NumHits/CopyNum-" << copyN;
          richHisto1D( HID(hpd1.str()), HPD1.str(),
                       -0.5,100.5,101 ) -> fill( (double)rawIDs.size() );
        }

        // count hits per PD column
        (pdColPixCount[hpd.panelID()])[hpd.pdCol()] += rawIDs.size();

        // Loop over raw RichSmartIDs
        if ( m_pdHitMaps )
        {
          std::ostringstream HPD2,hpd2;
          HPD2 << "Hit Map "   << hpd << " " << detInfo.str();
          hpd2 << "PDs/HitMap/CopyNum-" << copyN;
          for ( LHCb::RichSmartID::Vector::const_iterator iR = rawIDs.begin();
                iR != rawIDs.end(); ++iR )
          {

            // compute overall pixel row (alice or LHCb mode)
            const int row = ( (*iHPD).second.header().aliceMode() ?
                              (*iR).pixelRow()*Rich::DAQ::NumAlicePixelsPerLHCbPixel + (*iR).pixelSubRow() :
                              (*iR).pixelRow() );

            // HPD hit map
            richHisto2D( HID(hpd2.str()), HPD2.str(),
                         -0.5,Rich::DAQ::NumPixelColumns-0.5,
                         Rich::DAQ::NumPixelColumns,
                         -0.5,nPixRows-0.5,
                         nPixRows ) -> fill( (*iR).pixelCol(), row );

          } // raw channel ids
        }

      } // loop over HPDs

    } // ingresses
  } // L1 boards

  // HPD column plots
  for ( RichPDColCount::const_iterator iR = pdColPixCount.begin();
        iR != pdColPixCount.end(); ++iR )
  {
    for ( PDColCount::const_iterator iC = (*iR).second.begin();
          iC != (*iR).second.end(); ++iC )
    {
      std::ostringstream id,title,col;
      col << (*iR).first.rich() << "-" << Rich::text((*iR).first.rich(),(*iR).first.panel())
          << "-Col" << (*iC).first;
      id << "PDCols/NumHits/" << col.str();
      title << "# Hits " << col.str();
      richHisto1D( HID(id.str()), title.str(), -0.5, 799.5, 100 ) -> fill( (double)(*iC).second );
    }
  }

    // loop over HPDs for profile plots
  for ( HPDCountMap::const_iterator iHPD = hpdMap.begin();
        iHPD != hpdMap.end(); ++iHPD )
  {
    if ( iHPD->first.isValid() )
    {
      // use a try block in case of DB lookup errors
      try
      {
        const Rich::DAQ::HPDCopyNumber copyN = m_richSys->copyNumber(iHPD->first);
        // fill plots
        richProfile1D(HID("PDs/NumHitsVHPDCopyNumber"))->fill(copyN.data(),iHPD->second);
      }
      catch ( const GaudiException & excpt )
      {
        Error( excpt.message() ).ignore();
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

void HPDHitsMoni::initMap( HPDCountMap & hpdMap )
{
  // clear the map
  hpdMap.clear();
  // get list of all active HPDs
  const LHCb::RichSmartID::Vector & hpds = m_richSys->activePDRichSmartIDs();
  // Loop over all HPDs and (re)set count to zero
  for ( LHCb::RichSmartID::Vector::const_iterator iHPD = hpds.begin();
        iHPD != hpds.end(); ++iHPD )
  {
    hpdMap[*iHPD] = 0;
  }
}

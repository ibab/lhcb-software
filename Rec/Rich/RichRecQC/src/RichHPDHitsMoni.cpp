
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
  acquireTool( "RichSmartIDDecoder", m_decoder, NULL, true );

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  // return
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
  for ( const auto& L1 : data )
  {
    // loop over ingresses for this L1 board
    for ( const auto& In : L1.second )
    {
      // Loop over HPDs in this ingress
      for ( const auto& HPD : In.second.hpdData() )
      {

        // Valid HPD ID
        if ( !HPD.second.hpdID().isValid() ) { continue; }
        // inhibited HPD ?
        if ( HPD.second.header().inhibit() ) { continue; }

        // HPD info
        const LHCb::RichSmartID hpd     = HPD.second.hpdID();
        const DAQ::HPDHardwareID hardID = m_richSys->hardwareID(hpd);
        const DAQ::Level0ID l0ID        = m_richSys->level0ID(hpd);
        const DAQ::HPDCopyNumber copyN  = m_richSys->copyNumber(hpd);

        // Vector of SmartIDs
        const LHCb::RichSmartID::Vector & rawIDs = HPD.second.smartIDs();

        // save hits for profiles
        hpdMap[hpd] = rawIDs.size();

        // Detector info
        std::ostringstream detInfo;
        detInfo << "CopyN=" << copyN << " L0ID=" << l0ID << " hardID=" << hardID;

        // Alice or LHCb mode ? Use to define number of pixel rows for plots
        const int nPixRows = ( HPD.second.header().aliceMode() ?
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
          for ( const auto& R : rawIDs )
          {

            // compute overall pixel row (alice or LHCb mode)
            const int row = ( HPD.second.header().aliceMode() ?
                              R.pixelRow()*Rich::DAQ::NumAlicePixelsPerLHCbPixel + R.pixelSubRow() :
                              R.pixelRow() );

            // HPD hit map
            richHisto2D( HID(hpd2.str()), HPD2.str(),
                         -0.5,Rich::DAQ::NumPixelColumns-0.5,
                         Rich::DAQ::NumPixelColumns,
                         -0.5,nPixRows-0.5,
                         nPixRows ) -> fill( R.pixelCol(), row );

          } // raw channel ids
        }

      } // loop over HPDs
    } // ingresses
  } // L1 boards

  // HPD column plots
  for ( const auto& R : pdColPixCount )
  {
    for ( const auto& C : R.second )
    {
      std::ostringstream id,title,col;
      col << R.first.rich() << "-" << Rich::text(R.first.rich(),R.first.panel())
          << "-Col" << C.first;
      id << "PDCols/NumHits/" << col.str();
      title << "# Hits " << col.str();
      richHisto1D( HID(id.str()), title.str(), -0.5, 799.5, 100 ) -> fill( (double)C.second );
    }
  }

    // loop over HPDs for profile plots
  for ( const auto& HPD : hpdMap )
  {
    if ( HPD.first.isValid() )
    {
      // use a try block in case of DB lookup errors
      try
      {
        const Rich::DAQ::HPDCopyNumber copyN = m_richSys->copyNumber(HPD.first);
        // fill plots
        richProfile1D(HID("PDs/NumHitsVHPDCopyNumber"))->fill(copyN.data(),HPD.second);
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
  for ( const auto& hpd : hpds ) { hpdMap[hpd] = 0; }
}


//-----------------------------------------------------------------------------
/** @file RichHPDHitsMoni.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::MC::HPDHitsMoni
 *
 *  $Id: RichHPDHitsMoni.cpp,v 1.8 2008-06-11 09:14:41 jonrob Exp $
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
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( HPDHitsMoni );

// Standard constructor, initializes variables
HPDHitsMoni::HPDHitsMoni( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_decoder           ( NULL ),
    m_richSys           ( NULL )
{
}

// Destructor
HPDHitsMoni::~HPDHitsMoni() {};

//  Initialize
StatusCode HPDHitsMoni::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichSmartIDDecoder", m_decoder, 0, true );

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  return sc;
}

// Main execution
StatusCode HPDHitsMoni::execute()
{

  // Histogramming
  const Rich::HistoID hid;

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
        const DAQ::HPDHardwareID hardID = m_richSys->hardwareID(hpd);
        const DAQ::Level0ID l0ID        = m_richSys->level0ID(hpd);
        const Rich::DetectorType rich   = hpd.rich();

        // Vector of SmartIDs
        const LHCb::RichSmartID::Vector & rawIDs = (*iHPD).second.smartIDs();

        // create histo title
        std::ostringstream HPD1,HPD2;
        HPD1 << "Number hits in HPD " << hpd << " L0ID=" << l0ID << " hardID=" << hardID;
        HPD2 << "Hit Map for HPD " << hpd << " L0ID=" << l0ID << " hardID=" << hardID;

        // Alice or LHCb mode ? Use to define number of pixel rows
        const int nPixRows = ( (*iHPD).second.header().aliceMode() ? 
                               Rich::DAQ::MaxDataSizeALICE : Rich::DAQ::MaxDataSize );

        plot1D( rawIDs.size(),
                hid(rich,"NumHits/"+(std::string)l0ID), HPD1.str(),
                -0.5,100.5,101 );

        // Loop over raw RichSmartIDs
        for ( LHCb::RichSmartID::Vector::const_iterator iR = rawIDs.begin();
              iR != rawIDs.end(); ++iR )
        {
          
          // compute overall pixel row (alice or LHCb mode)
          const int row = ( (*iHPD).second.header().aliceMode() ? 
                            (*iR).pixelRow()*Rich::DAQ::NumAlicePixelsPerLHCbPixel + (*iR).pixelSubRow() :
                            (*iR).pixelRow() );

          // fill plot
          plot2D( (*iR).pixelCol(), row,
                  hid(rich,"HitMaps/"+(std::string)l0ID), HPD2.str(),
                  -0.5,Rich::DAQ::NumPixelColumns-0.5,
                  -0.5,nPixRows-0.5,
                  Rich::DAQ::NumPixelColumns,nPixRows );

        } // raw channel ids

      } // loop over HPDs
    } // ingresses
  } // L1 boards

  return StatusCode::SUCCESS;
}


//-----------------------------------------------------------------------------
/** @file RichHPDHitsMoni.cpp
 *
 *  Implementation file for algorithm class : Rich::Rec::MC::HPDHitsMoni
 *
 *  $Id: RichHPDHitsMoni.cpp,v 1.3 2007-02-02 10:08:36 jonrob Exp $
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
  m_richSys = getDet<DeRichSystem>( DeRichLocation::RichSystem );

  return sc;
}

// Main execution
StatusCode HPDHitsMoni::execute()
{

  // Histogramming
  const Rich::HistoID hid;

  // Obtain RichSmartIDs from raw decoding
  const DAQ::PDMap & smartIDs = m_decoder->allRichSmartIDs();

  debug() << "Found data for " << smartIDs.size() << " HPDs" << endreq;

  // Loop over HPDs and RichSmartIDs
  for ( DAQ::PDMap::const_iterator iHPD = smartIDs.begin();
        iHPD != smartIDs.end(); ++iHPD )
  {
    // HPD info
    const LHCb::RichSmartID hpd     = (*iHPD).first;
    const DAQ::HPDHardwareID hardID = m_richSys->hardwareID(hpd);
    const DAQ::Level0ID l0ID        = m_richSys->level0ID(hpd);
    const Rich::DetectorType rich   = hpd.rich();

    // Vector of SmartIDs
    const LHCb::RichSmartID::Vector & rawIDs = (*iHPD).second;

    // create histo title
    std::ostringstream HPD1,HPD2;
    HPD1 << "Number hits in HPD " << hpd << " L0ID=" << l0ID << " hardID=" << hardID;
    HPD2 << "Hit Map for HPD " << hpd << " L0ID=" << l0ID << " hardID=" << hardID;

    plot1D( rawIDs.size(),
            hid(rich,"NumHits/"+(std::string)l0ID), HPD1.str(),
            -0.5,100.5,101 );

    // Loop over raw RichSmartIDs
    for ( LHCb::RichSmartID::Vector::const_iterator iR = rawIDs.begin();
          iR != rawIDs.end(); ++iR )
    {

      // fill plot
      plot2D( (*iR).pixelCol(), (*iR).pixelRow(),
              hid(rich,"HitMaps/"+(std::string)l0ID), HPD2.str(),
              -0.5,31.5,-0.5,31.5,32,32 );

    } // raw channel ids

  } // loop over HPDs

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode HPDHitsMoni::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}


//-----------------------------------------------------------------------------
/** @file RichHPDHitsMoni.cpp
 *
 *  Implementation file for algorithm class : RichHPDHitsMoni
 *
 *  $Id: RichHPDHitsMoni.cpp,v 1.1 2006-09-25 06:24:47 jonrob Exp $
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
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichHPDHitsMoni>          s_factory ;
const        IAlgFactory& RichHPDHitsMoniFactory = s_factory ;


// Standard constructor, initializes variables
RichHPDHitsMoni::RichHPDHitsMoni( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : RichRecHistoAlgBase ( name, pSvcLocator ),
    m_decoder           ( NULL ),
    m_richSys           ( NULL )
{
}

// Destructor
RichHPDHitsMoni::~RichHPDHitsMoni() {};

//  Initialize
StatusCode RichHPDHitsMoni::initialize()
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
StatusCode RichHPDHitsMoni::execute()
{

  // Histogramming
  const RichHistoID hid;

  // Obtain RichSmartIDs from raw decoding
  const RichDAQ::PDMap & smartIDs = m_decoder->allRichSmartIDs();

  debug() << "Found data for " << smartIDs.size() << " HPDs" << endreq;

  // Loop over HPDs and RichSmartIDs
  for ( RichDAQ::PDMap::const_iterator iHPD = smartIDs.begin();
        iHPD != smartIDs.end(); ++iHPD )
  {
    // HPD info
    const RichSmartID hpd               = (*iHPD).first;
    const RichDAQ::HPDHardwareID hardID = m_richSys->hardwareID(hpd);
    const RichDAQ::Level0ID l0ID        = m_richSys->level0ID(hpd);
    const Rich::DetectorType rich       = hpd.rich();

    // Vector of SmartIDs
    const RichSmartID::Vector & rawIDs = (*iHPD).second;

    // create histo title
    std::ostringstream HPD1,HPD2;
    HPD1 << "Number hits in HPD " << hpd << " L0ID=" << l0ID << " hardID=" << hardID;
    HPD2 << "Hit Map for HPD " << hpd << " L0ID=" << l0ID << " hardID=" << hardID;

    plot1D( rawIDs.size(),
            hid(rich,"NumHits/"+(std::string)l0ID), HPD1.str(), 
            -0.5,100.5,101 );

    // Loop over raw RichSmartIDs
    for ( RichSmartID::Vector::const_iterator iR = rawIDs.begin();
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
StatusCode RichHPDHitsMoni::finalize()
{
  // Execute base class method
  return RichRecHistoAlgBase::finalize();
}


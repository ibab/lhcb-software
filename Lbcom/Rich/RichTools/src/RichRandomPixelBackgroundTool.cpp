
//-----------------------------------------------------------------------------
/** @file RichRandomPixelBackgroundTool.cpp
 *
 *  Implementation file for tool : Rich::RandomPixelBackgroundTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2008-10-10
 */
//-----------------------------------------------------------------------------

// local
#include "RichRandomPixelBackgroundTool.h"

//-----------------------------------------------------------------------------

using namespace Rich;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RandomPixelBackgroundTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  RandomPixelBackgroundTool::RandomPixelBackgroundTool( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent )
    : Rich::ToolBase  ( type, name , parent ),
      m_richSys       ( NULL                ),
      m_randSvc       ( NULL                )
{
  // tool interface
  declareInterface<Rich::IAddBackground>(this);
  // JOs
  declareProperty( "PixelBackgroundProb", m_pixBackgroundProb = 0.01 );
}

//=============================================================================
// Destructor
//=============================================================================
RandomPixelBackgroundTool::~RandomPixelBackgroundTool() { }

//=============================================================================

StatusCode RandomPixelBackgroundTool::initialize()
{
  const StatusCode sc = Rich::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  // random numbers
  m_randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );

  info() << "Will add random pixel background at P=" << m_pixBackgroundProb << endmsg;

  return sc;
}

//=============================================================================

StatusCode
RandomPixelBackgroundTool::createBackgrounds( HPDBackgrounds & backgrounds,
                                              const bool aliceMode ) const
{
  const Rich::DAQ::ShortType nPixRows = ( aliceMode ?
                                          Rich::DAQ::MaxDataSizeALICE :
                                          Rich::DAQ::MaxDataSize );
  const Rich::DAQ::ShortType nPixsPerHPD = Rich::DAQ::NumPixelColumns * nPixRows;

  // Get the list of all active HPDs
  const LHCb::RichSmartID::Vector & hpds = m_richSys->activePDRichSmartIDs();

  // make the binominal random number generator
  Rndm::Numbers poisrand ( m_randSvc, Rndm::Binomial(nPixsPerHPD,m_pixBackgroundProb) );
  // make a flat random number distribution between 0 and 1
  Rndm::Numbers flatrand ( m_randSvc, Rndm::Flat(0,1) );

  // loop over all HPDs
  for ( LHCb::RichSmartID::Vector::const_iterator iHPD = hpds.begin();
        iHPD != hpds.end(); ++iHPD )
  {
    // Throw a dice to determine how many pixels have background in this HPD
    int nBackPixs = (int)poisrand();
    if ( nBackPixs>0 )
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Adding " << nBackPixs << " background hits to " << *iHPD << endmsg;
      LHCb::RichSmartID::Vector & hits = backgrounds[*iHPD];
      const unsigned int maxTries(9999);
      unsigned int tries(0);
      while ( nBackPixs>0 && tries<maxTries )
      {
        // Make a random background hit
        LHCb::RichSmartID pixID(*iHPD);
        pixID.setPixelRow( (int)(flatrand() * nPixRows)                   );
        pixID.setPixelCol( (int)(flatrand() * Rich::DAQ::NumPixelColumns) );
        if ( aliceMode )
        { pixID.setPixelSubRow( (int)(flatrand() * Rich::DAQ::NumAlicePixelsPerLHCbPixel) ); }
        // is this hit already in the list ...
        if ( hits.end() == std::find(hits.begin(),hits.end(),pixID) )
        {
          // add it to the output data
          if ( msgLevel(MSG::DEBUG) ) debug() << " -> background hit " << pixID << endmsg;
          hits.push_back(pixID);
          --nBackPixs;
        }
        if ( ++tries == maxTries ) Warning( "Max tries exceeded... This is abnormal" ).ignore();
      } // loop over hits to add
    } // at least one background hit
  } // loop over active HPDs

  // finish with random number generators
  poisrand.finalize();
  flatrand.finalize();

  // return
  return StatusCode::SUCCESS;
}

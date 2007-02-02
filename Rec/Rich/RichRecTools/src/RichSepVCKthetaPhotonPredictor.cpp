
//-----------------------------------------------------------------------------
/** @file RichSepVCKthetaPhotonPredictor.cpp
 *
 *  Implementation file for tool : Rich::Rec::SepVCKthetaPhotonPredictor
 *
 *  CVS Log :-
 *  $Id: RichSepVCKthetaPhotonPredictor.cpp,v 1.9 2007-02-02 10:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   01/06/2005
 */
//-----------------------------------------------------------------------------

// local
#include "RichSepVCKthetaPhotonPredictor.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( SepVCKthetaPhotonPredictor );

// Standard constructor
SepVCKthetaPhotonPredictor::
SepVCKthetaPhotonPredictor( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_geomTool      ( NULL ),
    m_ckAngle       ( NULL ),
    m_ckRes         ( NULL ),
    m_minROI        ( Rich::NRadiatorTypes, 0 ),
    m_maxROI        ( Rich::NRadiatorTypes, 0 ),
    m_ckThetaMax    ( Rich::NRadiatorTypes, 0 ),
    m_sep2GMax      ( Rich::NRadiatorTypes, 0 ),
    m_tolF          ( Rich::NRadiatorTypes, 0 ),
    m_scale         ( Rich::NRadiatorTypes, 0 ),
    m_Nselected     ( Rich::NRadiatorTypes, 0 ),
    m_Nreject       ( Rich::NRadiatorTypes, 0 )
{

  // interface
  declareInterface<IPhotonPredictor>(this);

  // job options

  m_minROI[Rich::Aerogel]  = 130; // aerogel
  m_minROI[Rich::Rich1Gas] = 0;   // rich1Gas
  m_minROI[Rich::Rich2Gas] = 0;   // rich2Gas
  declareProperty( "MinTrackROI", m_minROI );

  m_maxROI[Rich::Aerogel]  = 350;   // aerogel
  m_maxROI[Rich::Rich1Gas] =  90;   // rich1Gas
  m_maxROI[Rich::Rich2Gas] = 200;   // rich2Gas
  declareProperty( "MaxTrackROI", m_maxROI );

  m_ckThetaMax[Rich::Aerogel]  = 350;
  m_ckThetaMax[Rich::Rich1Gas] =  90;
  m_ckThetaMax[Rich::Rich2Gas] = 200;
  declareProperty( "CKthetaMax", m_ckThetaMax );

  m_sep2GMax[Rich::Aerogel]   = 10;
  m_sep2GMax[Rich::Rich1Gas]  = 10;
  m_sep2GMax[Rich::Rich2Gas]  = 10;
  declareProperty( "Sep2GMax", m_sep2GMax );

  m_tolF[Rich::Aerogel]   = 0.1;
  m_tolF[Rich::Rich1Gas]  = 0.1;
  m_tolF[Rich::Rich2Gas]  = 0.1;
  declareProperty( "TolerenceFactor", m_tolF );

}

StatusCode SepVCKthetaPhotonPredictor::initialize()
{
  // Initialise base class
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichCherenkovAngle",        m_ckAngle  );
  acquireTool( "RichRecGeometry",           m_geomTool );
  acquireTool( "RichCherenkovResolution",   m_ckRes    );

  // loop over radiators
  for ( int rad = 0; rad < Rich::NRadiatorTypes; ++rad )
  {
    // cache some numbers
    m_minROI2.push_back( m_minROI[rad]*m_minROI[rad] );
    m_maxROI2.push_back( m_maxROI[rad]*m_maxROI[rad] );
    m_scale[rad] = (m_sep2GMax[rad]/m_ckThetaMax[rad]);
    // printout for this rad
    std::string trad = Rich::text((Rich::RadiatorType)rad);
    trad.resize(8,' ');
    info() << trad << " : Sep. range     " << format("%5.1f",m_minROI[rad]) << " -> "
           << format("%5.1f",m_maxROI[rad]) << " mm  : Tol. "
           << format("%5.1f",m_tolF[rad]) << " mm" << endreq;
  }

  return sc;
}

StatusCode SepVCKthetaPhotonPredictor::finalize()
{

  if ( m_Nselected[Rich::Aerogel]  > 0 ||
       m_Nselected[Rich::Rich1Gas] > 0 ||
       m_Nselected[Rich::Rich2Gas] > 0 )
  {

    // statistical tool
    const PoissonEffFunctor occ("%10.2f +-%7.2f");

    // printout stats
    info() << "=================================================================" << endreq
           << "  Pixel/Segment combination selection summary :-" << endreq
           << "    Aerogel   : "
           << occ(m_Nselected[Rich::Aerogel],m_Nselected[Rich::Aerogel]+m_Nreject[Rich::Aerogel])
           << " % of possible candidates" << endreq
           << "    Rich1Gas  : "
           << occ(m_Nselected[Rich::Rich1Gas],m_Nselected[Rich::Rich1Gas]+m_Nreject[Rich::Rich1Gas])
           << " % of possible candidates" << endreq
           << "    Rich2Gas  : "
           << occ(m_Nselected[Rich::Rich2Gas],m_Nselected[Rich::Rich2Gas]+m_Nreject[Rich::Rich2Gas])
           << " % of possible candidates" << endreq
           << "=================================================================" << endreq;
  }

  // Execute base class method
  return RichRecToolBase::finalize();
}

// fast decision on whether a photon is possible
bool
SepVCKthetaPhotonPredictor::photonPossible( LHCb::RichRecSegment * segment,
                                            LHCb::RichRecPixel * pixel ) const
{

  // Default to not selected
  bool OK = false;

  // which radiator
  const Rich::RadiatorType rad = segment->trackSegment().radiator();

  // Are they in the same Rich detector ?
  if ( segment->trackSegment().rich() == pixel->detector() )
  {

    // segment / hit separation squared
    const double sep2 = m_geomTool->trackPixelHitSep2(segment,pixel);

    // Check overall boundaries
    if ( sep2 < m_maxROI2[rad] && sep2 > m_minROI2[rad] )
    {

      // Cache separation
      const double sep = sqrt(sep2);

      // Loop over mass hypos and check finer grained boundaries
      for ( int ihypo = 0; ihypo < Rich::NParticleTypes; ++ihypo )
      {
        const Rich::ParticleIDType id = static_cast<Rich::ParticleIDType>(ihypo);

        // Get expected CK theta for this segment and hypo
        const double ckAngle = m_ckAngle->avgCherenkovTheta(segment,id);

        // expected separation, scales linearly with expected CK angle
        const double expSep = ckAngle * m_scale[rad];

        // is this pixel/segment pair in the accepted range
        const double dsep = fabs(sep-expSep);
        if ( dsep < m_tolF[rad] )
        {
          OK = true;
          if ( msgLevel(MSG::VERBOSE) )
          {
            verbose() << "  -> " << id << " fabs(sep-expSep)="
                      << dsep << " PASSED tol=" << m_tolF[rad] << endreq;
          }
          break;
        }
        if ( msgLevel(MSG::VERBOSE) && !OK )
        {
          verbose() << "  -> " << id << " fabs(sep-expSep)="
                    << dsep << " FAILED tol=" << m_tolF[rad] << " -> reject" << endreq;
        }

      } // loop over hypos

    } // overall boundary check
    else if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "  -> sep2=" << sep2
                << " FAILED overall boundary check " << m_minROI2[rad] << "->" << m_maxROI2[rad]
                << " -> reject" << endreq;
    }

  } // same detector
  else if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "  -> " << " FAILED RICH detector check -> reject" << endreq;
  }

  if ( OK ) { ++m_Nselected[rad]; }
  else      { ++m_Nreject[rad];   }

  return OK;
}



//-----------------------------------------------------------------------------
/** @file RichSepVCKthetaPhotonPredictor.cpp
 *
 *  Implementation file for tool : RichSepVCKthetaPhotonPredictor
 *
 *  CVS Log :-
 *  $Id: RichSepVCKthetaPhotonPredictor.cpp,v 1.1 2005-06-17 15:08:36 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   01/06/2005
 */
//-----------------------------------------------------------------------------

// local
#include "RichSepVCKthetaPhotonPredictor.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichSepVCKthetaPhotonPredictor>          s_factory ;
const        IToolFactory& RichSepVCKthetaPhotonPredictorFactory = s_factory ;

// Standard constructor
RichSepVCKthetaPhotonPredictor::
RichSepVCKthetaPhotonPredictor( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_geomTool      ( 0 ),
    m_ckAngle       ( 0 ),
    m_ckRes         ( 0 ),
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
  declareInterface<IRichPhotonPredictor>(this);

  // job options

  m_minROI[Rich::Aerogel] = 130; // aerogel
  m_minROI[Rich::C4F10]   = 0;   // c4f10
  m_minROI[Rich::CF4]     = 0;   // cf4
  declareProperty( "MinTrackROI", m_minROI );

  m_maxROI[Rich::Aerogel] = 350;   // aerogel
  m_maxROI[Rich::C4F10]   =  90;   // c4f10
  m_maxROI[Rich::CF4]     = 200;   // cf4
  declareProperty( "MaxTrackROI", m_maxROI );

  m_ckThetaMax[Rich::Aerogel] = 350;
  m_ckThetaMax[Rich::C4F10]   =  90;
  m_ckThetaMax[Rich::CF4]     = 200;
  declareProperty( "CKthetaMax", m_ckThetaMax );

  m_sep2GMax[Rich::Aerogel] = 10;
  m_sep2GMax[Rich::C4F10]   = 10;
  m_sep2GMax[Rich::CF4]     = 10;
  declareProperty( "Sep2GMax", m_sep2GMax );

  m_tolF[Rich::Aerogel] = 0.1;
  m_tolF[Rich::C4F10]   = 0.1;
  m_tolF[Rich::CF4]     = 0.1;
  declareProperty( "TolerenceFactor", m_tolF );

}

StatusCode RichSepVCKthetaPhotonPredictor::initialize()
{
  // Initialise base class
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichRecGeomTool",         m_geomTool );
  acquireTool( "RichCherenkovAngle",      m_ckAngle  );
  acquireTool( "RichCherenkovResolution", m_ckRes    );

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

StatusCode RichSepVCKthetaPhotonPredictor::finalize()
{

  if ( m_Nselected[Rich::Aerogel] > 0 ||
       m_Nselected[Rich::C4F10]   > 0 ||
       m_Nselected[Rich::CF4]     > 0 )
  {

    // statistical tool
    RichPoissonEffFunctor occ("%10.2f +-%7.2f");

    // printout stats
    info() << "=================================================================" << endreq
           << "  Pixel/Segment combination selection summary :-" << endreq
           << "    Aerogel   : "
           << occ(m_Nselected[Rich::Aerogel],m_Nselected[Rich::Aerogel]+m_Nreject[Rich::Aerogel])
           << " % of possible candidates" << endreq
           << "    C4F10     : "
           << occ(m_Nselected[Rich::C4F10],m_Nselected[Rich::C4F10]+m_Nreject[Rich::C4F10])
           << " % of possible candidates" << endreq
           << "    CF4       : "
           << occ(m_Nselected[Rich::CF4],m_Nselected[Rich::CF4]+m_Nreject[Rich::CF4])
           << " % of possible candidates" << endreq
           << "=================================================================" << endreq;
  }

  // Execute base class method
  return RichRecToolBase::finalize();
}

// fast decision on whether a photon is possible
bool RichSepVCKthetaPhotonPredictor::photonPossible( RichRecSegment * segment,
                                                     RichRecPixel * pixel ) const
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
        if ( sep > (expSep - m_tolF[rad]) &&
             sep < (expSep + m_tolF[rad]) )
        {
          OK = true;
          break;
        }

      }

    }

  }

  if ( OK )
  {
    ++m_Nselected[rad];
  }
  else
  {
    ++m_Nreject[rad];
  }

  return OK;

}


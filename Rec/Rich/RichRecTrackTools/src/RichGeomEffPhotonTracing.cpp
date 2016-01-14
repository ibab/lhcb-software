
//-----------------------------------------------------------------------------
/** @file RichGeomEffPhotonTracing.cpp
 *
 *  Implementation file for tool : Rich::Rec::GeomEffPhotonTracing
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichGeomEffPhotonTracing.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

// Standard constructor
GeomEffPhotonTracing::GeomEffPhotonTracing ( const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent )
  : ToolBase( type, name, parent )
{

  // define interface
  declareInterface<IGeomEff>(this);

  // Define job option parameters
  declareProperty( "NPhotonsGeomEffCalc",    m_nGeomEff        = 100   );
  declareProperty( "NPhotonsGeomEffBailout", m_nGeomEffBailout = 20    );
  declareProperty( "CheckHPDsAreActive",     m_hpdCheck        = false );
  declareProperty( "CheckBeamPipe", m_checkBeamPipe            = false );
  declareProperty( "UseDetailedHPDsInRayTracing", m_useDetailedHPDsForRayT = false );

}

StatusCode GeomEffPhotonTracing::initialize()
{
  using namespace Gaudi::Units;

  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",     m_rayTrace, NULL, true );
  acquireTool( "RichCherenkovAngle", m_ckAngle  );
  if ( m_hpdCheck )
  {
    m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );
    Warning( "Will check each pixel for HPD status. Takes additional CPU.",
             StatusCode::SUCCESS ).ignore();
  }

  // Set up cached parameters for geometrical efficiency calculation
  m_pdInc             = 1.0   / static_cast<double>(m_nGeomEff);
  const double incPhi = twopi / static_cast<double>(m_nGeomEff);
  double ckPhi = 0;
  m_phiValues.clear();
  for ( int iPhot = 0; iPhot < m_nGeomEff; ++iPhot, ckPhi+=incPhi )
  {
    m_phiValues.push_back(ckPhi);
  }

  // configure ray tracing
  m_traceMode = LHCb::RichTraceMode( LHCb::RichTraceMode::RespectHPDTubes,
                                     ( m_useDetailedHPDsForRayT ? 
                                       LHCb::RichTraceMode::FullHPDs : 
                                       LHCb::RichTraceMode::SimpleHPDs ) );
  if ( m_checkBeamPipe ) { m_traceMode.setBeamPipeIntersects(true); }
  _ri_debug << "Sampling Mode : " << m_traceMode << endmsg;

  return sc;
}

double
GeomEffPhotonTracing::geomEfficiency ( LHCb::RichRecSegment * segment,
                                       const Rich::ParticleIDType id ) const
{
  // Protect against the non-physical below threshold hypothesis
  if ( Rich::BelowThreshold == id ) return 0;

  if ( !segment->geomEfficiency().dataIsValid(id) )
  {
    double eff = 0;

    // Cherenkov theta for this segment/hypothesis combination
    const double ckTheta = m_ckAngle->avgCherenkovTheta( segment, id );
    if ( ckTheta > 0 )
    {

      _ri_verbo << "geomEfficiency : Trying segment " << segment->key() << " CK theta = " << ckTheta
                << " track Dir "
                << segment->trackSegment().bestMomentum().Unit()
                << endmsg;

      int nDetect(0), iPhot(0);
      for ( PhiList::const_iterator ckPhi = m_phiValues.begin();
            ckPhi != m_phiValues.end(); ++iPhot, ++ckPhi )
      {

        // Photon emission point is half-way between segment start and end points
        const Gaudi::XYZPoint & emissionPt = segment->trackSegment().bestPoint();

        // Photon direction around loop
        const Gaudi::XYZVector photDir = segment->trackSegment().vectorAtThetaPhi( ckTheta, *ckPhi );

        //if ( msgLevel(MSG::VERBOSE) )
        //{
        //  verbose() << " -> fake photon " << photDir
        //            << " testAng "
        //            << Rich::Geom::AngleBetween( segment->trackSegment().bestMomentum(), photDir )
        //            << endmsg;
        //}

        // Ray trace through detector, using fast circle modelling of HPDs
        LHCb::RichGeomPhoton photon;
        LHCb::RichTraceMode mode( m_traceMode );
        mode.setAeroRefraction ( segment->trackSegment().radiator() == Rich::Aerogel ); 
        const LHCb::RichTraceMode::RayTraceResult result = 
          m_rayTrace->traceToDetector( segment->trackSegment().rich(),
                                       emissionPt,
                                       photDir,
                                       photon,
                                       mode, Rich::top,
                                       segment->trackSegment().avPhotonEnergy() );
        if ( m_traceMode.traceWasOK(result) )
        {
          // Check HPD status
          if ( m_hpdCheck && !m_richSys->pdIsActive(photon.smartID().pdID()) ) continue;

          _ri_verbo << " -> photon was traced to detector at " 
                    << photon.smartID().pdID() << " "
                    << photon.detectionPoint()
                    << endmsg;

          // count detected photons
          ++nDetect;

          // update efficiency per HPD tally
          segment->addToGeomEfficiencyPerPD( id,
                                             photon.smartID().pdID(),
                                             (LHCb::RichRecSegment::FloatType)(m_pdInc) );

          // flag regions where we expect hits for this segment
          if ( photon.detectionPoint().x() > 0 )
          {
            segment->setPhotonsInXPlus(true);
          }
          else
          {
            segment->setPhotonsInXMinus(true);
          }
          if ( photon.detectionPoint().y() > 0 )
          {
            segment->setPhotonsInYPlus(true);
          }
          else
          {
            segment->setPhotonsInYMinus(true);
          }

        }

        // Bail out if tried m_geomEffBailout times and all have failed
        if ( 0 == nDetect && iPhot >= m_nGeomEffBailout ) break;

      } // fake photon loop

      // compute the final eff
      eff = static_cast<double>(nDetect)/static_cast<double>(m_nGeomEff);

    }

    // store result
    segment->setGeomEfficiency( id, (LHCb::RichRecSegment::FloatType)(eff) );
    _ri_debug << "Segment has " << id << " geom. eff. " << eff << endmsg;

  }

  // return result
  return segment->geomEfficiency( id );
}

double
GeomEffPhotonTracing::geomEfficiencyScat ( LHCb::RichRecSegment * segment,
                                           const Rich::ParticleIDType id ) const
{
  // Protect against the non-physical below threshold hypothesis
  if ( Rich::BelowThreshold == id ) return 0;

  if ( !segment->geomEfficiencyScat().dataIsValid(id) )
  {
    /** use same value as for normal Geom Eff.
     *  @todo Look to improving this by taking into account the cos^2 scattering
     *        probability. Need to do this though without using random numbers ...
     */
    segment->setGeomEfficiencyScat( id, 
                                    (LHCb::RichRecSegment::FloatType)(geomEfficiency(segment,id)) );
  }

  // return result fo this id type
  return segment->geomEfficiencyScat( id );
}

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( GeomEffPhotonTracing )

//-----------------------------------------------------------------------------

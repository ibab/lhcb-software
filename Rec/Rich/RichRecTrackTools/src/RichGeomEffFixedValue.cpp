
//-----------------------------------------------------------------------------
/** @file RichGeomEffFixedValue.cpp
 *
 *  Implementation file for tool : Rich::Rec::GeomEffFixedValue
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichGeomEffFixedValue.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( GeomEffFixedValue )

// Standard constructor
GeomEffFixedValue::GeomEffFixedValue ( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
  : ToolBase             ( type, name, parent ),
    m_ckAngle            ( NULL ),
    m_geomTool           ( NULL ),
    m_fixedValue         ( Rich::NRadiatorTypes, 0.7 ),
    m_fixedScatValue     ( 0.7 ),
    m_checkBoundaries    ( false ),
    m_checkPhotonRegions ( false )
{

  // interface
  declareInterface<IGeomEff>(this);

  // job options
  declareProperty( "FixedSignalEfficiency",  m_fixedValue       );
  declareProperty( "FixedScatterEfficiency", m_fixedScatValue   );
  declareProperty( "CheckHPDPanelBoundaries", m_checkBoundaries );
  declareProperty( "CheckPhotonRegions", m_checkPhotonRegions   );

}

StatusCode GeomEffFixedValue::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle );
  acquireTool( "RichRecGeometry", m_geomTool   );

  info() << "Fixed geometrical efficiencies (aero/rich1Gas/rich2Gas) : " << m_fixedValue << endmsg
         << "Fixed scattered efficiency                              : " << m_fixedScatValue << endmsg;
  if ( m_checkBoundaries )
  {
    info() << "Will take into account average HPD panel boundaries" << endmsg;
  }
  else
  {
    info() << "Will assume infinite HPD panel boundaries" << endmsg;
  }

  return sc;
}

double
GeomEffFixedValue::geomEfficiency ( LHCb::RichRecSegment * segment,
                                    const Rich::ParticleIDType id ) const
{
  // Protect against the non-physical below threshold hypothesis
  if ( Rich::BelowThreshold == id ) return 0;

  if ( !segment->geomEfficiency().dataIsValid(id) )
  {
    double eff = 0;

    // Cherenkov theta
    const double ckTh = m_ckAngle->avgCherenkovTheta( segment, id );

    if ( ckTh > 0 )
    {
      // First get the HPD panel acceptance (edges)
      eff = ( m_checkBoundaries ?
              m_geomTool->hpdPanelAcceptance(segment,id) : 1 );
      // .. next, scale by the average panel acceptance (circular HPDs)
      eff *= m_fixedValue[segment->trackSegment().radiator()];
    }

    // Set the geom eff
    segment->setGeomEfficiency( id, (LHCb::RichRecSegment::FloatType)(eff) );

    if ( m_checkPhotonRegions )
    {

      // Track impact point on HPD panel
      const Gaudi::XYZPoint & tkPoint = segment->pdPanelHitPointLocal();

      // radius of ring for given hypothesis
      const double rSig = m_ckAngle->avCKRingRadiusLocal(segment,id);

      // flag where hits could be
      if ( tkPoint.y()+rSig > 0 ) segment->setPhotonsInYPlus(true);
      if ( tkPoint.y()-rSig < 0 ) segment->setPhotonsInYMinus(true);
      if ( tkPoint.x()+rSig > 0 ) segment->setPhotonsInXPlus(true);
      if ( tkPoint.x()-rSig < 0 ) segment->setPhotonsInXMinus(true);

    }
    else
    {

      // assume hits in all regions
      segment->setPhotonsInYPlus(true);
      segment->setPhotonsInYMinus(true);
      segment->setPhotonsInXPlus(true);
      segment->setPhotonsInXMinus(true);

    }

  }

  return segment->geomEfficiency( id );
}

double
GeomEffFixedValue::geomEfficiencyScat ( LHCb::RichRecSegment * segment,
                                        const Rich::ParticleIDType id ) const
{
  // Protect against the non-physical below threshold hypothesis
  if ( Rich::BelowThreshold == id ) return 0;

  if ( !segment->geomEfficiencyScat().dataIsValid(id) )
  {

    float eff = 0;
    if ( segment->trackSegment().radiator() == Rich::Aerogel ) 
    {
      eff = float( m_ckAngle->avgCherenkovTheta(segment,id) > 0 ? m_fixedScatValue : 0.0f );
    }

    // Assign this efficiency to all hypotheses
    segment->setGeomEfficiencyScat( Rich::Electron, eff );
    segment->setGeomEfficiencyScat( Rich::Muon, eff );
    segment->setGeomEfficiencyScat( Rich::Pion, eff );
    segment->setGeomEfficiencyScat( Rich::Kaon, eff );
    segment->setGeomEfficiencyScat( Rich::Proton, eff );

  }

  return segment->geomEfficiencyScat( id );
}

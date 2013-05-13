
//-----------------------------------------------------------------------------
/** @file RichGeomEffCKMassRing.cpp
 *
 *  Implementation file for tool : Rich::Rec::GeomEffCKMassRing
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichGeomEffCKMassRing.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( GeomEffCKMassRing )

// Standard constructor
GeomEffCKMassRing::GeomEffCKMassRing ( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
  : ToolBase          ( type, name, parent ),
    m_ckAngle         ( NULL ),
    m_massHypoRings   ( NULL ),
    m_richPartProp    ( NULL )
{

  // define interface
  declareInterface<IGeomEff>(this);
  // JOs
  declareProperty( "UseFirstRingForAll", m_useFirstRingForAll = false );

}

StatusCode GeomEffCKMassRing::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle  );
  acquireTool( "RichMassHypoRings", m_massHypoRings );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  m_pidTypes = m_richPartProp->particleTypes();
  info() << "Particle types considered = " << m_pidTypes << endmsg;

  return sc;
}

double
GeomEffCKMassRing::geomEfficiency ( LHCb::RichRecSegment * segment,
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

      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "geomEfficiency : Trying segment " << segment->key() << " CK theta = " << ckTheta
                  << " track Dir "
                  << segment->trackSegment().bestMomentum().Unit()
                  << endmsg;
      }

      // Load the CK mass ring
      LHCb::RichRecRing * ring = m_massHypoRings->massHypoRing( segment, id );
      if ( ring )
      {

        // The ring points
        const LHCb::RichRecPointOnRing::Vector & points = ring->ringPoints();

        int nDetect(0);
        const double pdInc = 1.0 / static_cast<double>(ring->nTotalPointSamples());
        for ( LHCb::RichRecPointOnRing::Vector::const_iterator iP = points.begin();
              iP != points.end(); ++iP )
        {

          if ( (*iP).acceptance() == LHCb::RichRecPointOnRing::InHPDTube )
          {

            // The HPD ID
            const LHCb::RichSmartID hpdID = (*iP).smartID().pdID();

            if ( msgLevel(MSG::VERBOSE) )
            {
              verbose() << " -> photon was traced to detector at "
                        << hpdID << " "
                        << (*iP).globalPosition()
                        << endmsg;
            }

            // count detected photons
            ++nDetect;

            // update efficiency per HPD tally
            
            if ( m_useFirstRingForAll )
            {
              for ( Rich::Particles::const_iterator hypo = m_pidTypes.begin();
                    hypo != m_pidTypes.end(); ++hypo )
              { 
                segment->addToGeomEfficiencyPerPD( *hypo, hpdID, 
                                                   (LHCb::RichRecSegment::FloatType)(pdInc) );
              }
            }
            else
            {
              segment->addToGeomEfficiencyPerPD( id, hpdID, 
                                                 (LHCb::RichRecSegment::FloatType)(pdInc) );
            }

            // flag regions where we expect hits for this segment
            if ( (*iP).globalPosition().x() > 0 )
            {
              segment->setPhotonsInXPlus(true);
            }
            else
            {
              segment->setPhotonsInXMinus(true);
            }
            if ( (*iP).globalPosition().y() > 0 )
            {
              segment->setPhotonsInYPlus(true);
            }
            else
            {
              segment->setPhotonsInYMinus(true);
            }

          } // in HPD tube

        } // fake photon loop

        // compute the final eff
        eff = static_cast<double>(nDetect)/static_cast<double>(ring->nTotalPointSamples());

      }
      else
      {
        Error( "Null RichRecRing pointer -> return 0" );
      }

    }

    // store result
    if ( m_useFirstRingForAll )
    {
      for ( Rich::Particles::const_iterator hypo = m_pidTypes.begin();
            hypo != m_pidTypes.end(); ++hypo )
      { 
        segment->setGeomEfficiency( *hypo, (LHCb::RichRecSegment::FloatType)(eff) );
      }
    }
    else
    {
      segment->setGeomEfficiency( id, (LHCb::RichRecSegment::FloatType)(eff) );
      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Segment "
          //<< segment->key()
                << " has " << id << " geom. eff. " << eff << endmsg;
      }
    }

  }

  // return result
  return segment->geomEfficiency( id );
}

double
GeomEffCKMassRing::geomEfficiencyScat ( LHCb::RichRecSegment * segment,
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

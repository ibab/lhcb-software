
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

// Standard constructor
GeomEffCKMassRing::GeomEffCKMassRing ( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
  : ToolBase( type, name, parent )
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
  acquireTool( "RichCherenkovAngle",     m_ckAngle       );
  acquireTool( "RichMassHypoRings",      m_massHypoRings );
  acquireTool( "RichParticleProperties", m_richPartProp  );

  m_pidTypes = m_richPartProp->particleTypes();
  _ri_debug << "Particle types considered = " << m_pidTypes << endmsg;

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

      _ri_verbo << "geomEfficiency : Trying segment " << segment->key() << " CK theta = " << ckTheta
                << " track Dir "
                << segment->trackSegment().bestMomentum().Unit()
                << endmsg;

      // Load the CK mass ring
      LHCb::RichRecRing * ring = m_massHypoRings->massHypoRing( segment, id );
      if ( ring )
      {

        // The ring points
        const LHCb::RichRecPointOnRing::Vector & points = ring->ringPoints();

        int nDetect(0);
        const double pdInc = 1.0 / static_cast<double>(ring->nTotalPointSamples());
        for ( const auto& P : points )
        {

          if ( P.acceptance() == LHCb::RichRecPointOnRing::InHPDTube )
          {

            // The HPD ID
            const LHCb::RichSmartID hpdID = P.smartID().pdID();

            _ri_verbo << " -> photon was traced to detector at "
                      << hpdID << " "
                      << P.globalPosition()
                      << endmsg;
            
            // count detected photons
            ++nDetect;

            // update efficiency per HPD tally

            if ( m_useFirstRingForAll )
            {
              for ( const auto& hypo : m_pidTypes )
              {
                segment->addToGeomEfficiencyPerPD( hypo, hpdID,
                                                   (LHCb::RichRecSegment::FloatType)(pdInc) );
              }
            }
            else
            {
              segment->addToGeomEfficiencyPerPD( id, hpdID,
                                                 (LHCb::RichRecSegment::FloatType)(pdInc) );
            }

            // flag regions where we expect hits for this segment
            if ( P.globalPosition().x() > 0 )
            {
              segment->setPhotonsInXPlus(true);
            }
            else
            {
              segment->setPhotonsInXMinus(true);
            }
            if ( P.globalPosition().y() > 0 )
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
      for ( const auto& hypo : m_pidTypes )
      {
        segment->setGeomEfficiency( hypo, (LHCb::RichRecSegment::FloatType)(eff) );
      }
    }
    else
    {
      segment->setGeomEfficiency( id, (LHCb::RichRecSegment::FloatType)(eff) );
      _ri_debug << "Segment has " << id << " geom. eff. " << eff << endmsg;
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

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( GeomEffCKMassRing )

//-----------------------------------------------------------------------------

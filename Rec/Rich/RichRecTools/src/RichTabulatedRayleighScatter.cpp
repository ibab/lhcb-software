
//-----------------------------------------------------------------------------
/** @file RichTabulatedRayleighScatter.cpp
 *
 *  Implementation file for tool : Rich::Rec::TabulatedRayleighScatter
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTabulatedRayleighScatter.h"

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TabulatedRayleighScatter )

// Standard constructor
TabulatedRayleighScatter::TabulatedRayleighScatter ( const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
  : Rich::Rec::ToolBase( type, name, parent )
{
  // interface
  declareInterface<IRayleighScatter>(this);
}

double
TabulatedRayleighScatter::photonScatteredProb( const LHCb::RichRecSegment * segment,
                                               const double energy ) const
{
  double prob(0);

  // check this is aerogel
  if ( Rich::Aerogel == segment->trackSegment().radiator() )
  {

    // check energy is valid
    if ( energy > 0 )
    {

      // check path length is valid
      const double path = segment->trackSegment().pathLength();
      if ( path > 0 )
      {

        // get the radiators
        const Rich::RadIntersection::Vector & radInts = segment->trackSegment().radIntersections();

        if ( !radInts.empty() )
        {
          // normalise over each intersection
          double totPlength(0), rayleigh(0);
          for ( const auto& R : radInts )
          {
            const double len = R.pathLength();
            totPlength += len;
            rayleigh   += len * R.radiator()->rayleigh()->value(energy*Gaudi::Units::eV);
          }
          if ( totPlength > 0 ) rayleigh /= totPlength;

          // compute
          prob = ( 1.0 - (rayleigh/path)*(1.0-std::exp(-path/rayleigh)) );
        }

      }
    }
  }

  // return the final probability of rayleigh scattering
  return prob;
}

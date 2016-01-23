
//-----------------------------------------------------------------------------
/** @file RichParticleProperties.h
 *
 *  Header file for tool : Rich::ParticleProperties
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHPARTICLEPROPERTIES_H
#define RICHTOOLS_RICHPARTICLEPROPERTIES_H 1

// STL
#include <vector>

// GSL
#include "gsl/gsl_math.h"

// Rich Kernel
#include "RichKernel/RichTrackSegment.h"

// base class
#include "RichKernel/RichToolBase.h"

// interfaces
#include "RichKernel/IRichParticleProperties.h"
#include "RichKernel/IRichRefractiveIndex.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class ParticleProperties RichParticleProperties.h
   *
   *  Tool to calculate various physical properties
   *  for the different mass hypotheses.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   15/03/2002
   */
  //-----------------------------------------------------------------------------

  class ParticleProperties : public Rich::ToolBase,
                             virtual public IParticleProperties 
  {

  public: // Methods for Gaudi Framework

    /// Standard constructor
    ParticleProperties( const std::string& type,
                        const std::string& name,
                        const IInterface* parent );

    /// Destructor
    virtual ~ParticleProperties() {};

    // Initialize method
    StatusCode initialize();

  public: // methods (and doxygen comments) inherited from public interface

    // Returns 'beta' for given particle hypothesis
    double beta( const double ptot, const Rich::ParticleIDType id ) const;

    // Returns the nominal mass for a given particle type
    double mass( const Rich::ParticleIDType id ) const;

    // Returns the nominal mass squared for a given particle type
    double massSq( const Rich::ParticleIDType id ) const;

    // Returns the threshold momentum for a given hypothesis in a given radiator
    double thresholdMomentum( const Rich::ParticleIDType id,
                              const Rich::RadiatorType rad ) const;

    // Calculates the threshold momentum for a given mass hypothesis
    double thresholdMomentum( const Rich::ParticleIDType id,
                              const LHCb::RichTrackSegment& trSeg ) const;

    // Vector of the mass hypotheses to be considered
    const Rich::Particles & particleTypes() const;

  private:  // Private data

    /// Refractive index tool
    const IRefractiveIndex * m_refIndex = nullptr;

    /// Array containing particle masses
    std::vector<double> m_particleMass;

    /// Array containing square of particle masses
    std::vector<double> m_particleMassSq;

    /// Particle ID types to consider in the likelihood minimisation (JO)
    std::vector<std::string> m_pidTypesJO;
    
    /// Particle ID types to consider in the likelihood minimisation
    Rich::Particles m_pidTypes;

  };

}

#endif // RICHTOOLS_RICHPARTICLEPROPERTIES_H

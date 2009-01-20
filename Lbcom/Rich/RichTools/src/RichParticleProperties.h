
//-----------------------------------------------------------------------------
/** @file RichParticleProperties.h
 *
 *  Header file for tool : Rich::ParticleProperties
 *
 *  CVS Log :-
 *  $Id: RichParticleProperties.h,v 1.7 2009-01-20 16:00:55 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHPARTICLEPROPERTIES_H
#define RICHTOOLS_RICHPARTICLEPROPERTIES_H 1

// GSL
#include "gsl/gsl_math.h"

// Rich Kernel
#include "RichKernel/BoostArray.h"

// base class
#include "RichKernel/RichToolBase.h"

// interfaces
#include "RichKernel/IRichParticleProperties.h"
#include "RichKernel/IRichRefractiveIndex.h"

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

    // Finalize method
    StatusCode finalize();

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

    // Returns the threshold momentum squared for a given hypothesis in a given radiator
    double thresholdMomentumSq( const Rich::ParticleIDType id,
                                const Rich::RadiatorType rad ) const;

    // Vector of the mass hypotheses to be considered
    const Rich::Particles & particleTypes() const;

  private:  // Private data

    /// Array containing particle masses
    boost::array<double,Rich::NParticleTypes> m_particleMass;

    /// Array containing square of particle masses
    boost::array<double,Rich::NParticleTypes> m_particleMassSq;

    /// Momentum thresholds
    double m_momThres[Rich::NRadiatorTypes][Rich::NParticleTypes];

    /// Momentum thresholds squared (cached for speed)
    double m_momThres2[Rich::NRadiatorTypes][Rich::NParticleTypes];

    /// Particle ID types to consider in the likelihood minimisation (JO)
    std::vector<std::string> m_pidTypesJO;
    
    /// Particle ID types to consider in the likelihood minimisation
    Rich::Particles m_pidTypes;

  };

}

#endif // RICHTOOLS_RICHPARTICLEPROPERTIES_H

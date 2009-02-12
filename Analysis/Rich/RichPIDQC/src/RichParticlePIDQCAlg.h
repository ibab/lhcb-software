// $Id: RichParticlePIDQCAlg.h,v 1.1.1.1 2009-02-12 12:58:14 jonrob Exp $
#ifndef RICHPIDQC_RICHPARTICLEPIDQCALG_H 
#define RICHPIDQC_RICHPARTICLEPIDQCALG_H 1

// STL
#include <sstream>

// Gaudi
#include "GaudiKernel/IJobOptionsSvc.h"

// DaVinci
#include "Kernel/DVAlgorithm.h"

// Rich
#include "RichRecBase/IRichPIDPlots.h"
#include "RichKernel/RichHashMap.h"

namespace Rich
{

  /** @class ParticlePIDQCAlg RichParticlePIDQCAlg.h
   *  
   *  Produces standard Rich PID performance plots for a given Particle decay tree
   * 
   *  @author Chris Jones
   *  @date   2009-02-11
   */
  class ParticlePIDQCAlg : public DVAlgorithm 
  {
    
  public: 
    
    /// Standard constructor
    ParticlePIDQCAlg( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~ParticlePIDQCAlg( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  private: // definitions

    typedef std::pair< const LHCb::Particle*, std::string > NamedParticle;
    typedef std::vector< NamedParticle > PartList;

    typedef Rich::HashMap< std::string, const Rich::Rec::IPIDPlots * > PIDToolMap;
    
  private: // methods

    /// Pointer to Job Options Service
    IJobOptionsSvc* joSvc() const;
    
    /// Returns the ParticleProperty object for a given ParticleID
    const LHCb::ParticleProperty * partProp( const LHCb::ParticleID& id ) const;

    /// Create a unique list of stable Particles
    void addParticle( const LHCb::Particle * particle, 
                      PartList & stableParticles,
                      const unsigned int recurCount = 0,
                      const std::string & history   = "" ) const;

    /// Returns the PID tool instance for the given Particle Name
    const Rich::Rec::IPIDPlots * pidTool( const std::string & name ) const;

    // Get the Rich type enum from the Particle properties object
    const Rich::ParticleIDType pidType( const LHCb::ParticleProperty * prop ) const;

  private: // data

    /// Pointer to job options service
    mutable IJobOptionsSvc * m_jos;

    /// Mapping between particle names and PID plot tools
    mutable PIDToolMap m_pidTools;

    /// Fill full set of histograms
    bool m_extraHistos;       

  };

}

#endif // RICHPIDQC_RICHPARTICLEPIDQCALG_H

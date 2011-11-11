#ifndef PARTICLE2BACKGROUNDCATEGORYRELATIONSALG_H 
#define PARTICLE2BACKGROUNDCATEGORYRELATIONSALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IBackgroundCategory;

/** @class Particle2BackgroundCategoryRelationsAlg Particle2BackgroundCategoryRelationsAlg.h
 *  
 *  Algorithm which takes as input a TES container of Particles, gets the BackgroundCategory
 *  for each one, and writes a Relations table between these Particles and their
 *  BackgroundCategories to the TES. The table is Particle*->int. 
 *
 *  InputLocations : The TES locations of the particles for which the table is to be written.
 *                   The table is written to InputLocations-"/Particles"+"/P2BCRelations"
 *
 *  @author V. Gligorov
 *  @date   2009-11-30
 */

class Particle2BackgroundCategoryRelationsAlg : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  Particle2BackgroundCategoryRelationsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Particle2BackgroundCategoryRelationsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  /// Save the background category information for all particles at the given location
  StatusCode backCategoriseParticles(const std::string& location) const; 

  /// Save the background category information for the given particle
  StatusCode backCategoriseParticle(const LHCb::Particle * particle,
                                    LHCb::Relation1D<LHCb::Particle, int>* catRelations) const;
   
private:

  std::vector<std::string> m_particleLocations;

  IBackgroundCategory* m_bkg; 

  bool m_fullTree;

};

#endif // PARTICLE2BACKGROUNDCATEGORYRELATIONSALG_H

#ifndef PARTICLE2BACKGROUNDCATEGORYRELATIONSALG_H 
#define PARTICLE2BACKGROUNDCATEGORYRELATIONSALG_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "Relations/Relations.h"
//LHCb
#include "Event/Particle.h"
// DaVinci
#include "Kernel/IBackgroundCategory.h"
#include "Kernel/DaVinciStringUtils.h"
// boost
#include "boost/algorithm/string.hpp"

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
  Particle2BackgroundCategoryRelationsAlg( const std::string& name,
                                           ISvcLocator* pSvcLocator );

  virtual ~Particle2BackgroundCategoryRelationsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  /// relations type
  typedef LHCb::Relation1D<LHCb::Particle,int> CatRelations;

private:

  /** Returns the full location of the given object in the Data Store
   *
   *  @param pObj Data object
   *
   *  @return Location of given data object
   */
  inline std::string objectLocation( const DataObject * pObj ) const
  {
    return ( !pObj ? "" :
             (pObj->registry() ? pObj->registry()->identifier() : "") );
  }

  /// Save the background category information for all particles at the given location
  StatusCode backCategoriseParticles( const std::string& location ) const; 
  
  /// Save the background category information for the given particle
  StatusCode backCategoriseParticle( const LHCb::Particle * particle,
                                     const unsigned int     recurCount = 0 ) const;

  /// Get the relations object for a given Particle
  CatRelations * catRelations( const LHCb::Particle * particle ) const;
  
private:

  /// Particle locations to process
  std::vector<std::string> m_particleLocations;

  /// Background Category tool
  IBackgroundCategory* m_bkg; 

  /// Flag to turn on processing the full tree
  bool m_fullTree;

  typedef std::map<std::string,CatRelations*> CatMap;
  /// Mapping between particle locations and relations object
  mutable CatMap m_catMap;

};

#endif // PARTICLE2BACKGROUNDCATEGORYRELATIONSALG_H

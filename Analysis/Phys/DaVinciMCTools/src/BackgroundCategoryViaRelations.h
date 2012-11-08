#ifndef BACKGROUNDCATEGORYVIARELATIONS_H 
#define BACKGROUNDCATEGORYVIARELATIONS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IBackgroundCategory.h"          // Interface
#include "Relations/Relations.h"
#include "Event/Particle.h"

// boost
#include "boost/algorithm/string.hpp"

/** @class BackgroundCategoryViaRelations BackgroundCategoryViaRelations.h
 *
 *  A tool to read a relations table of Particle->BackgroundCategory and
 *  return the results. Only implements the category method for now.
 *
 *  inputTable : give the TES location of the relations table. This should be
 *               a relations table Particle*->int, created by the algorithm
 *               Particle2BackgroundCategoryRelationsAlg.
 *
 *  @author Vladimir Gligorov
 *  @date   2009-11-27
 */

class BackgroundCategoryViaRelations : public GaudiTool, 
                                       virtual public IBackgroundCategory 
{

private:

  typedef std::pair<const LHCb::Particle*, const LHCb::MCParticle*> DaughterAndPartnerPair;
  typedef std::vector<DaughterAndPartnerPair> DaughterAndPartnerVector;

public:

  /// Standard constructor
  BackgroundCategoryViaRelations( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent);

public:

  IBackgroundCategory::categories category( const LHCb::Particle* reconstructed_mother ,
                                            const LHCb::Particle* headP = NULL );

public:

  const LHCb::MCParticle* origin( const LHCb::Particle* ) 
  {
    Error ( "The origin method is not applicable to this BackgroundCategory implementation" ).ignore() ;
    return 0;
  }

  const DaughterAndPartnerVector getDaughtersAndPartners( const LHCb::Particle* ) 
  {
    Error("The getDaughtersAndPartners method is not applicable to this BackgroundCategory implementation" ).ignore() ;
    return DaughterAndPartnerVector(0);
  }

  virtual ~BackgroundCategoryViaRelations( ); ///< Destructor

private:

  /// Get the TES location of an object.
  const std::string b2cLocation(const DataObject* pObject) const
  {
    std::string loc = (!pObject ? "Null DataObject" :
                       (pObject->registry() ? pObject->registry()->identifier() : "UnRegistered"));
    // Form the relations TES location for this Particles location
    boost::replace_all( loc, "/Particles", "/P2BCRelations" );
    // return
    return loc;
  }

private:

  typedef LHCb::Relation1D<LHCb::Particle,int> TableP2BC;
  typedef std::vector<TableP2BC*> TablesP2BC;

  std::vector<std::string> m_P2BCLocation;

  TablesP2BC m_catRelations;

};

#endif // BACKGROUNDCATEGORYVIARELATIONS_H

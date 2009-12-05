#ifndef BACKGROUNDCATEGORYVIARELATIONS_H 
#define BACKGROUNDCATEGORYVIARELATIONS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IBackgroundCategory.h"          // Interface
#include "Relations/IRelation.h"

/** @class BackgroundCategoryViaRelations BackgroundCategoryViaRelations.h
 *
 *  A tool to read a relations table of Particle->BackgroundCategory and
 *  return the results. Only implements the category method for now.  
 *
 *  @author Vladimir Gligorov
 *  @date   2009-11-27
 */
typedef std::pair<const LHCb::Particle*, const LHCb::MCParticle*> DaughterAndPartnerPair;
typedef std::vector<DaughterAndPartnerPair> DaughterAndPartnerVector;

class BackgroundCategoryViaRelations : public GaudiTool, virtual public IBackgroundCategory {
public: 
  /// Standard constructor
  BackgroundCategoryViaRelations( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  IBackgroundCategory::categories category(const LHCb::Particle*);

  StatusCode initialize(); 
  StatusCode finalize();

  const LHCb::MCParticle* origin( const LHCb::Particle* ) {Error ( "The origin method is not applicable to this BackgroundCategory implementation" ).ignore() ; return 0;};
  const DaughterAndPartnerVector getDaughtersAndPartners( const LHCb::Particle* ) {Error("The getDaughtersAndPartners method is not applicable to this BackgroundCategory implementation" ).ignore() ; return DaughterAndPartnerVector(0);};

  virtual ~BackgroundCategoryViaRelations( ); ///< Destructor

protected:

private:

  //typedef IRelation <const LHCb::Particle*,IBackgroundCategory::categories> TableP2BC;
  typedef IRelation <const LHCb::Particle*,int> TableP2BC;
  typedef std::vector<TableP2BC*> TablesP2BC;

  std::vector<std::string> m_P2BCLocation;

  TablesP2BC m_catRelations;

};
#endif // BACKGROUNDCATEGORYVIARELATIONS_H

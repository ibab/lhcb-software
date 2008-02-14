#ifndef USER_TAGGINGUTILSCHECKER_H 
#define USER_TAGGINGUTILSCHECKER_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Kernel/IGeomDispCalculator.h"
#include "MCInterfaces/IPrintMCDecayTreeTool.h"
#include "ITaggingUtilsChecker.h"

/** @class TaggingUtilsChecker TaggingUtilsChecker.h 
 *
 *  Utility Tool 
 *
 *  @author Marco Musy
 *  @date   09/06/2007
 */

class TaggingUtilsChecker : public GaudiTool, 
                            virtual public ITaggingUtilsChecker {

public: 
  /// Standard constructor
  TaggingUtilsChecker( const std::string& type,
		const std::string& name,
		const IInterface* parent );
  virtual ~TaggingUtilsChecker( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  StatusCode calcIP( const LHCb::Particle* , 
		     const LHCb::Vertex* , double&, double&);
  StatusCode calcIP( const LHCb::Particle* , 
		     const LHCb::RecVertex* , double&, double&);
  StatusCode calcIP( const LHCb::Particle*,
		     const LHCb::RecVertex::ConstVector& ,
		     double& , double& );
  int countTracks( LHCb::Particle::ConstVector& );
  const LHCb::Particle* motherof( const LHCb::Particle* ,
                                  const LHCb::Particle::ConstVector& );
  bool isinTree(const LHCb::Particle*, LHCb::Particle::ConstVector&, double&);
  LHCb::Particle::ConstVector FindDaughters( const LHCb::Particle* );
  const LHCb::MCParticle* originof( const LHCb::MCParticle*  );
  int comes_from_excitedB(const LHCb::MCParticle* , const LHCb::MCParticle* );
  //-------------------------------------------------------------

private:

  LHCb::MCParticle::ConstVector prodsBstar( const LHCb::MCParticle*  );
  LHCb::MCParticle::ConstVector prodsBstarstar( const LHCb::MCParticle*  );
  LHCb::MCParticle::ConstVector prodsBstring( const LHCb::MCParticle*  );
  HepMC::GenParticle* HEPassociated(const LHCb::MCParticle* );
  LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* );

  IGeomDispCalculator *m_Geom;
  IPrintMCDecayTreeTool* m_debug;

};

//===============================================================//
#endif // USER_TAGGINGUTILSCHECKER_H

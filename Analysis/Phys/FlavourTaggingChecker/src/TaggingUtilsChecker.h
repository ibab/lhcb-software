#ifndef USER_TAGGINGUTILSCHECKER_H 
#define USER_TAGGINGUTILSCHECKER_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "MCInterfaces/IPrintMCDecayTreeTool.h"
#include "ITaggingUtilsChecker.h"
#include <Kernel/IDistanceCalculator.h>
#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>

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
		     const LHCb::VertexBase* , double&, double&);
  StatusCode calcIP( const LHCb::Particle*,
		     const LHCb::RecVertex::ConstVector& ,
		     double& , double& );
  StatusCode calcIPPU( const LHCb::Particle*,
                       const LHCb::Particle*,
                       const LHCb::RecVertex::ConstVector& ,
                       const double ,
                       double& , double&, double&,
                       double& , double&, double&,
                       double& , double&, double&,
                       int& , double&, double&);
  StatusCode calcDOCAmin( const LHCb::Particle* ,
			  const LHCb::Particle* , 
			  const LHCb::Particle* , double& , double& );
  int countTracks( LHCb::Particle::ConstVector& );
  const LHCb::Particle* motherof( const LHCb::Particle* ,
                                  const LHCb::Particle::ConstVector& );
  bool isinTree(const LHCb::Particle*, LHCb::Particle::ConstVector&, double&);
  LHCb::Particle::ConstVector FindDaughters( const LHCb::Particle* );
  const LHCb::MCParticle* originof( const LHCb::MCParticle*  );
  int comes_from_excitedB(const LHCb::MCParticle* , const LHCb::MCParticle* );
 
  //-------------------------------------------------------------

private:
  // TO Fix with MC12
  LHCb::MCParticle::ConstVector prodsBstar( const LHCb::MCParticle*  );
  LHCb::MCParticle::ConstVector prodsBstarstar( const LHCb::MCParticle*  );
  LHCb::MCParticle::ConstVector prodsBstring( const LHCb::MCParticle*  );
  HepMC::GenParticle* HEPassociated(const LHCb::MCParticle* );
  LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* );

  IPrintMCDecayTreeTool* m_debug;
  const IDistanceCalculator *m_Dist;
  IDVAlgorithm* m_dva;

};

//===============================================================//
#endif // USER_TAGGINGUTILSCHECKER_H

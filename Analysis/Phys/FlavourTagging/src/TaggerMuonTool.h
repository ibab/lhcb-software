// $Id: TaggerMuonTool.h,v 1.5 2005-10-05 11:05:45 musy Exp $
#ifndef USER_TAGGERMUONTOOL_H 
#define USER_TAGGERMUONTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"
#include "INNetTool.h"

#include "DaVinciTools/IGeomDispCalculator.h"

#include "Event/ProtoParticle.h"
#include "MuonID/IMuonIDDLLTool.h"
#include "MuonID/IMuonIDNSharedHitsTool.h"

/** @class TaggerMuonTool TaggerMuonTool.h 
 *
 *  Tool to tag the B flavour with a Muon Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerMuonTool : public GaudiTool, 
	virtual public ITagger {

public: 
  /// Standard constructor
  TaggerMuonTool( const std::string& type,
		  const std::string& name,
		  const IInterface* parent );
  virtual ~TaggerMuonTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  virtual Tagger tag( const Particle*, 
		      std::vector<const Vertex*>&, ParticleVector&);
  //-------------------------------------------------------------

private:

  INNetTool* m_nnet;
  IDataProviderSvc* m_eventSvc;
  IGeomDispCalculator *m_Geom;

  /// Tool for mu+/- delta log likelyhood based on Muon System
  IMuonIDDLLTool*  m_muonIDdll;
  /// Tool for number of track ssharing hits in the Muon System
  IMuonIDNSharedHitsTool*  m_muonIDnsh;

  std::string m_CombinationTechnique;
  double pol3(double x, double a0, double a1, double a2);
  StatusCode calcIP( Particle* axp, 
		     const Vertex* RecVert, 
		     double& ip, double& iperr);
  //properties 
  double m_Pt_cut_muon;
  double m_P_cut_muon;
  double m_AverageOmega;
  double m_muonDLL_cut;
  int    m_muonNSH_cut;
};

//===============================================================//
#endif // USER_TAGGERMUONTOOL_H

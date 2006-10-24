// $Id: TaggerMuonTool.h,v 1.7 2006-10-24 10:16:45 jpalac Exp $
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

#include "Kernel/IGeomDispCalculator.h"

//#include "MuonID/IMuonIDDLLTool.h"
//#include "MuonID/IMuonIDNSharedHitsTool.h"

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
  virtual LHCb::Tagger tag( const LHCb::Particle*, const LHCb::RecVertex*,
			    std::vector<const LHCb::Vertex*>&, LHCb::
			    Particle::ConstVector&);
  //-------------------------------------------------------------

private:

  INNetTool* m_nnet;
  IDataProviderSvc* m_eventSvc;
  IGeomDispCalculator *m_Geom;

  /// Tool for mu+/- delta log likelyhood based on Muon System
  //IMuonIDDLLTool*  m_muonIDdll;
  /// Tool for number of track ssharing hits in the Muon System
  //IMuonIDNSharedHitsTool*  m_muonIDnsh;

  std::string m_CombinationTechnique, m_NeuralNetName;
  StatusCode calcIP( const LHCb::Particle* axp, 
		     const LHCb::Vertex* RecVert, 
		     double& ip, double& iperr);
  StatusCode calcIP( const LHCb::Particle* axp, 
		     const LHCb::RecVertex* RecVert, 
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

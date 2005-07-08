// $Id: TaggerMuonTool.h,v 1.4 2005-07-08 11:32:06 pkoppenb Exp $
#ifndef USER_TAGGERMUONTOOL_H 
#define USER_TAGGERMUONTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "DaVinciTools/IGeomDispCalculator.h"
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"
#include "INNetTool.h"

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
  IGeomDispCalculator *m_Geom;
  INNetTool* m_nnet;
  std::string m_CombinationTechnique;
  double pol3(double x, double a0, double a1, double a2);
  StatusCode calcIP( Particle* axp, 
		     const Vertex* RecVert, 
		     double& ip, double& iperr);
  //properties 
  double m_Pt_cut_muon;
  double m_P_cut_muon;
  double m_AverageOmega;
};

//===============================================================//
#endif // USER_TAGGERMUONTOOL_H

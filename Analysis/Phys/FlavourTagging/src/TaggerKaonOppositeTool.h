// $Id: TaggerKaonOppositeTool.h,v 1.4 2005-07-08 11:32:06 pkoppenb Exp $
#ifndef USER_TAGGERKAONOPPOSITETOOL_H 
#define USER_TAGGERKAONOPPOSITETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/ProtoParticle.h"
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"
#include "Kernel/IGeomDispCalculator.h"
#include "INNetTool.h"

/** @class TaggerKaonOppositeTool TaggerKaonOppositeTool.h 
 *
 *  Tool to tag the B flavour with a KaonOpposite Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerKaonOppositeTool : public GaudiTool, 
	virtual public ITagger {

public: 
  /// Standard constructor
  TaggerKaonOppositeTool( const std::string& type,
			  const std::string& name,
			  const IInterface* parent );
  virtual ~TaggerKaonOppositeTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  virtual Tagger tag( const Particle*, 
		      std::vector<const Vertex*>&, ParticleVector&);
  //-------------------------------------------------------------

private:
  void calcIP( const Particle* , const Vertex* , double& , double& );
  double pol2(double x, double a0, double a1);

  IGeomDispCalculator *m_Geom;
  INNetTool* m_nnet;
  std::string m_CombinationTechnique;

  //properties 
  double m_Pt_cut_kaon;
  double m_P_cut_kaon ;
  double m_IP_cut_kaon ;
  double m_lcs_kf, m_lcs_km, m_lcs_ku, m_IP_kf;
  double m_IP_km, m_IP_ku;
  double m_AverageOmega;

};

//===============================================================//
#endif // USER_TAGGERKAONOPPOSITETOOL_H

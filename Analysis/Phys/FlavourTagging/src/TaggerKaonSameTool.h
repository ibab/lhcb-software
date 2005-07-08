// $Id: TaggerKaonSameTool.h,v 1.4 2005-07-08 11:32:06 pkoppenb Exp $
#ifndef USER_TAGGERKAONSAMETOOL_H 
#define USER_TAGGERKAONSAMETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Kernel/IGeomDispCalculator.h"
#include "Event/FlavourTag.h"
#include "Event/ProtoParticle.h"
#include "Kernel/ITagger.h"
#include "INNetTool.h"

/** @class TaggerKaonSameTool TaggerKaonSameTool.h 
 *
 *  Tool to tag the B flavour with a KaonSame Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerKaonSameTool : public GaudiTool, 
	virtual public ITagger {

public: 
  /// Standard constructor
  TaggerKaonSameTool( const std::string& type,
		      const std::string& name,
		      const IInterface* parent );
  virtual ~TaggerKaonSameTool( ); ///< Destructor
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
  double m_Pt_cut_kaonS;
  double m_P_cut_kaonS;
  double m_IP_cut_kaonS;
  double m_phicut_kaonS;
  double m_etacut_kaonS;
  double m_dQcut_kaonS;
  double m_lcs_kSu;
  double m_AverageOmega;
};

//===============================================================//
#endif // USER_TAGGERKAONSAMETOOL_H

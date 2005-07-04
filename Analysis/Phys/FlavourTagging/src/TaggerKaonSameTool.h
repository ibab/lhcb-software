// $Id: TaggerKaonSameTool.h,v 1.2 2005-07-04 15:40:09 pkoppenb Exp $
#ifndef USER_TAGGERKAONSAMETOOL_H 
#define USER_TAGGERKAONSAMETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/ProtoParticle.h"
#include "Kernel/ITagger.h"
#include "Kernel/IGeomDispCalculator.h"

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
  ParticleVector taggers( const Particle*, const Vertex*, 
				  const ParticleVector& );
  //-------------------------------------------------------------

private:
  void calcIP( const Particle* , const Vertex* , double& , double& );

  IGeomDispCalculator *m_Geom;

  //properties 
  double m_Pt_cut_kaonS;
  double m_P_cut_kaonS;
  double m_IP_cut_kaonS;
  double m_phicut_kaonS;
  double m_etacut_kaonS;
  double m_dQcut_kaonS;
  double m_lcs_kSu;

};

//===============================================================//
#endif // USER_TAGGERKAONSAMETOOL_H

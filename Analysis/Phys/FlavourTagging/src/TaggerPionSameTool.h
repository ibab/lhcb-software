// $Id: TaggerPionSameTool.h,v 1.1 2005-07-04 08:20:06 pkoppenb Exp $
#ifndef USER_TAGGERKAONSAMETOOL_H 
#define USER_TAGGERKAONSAMETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/ProtoParticle.h"
#include "FlavourTagging/ITagger.h"
#include "DaVinciTools/IGeomDispCalculator.h"

/** @class TaggerPionSameTool TaggerPionSameTool.h 
 *
 *  Tool to tag the B flavour with a PionSame Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerPionSameTool : public GaudiTool, 
	virtual public ITagger {

public: 
  /// Standard constructor
  TaggerPionSameTool( const std::string& type,
		      const std::string& name,
		      const IInterface* parent );
  virtual ~TaggerPionSameTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  virtual ParticleVector taggers( Particle*, Vertex*, 
				  std::vector<Particle*> );
  //-------------------------------------------------------------

private:
  void calcIP( Particle* , Vertex* , double& , double& );

  IGeomDispCalculator *m_Geom;

  //properties 
  double m_Pt_cut_pionS;
  double m_P_cut_pionS;
  double m_IP_cut_pionS;
  double m_dQcut_pionS;
  double m_lcs_pSu;

};
//===============================================================//
#endif // USER_TAGGERKAONSAMETOOL_H

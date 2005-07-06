// $Id: TaggerPionSameTool.h,v 1.3 2005-07-06 00:33:20 musy Exp $
#ifndef USER_TAGGERPIONSAMETOOL_H 
#define USER_TAGGERPIONSAMETOOL_H 1

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
  virtual Tagger tag( const Particle*, 
		      std::vector<const Vertex*>&, ParticleVector&);
  //-------------------------------------------------------------

private:
  void calcIP( const Particle* , const Vertex* , double& , double& );
  double pol2(double x, double a0, double a1);

  double m_ProbMin;
  IGeomDispCalculator *m_Geom;
  INNetTool* m_nnet;
  std::string m_CombinationTechnique;

  //properties 
  double m_Pt_cut_pionS;
  double m_P_cut_pionS;
  double m_IP_cut_pionS;
  double m_dQcut_pionS;
  double m_lcs_pSu;
};
//===============================================================//
#endif // USER_TAGGERPIONSAMETOOL_H

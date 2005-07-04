// $Id: TaggerJetSameTool.h,v 1.1 2005-07-04 16:51:53 pkoppenb Exp $
#ifndef USER_TAGGERJETSAMETOOL_H 
#define USER_TAGGERJETSAMETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Kernel/ITagger.h"

/** @class TaggerJetSameTool TaggerJetSameTool.h 
 *
 *  Tool to tag the B flavour with a JetSame Tagger
 *
 *  @author Julien Babel
 *  @date   30/06/2005
 */

class TaggerJetSameTool : public GaudiTool, 
	virtual public ITagger {

public: 
  /// Standard constructor
  TaggerJetSameTool( const std::string& type,
		     const std::string& name,
		     const IInterface* parent );
  virtual ~TaggerJetSameTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  virtual ParticleVector taggers( const Particle*, const Vertex*, 
				  const ParticleVector& );
  //-------------------------------------------------------------

private:
  //properties 
  double  m_Pt_cut_jetS, m_dR_cut_jetS;
};
//===============================================================//
#endif // USER_TAGGERJETSAMETOOL_H

// $Id: TaggerMuonTool.h,v 1.1 2005-07-04 08:20:06 pkoppenb Exp $
#ifndef USER_TAGGERMUONTOOL_H 
#define USER_TAGGERMUONTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "FlavourTagging/ITagger.h"

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
  virtual ParticleVector taggers( Particle*, Vertex*, 
				  std::vector<Particle*> );
  //-------------------------------------------------------------

private:
  //properties 
  double m_Pt_cut_muon;
  double m_P_cut_muon;
};

//===============================================================//
#endif // USER_TAGGERMUONTOOL_H

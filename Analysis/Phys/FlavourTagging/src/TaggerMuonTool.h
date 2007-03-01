// $Id: TaggerMuonTool.h,v 1.8 2007-03-01 20:59:23 musy Exp $
#ifndef USER_TAGGERMUONTOOL_H 
#define USER_TAGGERMUONTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"

// from local
#include "ITaggingUtils.h"
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
  virtual LHCb::Tagger tag( const LHCb::Particle*, const LHCb::RecVertex*,
			    std::vector<const LHCb::Vertex*>&, LHCb::
			    Particle::ConstVector&);
  //-------------------------------------------------------------

private:

  INNetTool* m_nnet;
  ITaggingUtils* m_util;

  IDataProviderSvc* m_eventSvc;
  std::string m_CombinationTechnique, m_NeuralNetName;

  //properties 
  double m_Pt_cut_muon;
  double m_P_cut_muon;
  double m_lcs_cut_muon;
  double m_AverageOmega;
};

//===============================================================//
#endif // USER_TAGGERMUONTOOL_H

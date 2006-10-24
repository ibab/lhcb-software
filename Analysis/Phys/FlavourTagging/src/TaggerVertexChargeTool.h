// $Id: TaggerVertexChargeTool.h,v 1.3 2006-10-24 10:16:45 jpalac Exp $
#ifndef USER_TAGGERVERTEXCHARGETOOL_H 
#define USER_TAGGERVERTEXCHARGETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"

/** @class TaggerVertexChargeTool TaggerVertexChargeTool.h 
 *
 *  Tool to tag the B flavour with a VertexCharge Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerVertexChargeTool : public GaudiTool, 
	virtual public ITagger {

public: 
  /// Standard constructor
  TaggerVertexChargeTool( const std::string& type,
			  const std::string& name,
			  const IInterface* parent );
  virtual ~TaggerVertexChargeTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  virtual LHCb::Tagger tag( const LHCb::Particle*, const LHCb::RecVertex*,
			    std::vector<const LHCb::Vertex*>&, 
			    LHCb::Particle::ConstVector&);
  //-------------------------------------------------------------

private:

  std::string m_CombinationTechnique;
  double m_AverageOmega, m_PowerK, m_MinimumCharge, m_P0, m_P1, m_P2;
};

//===============================================================//
#endif // USER_TAGGERVERTEXCHARGETOOL_H

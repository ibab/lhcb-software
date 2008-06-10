// $Id: TaggerVertexChargeTool.h,v 1.6 2008-06-10 19:01:00 musy Exp $
#ifndef USER_TAGGERVERTEXCHARGETOOL_H 
#define USER_TAGGERVERTEXCHARGETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"
#include "Kernel/ISecondaryVertexTool.h"

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

  ISecondaryVertexTool* m_svtool;
  std::string m_SecondaryVertexToolName;

  std::string m_CombinationTechnique;
  double m_AverageOmega, m_PowerK, m_MinimumCharge, m_P0, m_P1, m_Gt075;
  double m_wSameSign2, m_wSameSignMoreThan2;
  double m_ProbMin;

};

//===============================================================//
#endif // USER_TAGGERVERTEXCHARGETOOL_H

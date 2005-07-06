// $Id: TaggerVertexChargeTool.h,v 1.1 2005-07-06 00:36:03 musy Exp $
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
  virtual Tagger tag( const Particle*, 
		      std::vector<const Vertex*>&, ParticleVector&);
  //-------------------------------------------------------------

private:
  std::vector<const Particle*> toStdVector( const SmartRefVector<Particle>& refvector );
  double m_AverageOmega;
};

//===============================================================//
#endif // USER_TAGGERVERTEXCHARGETOOL_H

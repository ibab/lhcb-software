// $Id: SVertexTool.h,v 1.5 2007-03-01 20:59:23 musy Exp $
#ifndef SVERTEXTOOL_H 
#define SVERTEXTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "Event/ProtoParticle.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/ISecondaryVertexTool.h" // Interface

/** @class SVertexTool SVertexTool.h SVertexTool.h
 *  
 *  v1.3 This tool takes as input a primary vertex and a particle vector
 *       and returns a secondary vertex (based on a likelihood function)
 *  @author Marco Musy
 *  @date   2005-06-30
 */

class SVertexTool : public GaudiTool, virtual public ISecondaryVertexTool {

public:
  /// Standard constructor
  SVertexTool( const std::string& type, 
               const std::string& name,
               const IInterface* parent );

  virtual ~SVertexTool( ); ///< Destructor

  /// Initialize
  StatusCode initialize();
  /// Finalize
  StatusCode finalize();
  
  //----------------------------------------------------------------
  std::vector<LHCb::Vertex> buildVertex ( const LHCb::RecVertex&, 
					  const LHCb::Particle::ConstVector& );
  //----------------------------------------------------------------

private:
  IGeomDispCalculator* geom;
  IVertexFit *fitter;
  double m_lcs_cut;

  bool isin(LHCb::Particle::ConstVector& , const LHCb::Particle* );

  double angle( Gaudi::LorentzVector , Gaudi::LorentzVector );
  double ipprob(double x) ;
  double ptprob(double x);
  double aprob(double x);
};
#endif // SVERTEXTOOL_H

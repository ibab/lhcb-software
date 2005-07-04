// $Id: SVertexTool.h,v 1.1 2005-07-04 08:20:06 pkoppenb Exp $
#ifndef SVERTEXTOOL_H 
#define SVERTEXTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "Event/ProtoParticle.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IVertexFitter.h"
#include "FlavourTagging/ISecondaryVertexTool.h" // Interface

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
  virtual std::vector<Vertex>
    buildVertex ( const Vertex, const ParticleVector ); 
  //----------------------------------------------------------------

private:
  IGeomDispCalculator* geom;
  IVertexFitter *fitter;

  double ipprob(double x) ;
  double ptprob(double x);
  double aprob(double x);
};
#endif // SVERTEXTOOL_H

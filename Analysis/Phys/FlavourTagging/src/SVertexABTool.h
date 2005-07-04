// $Id: SVertexABTool.h,v 1.1 2005-07-04 08:20:05 pkoppenb Exp $
#ifndef SVERTEXABTOOL_H 
#define SVERTEXABTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "Event/ProtoParticle.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IVertexFitter.h"
#include "FlavourTagging/ISecondaryVertexTool.h"   // Interface

/** @class SVertexABTool SVertexABTool.h SVertexABTool.h
 *  
 *  DO NOT USE YET: only here for development purpose! (still crashing)
 *  v1.3 This tool takes as input a primary vertex and a particle vector
 *       and returns two secondary vertices
 *  @author Marco Musy
 *  @date   2004-12-14
 */

class SVertexABTool : public GaudiTool, virtual public ISecondaryVertexTool {

public:

  /// Standard constructor
  SVertexABTool( const std::string& type, 
		 const std::string& name,
		 const IInterface* parent );
  virtual ~SVertexABTool( ); ///< Destructor
  /// Initialize
  StatusCode initialize();
  /// Finalize
  StatusCode finalize();
  
  //-------------------------------------------------------------
  virtual std::vector<Vertex>
    buildVertex( const Vertex, const ParticleVector ); 
  //-------------------------------------------------------------

private:
  IGeomDispCalculator* geom;
  IVertexFitter *fitter;
  ParticleVector toStdVector(SmartRefVector<Particle>& );

  bool m_SkipTrackPurge;
  bool isinVtx( Vertex, Particle* );
  double ipprob(double x) ;
  double ptprob(double x);
  double aprob(double x);
};
#endif // SVERTEXNNTOOL_H

// $Id: SecondaryVertexTool.h,v 1.4 2005-05-25 18:33:31 musy Exp $
#ifndef SECONDARYVERTEXTOOL_SECONDARYVERTEXTOOL_H 
#define SECONDARYVERTEXTOOL_SECONDARYVERTEXTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "FlavourTagging/ISecondaryVertexTool.h"            // Interface

#include "Event/ProtoParticle.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IVertexFitter.h"

/** @class SecondaryVertexTool SecondaryVertexTool.h SecondaryVertexTool.h
 *  
 *  v1.3 This tool takes as input a primary vertex and a particle vector
 *       and returns a secondary vertex
 *  @author Marco Musy
 *  @date   2004-12-14
 */
class SecondaryVertexTool : public GaudiTool, 
	virtual public ISecondaryVertexTool {
public:
  /// Standard constructor
  SecondaryVertexTool( const std::string& type, 
		       const std::string& name,
		       const IInterface* parent );

  virtual ~SecondaryVertexTool( ); ///< Destructor

  /// Initialize
  StatusCode initialize();
  /// Finalize
  StatusCode finalize();
  
  //-------------------------------------------------------------
  virtual Vertex SVertex  ( const Vertex, const ParticleVector ); 
  virtual Vertex SVertexNN( const Vertex, const ParticleVector ); 
  //-------------------------------------------------------------

private:
  IGeomDispCalculator* geom;
  IVertexFitter *fitter;

  double NNseeding(double P1,
		   double P2,
		   double PT1,
		   double PT2,
		   double B0PT,
		   double IP1,
		   double IP2,
		   double JVZ,
		   double JCHI );
 
  double SIGMOID(double x);
  double ipprob(double x) ;
  double ptprob(double x);
  double aprob(double x);

};
#endif // SECONDARYVERTEXTOOL_SECONDARYVERTEXTOOL_H

// $Id: SVertexABTool.h,v 1.4 2006-10-24 10:16:44 jpalac Exp $
#ifndef SVERTEXABTOOL_H 
#define SVERTEXABTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"

#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IVertexFit.h"

#include "Kernel/ISecondaryVertexTool.h"   // Interface

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
  std::vector<LHCb::Vertex> buildVertex( const LHCb::RecVertex&, 
					 const LHCb::Particle::ConstVector& ); 
  //-------------------------------------------------------------

private:
  IGeomDispCalculator* geom;
  IVertexFit *fitter;

  bool m_SkipTrackPurge;
  double m_cut_chiA, m_cut_chiB, m_cut_zA, m_cut_zB;

  bool isinVtx( const LHCb::Vertex, const LHCb::Particle* );
  double ipprob(double x) ;
  double ptprob(double x);
  double aprob(double x);
};
#endif // SVERTEXNNTOOL_H

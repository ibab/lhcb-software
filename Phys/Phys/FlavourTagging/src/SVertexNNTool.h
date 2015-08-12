// $Id: SVertexNNTool.h,v 1.4 2010-01-18 22:17:04 musy Exp $
#ifndef SVERTEXNNTOOL_H
#define SVERTEXNNTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "FlavourTagging/ITaggingUtils.h"

#include "Kernel/IVertexFit.h"
#include "Kernel/ISecondaryVertexTool.h"   // Interface

/** @class SVertexNNTool SVertexNNTool.h SVertexNNTool.h
 *
 *  v1.3 This tool takes as input a primary vertex and a particle vector
 *       and returns a secondary vertex
 *  @author Marco Musy
 *  @date   2004-12-14
 */

class SVertexNNTool : public GaudiTool, virtual public ISecondaryVertexTool {

public:

  /// Standard constructor
  SVertexNNTool( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );
  virtual ~SVertexNNTool( ); ///< Destructor
  /// Initialize
  StatusCode initialize();

  //-------------------------------------------------------------
  std::vector<LHCb::Vertex> buildVertex( const LHCb::RecVertex&,
                                         const LHCb::Particle::ConstVector& );
  //-------------------------------------------------------------

private:
  ITaggingUtils* m_util;
  const IVertexFit *fitter;

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
};
#endif // SVERTEXNNTOOL_H

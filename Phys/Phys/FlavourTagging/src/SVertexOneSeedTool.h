// $Id: SVertexOneSeedTool.h,v 1.1 2010-06-17 17:46:03 mgrabalo Exp $
#ifndef SVERTEXONESEEDTOOL_H 
#define SVERTEXONESEEDTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "Event/ProtoParticle.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/ISecondaryVertexTool.h" // Interface
#include "FlavourTagging/ITaggingUtils.h"
#include <string>

#include "MultiplePersonalityCall.h"

/** @class SVertexOneSeedTool SVertexOneSeedTool.h SVertexOneSeedTool.h
 *  
 *  v1.3 This tool takes as input a primary vertex and a particle vector
 *       and returns a secondary vertex (based on a likelihood function)
 *  @author Marco Musy
 *  @date   2005-06-30
 */

class SVertexOneSeedTool : public GaudiTool, virtual public ISecondaryVertexTool {

public:
  /// Standard constructor
  SVertexOneSeedTool( const std::string& type, 
               const std::string& name,
               const IInterface* parent );

  virtual ~SVertexOneSeedTool( ); ///< Destructor

  /// Initialize
  StatusCode initialize();
  
  //----------------------------------------------------------------
  MultiplePersonalityCall<boost::function< 
  std::vector<LHCb::Vertex>(
                            const LHCb::RecVertex&,
                            const LHCb::Particle::ConstVector& ) > >
  m_buildVertex;

  std::vector<LHCb::Vertex> buildVertex( const LHCb::RecVertex&,
                                         const LHCb::Particle::ConstVector& );
  std::vector<LHCb::Vertex> buildVertexReco12( const LHCb::RecVertex&,
                                         const LHCb::Particle::ConstVector& );
  std::vector<LHCb::Vertex> buildVertexReco14( const LHCb::RecVertex&,
                                         const LHCb::Particle::ConstVector& );
  //----------------------------------------------------------------

private:

  double pol(double x, double a0, double a1=0, 
             double a2=0, double a3=0, double a4=0);
  double combine(double p1, double p2, double p3=0.5,
                 double p4=0.5, double p5=0.5, double p6=0.5, double p7=0.5);

  ITaggingUtils* m_util;

  const IVertexFit *fitter;
  double m_lcs_Long_cut;
  double m_lcs_Upstream_cut;
  double m_lcs_vtxaddedtracks_cut;
  double m_maxprobf, m_ptmin, m_ipsmin, m_dphimin;
  double m_ghostProb_vtx;  
  bool m_noclones;
  std::string m_personality;
};
#endif // SVERTEXTOOL_H

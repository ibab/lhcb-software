// $Id: SVertexTool.h,v 1.8 2010-01-18 22:17:04 musy Exp $
#ifndef SVERTEXTOOL_H
#define SVERTEXTOOL_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "Event/ProtoParticle.h"
#include "Kernel/IVertexFit.h"
#include "Kernel/ISecondaryVertexTool.h" // Interface
#include "FlavourTagging/ITaggingUtils.h"

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

  //----------------------------------------------------------------
  std::vector<LHCb::Vertex> buildVertex ( const LHCb::RecVertex&, 
                                          const LHCb::Particle::ConstVector& );
  //----------------------------------------------------------------

private:

  ITaggingUtils* m_util;

  const IVertexFit *fitter;
  double m_lcs_Long_cut;
  double m_lcs_Upstream_cut;

  float  m_ipfitpol0;
  float  m_ipfitpol1;
  float  m_ipfitpol2;
  float  m_ipfitpol3;
  float  m_ipfitpol4;
  float  m_ipfitpol5;

  float  m_ptfitpol0;
  float  m_ptfitpol1;
  float  m_ptfitpol2;
  float  m_ptfitpol3;
  float  m_ptfitpol4;

  float  m_anglepol0;
  float  m_anglepol1;

  bool isin(LHCb::Particle::ConstVector& , const LHCb::Particle* );

  double angle( const Gaudi::LorentzVector& , const Gaudi::LorentzVector& );
  double ipprob(double x) ;
  double ptprob(double x);
  double aprob(double x);

};
#endif // SVERTEXTOOL_H

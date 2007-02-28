// $Id: SVertexTool.h,v 1.4 2007-02-28 08:27:58 sposs Exp $
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
  float  m_anglepol2;  

  double angle( Gaudi::LorentzVector , Gaudi::LorentzVector );
  double ipprob(double x) ;
  double ptprob(double x);
  double aprob(double x);
};
#endif // SVERTEXTOOL_H

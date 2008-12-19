// $Id: TupleToolGeometry.h,v 1.6 2008-12-19 13:12:08 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLGEOMETRY_H
#define JBOREL_TUPLETOOLGEOMETRY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

class IContextTool;
class IDistanceCalculator;

namespace LHCb {
  class Particle;
  class VertexBase;
};


/** @class TupleToolGeometry TupleToolGeometry.h jborel/TupleToolGeometry.h
 *
 * \brief Fill geometry related information for DecayTreeTuple
 *
 * - head_MINIP : minimum impact parameter on any PV
 * - head_MINIPCHI2 : minimum chi2 IP on all PVs
 * - head_ENDVERTEX_[X|Y|Z] : decay vertex position for composite particles
 * - head_ENDVERTEX_[X|Y|Z]ERR : decay vertex position error estimate for composite particles
 * - head_ENDVERTEX_CHI2 : decay vertex chi2
 * - head_ENDVERTEX_NDOF : decay vertex nDoF
 * - head_IP : impact parameter with respect to the PhysDesktop::relatedVertex() of the top of decay chain
 * - head_IPCHI2 : impact parameter chi2 with respect to the PhysDesktop::relatedVertex() of the top of decay chain
 * - head_FD : flight distance of composite particle wrt. the PhysDesktop::relatedVertex() of the top of decay chain
 * - head_FDCHI2 : flight distance significance in units of chi2 wrt. the PhysDesktop::relatedVertex() of the top of decay chain
 * - head_DIRA : direction angle wrt. the PhysDesktop::relatedVertex() of the top of decay chain
 * - head_IP_OWNPV : impact parameter with respect to the PhysDesktop::relatedVertex() considered particle 
 *  (not vailable for the top of decay chain)
 * - head_IPCHI2_OWNPV : impact parameter chi2 with respect to the PhysDesktop::relatedVertex() considered particle 
 * (not vailable for the top of decay chain)
 * - head_FD_OWNPV : flight distance of composite particle wrt. the PhysDesktop::relatedVertex() considered particle 
 * (not vailable for the top of decay chain)
 * - head_FDCHI2_OWNPV : flight distance significance in units of chi2 wrt. the PhysDesktop::relatedVertex() considered particle 
 * (not vailable for the top of decay chain)
 * - head_DIRA_OWNPV : direction angle wrt. the PhysDesktop::relatedVertex() considered particle 
 * (not vailable for the top of decay chain)
 * - head_IP_ORIVX : impact parameter with respect to the ancestor's vertex (when applicable)
 * - head_IPCHI2_ORIVX : impact parameter chi2 with respect to the ancestor's vertex (when applicable)
 * - head_FD_ORIVX : flight distance of composite particle wrt. the ancestor's vertex (when applicable)
 * - head_FDCHI2_ORIVX : flight distance significance in units of chi2 wrt. ancestor's vertex (when applicable)
 * - head_DIRA_ORIVX : direction angle wrt. ancestor's vertex (when applicable)
 *
 * \sa DecayTreeTuple
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
 */
class TupleToolGeometry : public GaudiTool, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolGeometry( const std::string& type,
		     const std::string& name,
		     const IInterface* parent);
  
  virtual ~TupleToolGeometry( ){}; ///< Destructor
  
  virtual StatusCode initialize();
  
  StatusCode fill( const LHCb::Particle*
		   , const LHCb::Particle*
		   , const std::string&
		   , Tuples::Tuple& );

private:

  /// origin vertex
  const LHCb::VertexBase* originVertex( const  LHCb::Particle*
                                        , const LHCb::Particle* ) const;
  /// fill related pV stuff
  StatusCode fillBPV(const LHCb::VertexBase*, const LHCb::Particle*, std::string, Tuples::Tuple&, std::string trail = "") const ;

  /// fill min IP
  StatusCode fillMinIP(const LHCb::Particle*, std::string, Tuples::Tuple&) const ;

  /// fill end vertex stuff
  StatusCode fillEndVertex(const LHCb::Particle*, std::string, Tuples::Tuple&) const ;

  /// fill origin vertex stuff
  StatusCode fillOriginVertex(const LHCb::Particle*, std::string, Tuples::Tuple&) const ;

  /// fill flight
  StatusCode fillFlight(const LHCb::VertexBase* oriVtx, const LHCb::Particle*, 
                        std::string, Tuples::Tuple&, std::string trail = "") const ;

  IContextTool* m_context;
  const IDistanceCalculator* m_dist;
  
  int m_photonID; 
  int m_pi0ID;

};
#endif // JBOREL_TUPLETOOLGEOMETRY_H

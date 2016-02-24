#ifndef WSUTCLIFFE_TupleToolSLTools_H
#define WSUTCLIFFE_TupleToolSLTools_H 

// Include files
// from Gaudi
#include "DecayTreeTupleBase/TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"            // Interface
#include "TrackInterfaces/ITrackVertexer.h"
#include "TLorentzVector.h"
#include "TVector3.h"
#include "ITaggingUtilsChecker.h"
#include "Kernel/IParticleDescendants.h"


class IDVAlgorithm;
class IDistanceCalculator;
class IBTaggingTool;
class IVertexFit;
class IPVReFitter;


namespace LHCb {
  class Particle;
  class Vertex;
}

/** @class TupleToolSLTools TupleToolSLTools.h
 *
 * \brief Fill isolation information for DecayTreeTuple
 *
 * - head_NOPARTWITHINDCHI2WDW : no. of non-signal particles that when added to vertex give delta chi2 < specified window
 * - head_NOPARTWITHINCHI2WDW : no. of non-signal particles that when added to vertex give chi2 < specified window
 * head_SMALLESTCHI2: chi2 of smallest chi2 combination with any of the input Particles
 * head_SMALLESTDELTACHI2: delta chi2 of smallest delta chi2 combination with any of the input Particles
 *
 * \sa DecayTreeTuple
 *
 *  @todo Maybe one should get Tracks instead of Particles?
 *
 *  @author Mitesh Patel, Patrick Koppenburg
 *  @date   2008-04-15
 */
class TupleToolSLTools : public TupleToolBase,
                         virtual public IParticleTupleTool
{
  
public:
  
  /// Standard constructor
  TupleToolSLTools( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  
  virtual ~TupleToolSLTools( ){}; ///< Destructor
  
  virtual StatusCode initialize();
  
  StatusCode fill( const LHCb::Particle*
                   , const LHCb::Particle*
                   , const std::string&
                   , Tuples::Tuple& );

private:

  double Mcorr(TLorentzVector , TVector3);

  std::vector<double> Mcorr_errors(TVector3 v1, TVector3 v2, TLorentzVector p,  Gaudi::SymMatrix7x7 cov1, Gaudi::SymMatrix3x3 cov2 );
  const LHCb::Particle* findID(unsigned int id, LHCb::Particle::ConstVector& v, std::string s="");
  
  std::vector<TLorentzVector> recoNu(TLorentzVector Y, TVector3 M_dirn, double mass);
  StatusCode fillVertex( const LHCb::VertexBase* vtx,
                         const std::string& vtx_name,
                         Tuples::Tuple& tuple ) const;
  
  IDVAlgorithm* m_dva;
  const IDistanceCalculator* m_dist;
  IParticleDescendants* m_descend;
  ITaggingUtilsChecker* m_util;
  const IVertexFit* m_pVertexFit;
  IPVReFitter* m_pvReFitter;
  std::string m_typeVertexFit;
  double m_Bmass;
  bool m_vcov;
  bool m_momcov;

};

#endif // MPATEL_TUPLETOOLGEOMETRY_H

#ifndef MPATEL_TUPLETOOLVTXISOLN_H
#define MPATEL_TUPLETOOLVTXISOLN_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface
#include "Kernel/IVertexFit.h"
#include "Kernel/IGeomDispCalculator.h"
class DVAlgorithm;
class IDistanceCalculator;

namespace LHCb {
  class Particle;
  class Vertex;
};


/** @class TupleToolVtxIsoln TupleToolVtxIsoln.h 
 *
 * \brief Fill isolation information for DecayTreeTuple
 *
 * - head_NOPARTWITHINIPWDW : no. of non-signal particles within given ip window 
 * - head_NOPARTWITHINIPSWDW :  no. of non-signal particles within given ips window 
 * - head_NOPARTWITHINDCHI2WDW : no. of non-signal particles that when added to vertex give delta chi2 < specified window 
 * - head_NOPARTWITHINCHI2WDW : no. of non-signal particles that when added to vertex give chi2 < specified window 
 * 
 * \sa DecayTreeTuple
 *
 *  @todo Maybe one should get Tracks instead of Particles?
 *
 *  @author Mitesh Patel
 *  @date   2008-04-15
 */
class TupleToolVtxIsoln : public GaudiTool, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolVtxIsoln( const std::string& type,
		     const std::string& name,
		     const IInterface* parent);
  
  virtual ~TupleToolVtxIsoln( ){}; ///< Destructor
  
  virtual StatusCode initialize();
  
  StatusCode fill( const LHCb::Particle*
		   , const LHCb::Particle*
		   , const std::string&
		   , Tuples::Tuple& );

private:

  const LHCb::Vertex* originVertex( const  LHCb::Particle*
				    , const LHCb::Particle* ) const;
  

  DVAlgorithm* m_dva;
  const IDistanceCalculator* m_dist;
  const IVertexFit* m_pVertexFit;
  //const IGeomDispCalculator* m_ipTool;
  double m_IP; 
  //  double m_IPS; 
  double m_deltaChi2; 
  double m_Chi2; 
  std::string m_typeVertexFit; 
  std::vector<std::string> m_inputParticles;
};
#endif // MPATEL_TUPLETOOLGEOMETRY_H

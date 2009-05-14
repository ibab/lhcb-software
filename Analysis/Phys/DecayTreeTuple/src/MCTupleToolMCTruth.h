// $Id: MCTupleToolMCTruth.h,v 1.4 2009-05-14 12:52:39 pkoppenb Exp $
#ifndef MCTUPLETOOLMCTRUTH_H 
#define MCTUPLETOOLMCTRUTH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IMCParticleTupleTool.h"            // Interface
#include "Event/MCParticle.h"            // Interface
/** @class MCTupleToolMCTruth MCTupleToolMCTruth.h
 *  
 *  Fill MC Particle 
 *
 * See the properties inline documentation to toggle on/off some of
 * the columns
 *
 * - head_TRUEID : true pid
 *
 * - head_TRUEP[E|X|Y|Z] : true four vector momentum
 * - head_TRUEORIGINVERTEX_[X|Y|Z] : position of the true origin vertex.
 * - head_TRUEENDVERTEX_[X|Y|Z] : position of the true end vertex (the
     first one)
 * - head_TRUETAU : true propertime
 * - head_TRUE{ThetaL,ThetaK,Phi} : true helicity angles
 * - head_TRUE{ThetaTr,PhiTr,ThetaVtr} : true transversity angles
 * 
 * \sa TupleToolMCTruth, DecayTreeTuple, MCDecayTreeTuple
 * 
 *  @author Patrick Koppenburg
 *  @date   2009-01-19
 */

class MCTupleToolMCTruth : public GaudiTool, virtual public IMCParticleTupleTool {
public: 
  /// Standard constructor
  MCTupleToolMCTruth( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~MCTupleToolMCTruth( ); ///< Destructor
  virtual StatusCode initialize();
  virtual StatusCode fill( const LHCb::MCParticle*
			   , const LHCb::MCParticle*
			   , const std::string&
			   , Tuples::Tuple& );


protected:

private:

  bool m_storeKinetic; //!< Store kinetic information from the associated candidate
  bool m_storePT; //!< Store the propertime information for associated composite particle
  bool m_storeVertexes; //!< Store the end and origin true vertex information
  bool isStable(const LHCb::MCParticle * mc) const 
  {
    if(!mc) return true;
    if(mc->endVertices().empty()) return true;
    unsigned int anID=abs(mc->particleID().pid());
    
    if(anID == 11 || anID == 13 
       || anID == 22 || anID == 12 
     || anID == 111 ||  anID == 211 
       || anID == 130 ||  anID == 321
       || anID == 2112 ||  anID == 2212) return true;
    return false;
}

};
#endif // MCTUPLETOOLMCTRUTH_H

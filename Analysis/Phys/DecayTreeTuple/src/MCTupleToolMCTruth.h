// $Id: MCTupleToolMCTruth.h,v 1.3 2009-01-22 10:36:30 pkoppenb Exp $
#ifndef MCTUPLETOOLMCTRUTH_H 
#define MCTUPLETOOLMCTRUTH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IMCParticleTupleTool.h"            // Interface


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

};
#endif // MCTUPLETOOLMCTRUTH_H

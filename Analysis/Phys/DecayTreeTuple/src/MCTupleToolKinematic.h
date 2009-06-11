// $Id: MCTupleToolKinematic.h,v 1.2 2009-06-11 15:42:59 rlambert Exp $
#ifndef MCTUPLETOOLKINEMATIC_H 
#define MCTUPLETOOLKINEMATIC_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IMCParticleTupleTool.h"            // Interface
#include "Event/MCParticle.h"            // Interface
/** @class MCTupleToolKinematic MCTupleToolKinematic.h
 *  
 *  Fill MC Particle 
 *
 * See the properties inline documentation to toggle on/off some of
 * the columns
 *
 * - head_TRUEID : true pid
 *
 * - head_TRUEP[E|X|Y|Z] : true four vector momentum
 * - head_TRUEPT : true transverse momentum, PT
 * - head_TRUEORIGINVERTEX_[X|Y|Z] : position of the true origin vertex.
 * - head_TRUEENDVERTEX_[X|Y|Z] : position of the true end vertex (the
     first one)
 * - head_TRUEISSTABLE : MCAssociate has no daughters.
 * - head_TRUETAU : true propertime
 * 
 * \sa MCTupleToolKinematic, DecayTreeTuple, MCDecayTreeTuple
 * 
 *  @author Patrick Koppenburg
 *  @date   2009-01-19
 */

class MCTupleToolKinematic : public GaudiTool, virtual public IMCParticleTupleTool {
public: 
  /// Standard constructor
  MCTupleToolKinematic( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~MCTupleToolKinematic( ); ///< Destructor
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
  

#include "isStable.h"

};
#endif // MCTUPLETOOLMCTRUTH_H

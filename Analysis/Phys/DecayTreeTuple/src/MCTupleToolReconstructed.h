// $Id: MCTupleToolReconstructed.h,v 1.2 2009-01-22 17:52:41 pkoppenb Exp $
#ifndef MCTUPLETOOLMCRECONSTRUCTED_H 
#define MCTUPLETOOLMCRECONSTRUCTED_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IMCParticleTupleTool.h"            // Interface
#include "Kernel/Particle2MCLinker.h"
#include "Event/ProtoParticle.h"

/** @class MCTupleToolReconstructed MCTupleToolReconstructed.h
 *  
 *  Fill MC Particle 
 *
 * See the properties inline documentation to toggle on/off some of
 * the columns
 *
 * - head_Reconstructible : IMCReconstructible category
 * - head_Reconstructed : IMCReconstructed  category
 * - head_PP_PX,Y,Z : ProtoParticle PX, PY, PZ
 * - head_PP_Weight : ProtoParticle association weight
 * 
 * \sa MCDecayTreeTuple, IMCReconstructible, IMCReconstructed
 * 
 *  @author Patrick Koppenburg
 *  @date   2009-01-22
 */
class IMCReconstructible ;
class IMCReconstructed ;

class MCTupleToolReconstructed : public GaudiTool, virtual public IMCParticleTupleTool {
public: 
  /// Standard constructor
  MCTupleToolReconstructed( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~MCTupleToolReconstructed( ); ///< Destructor
  virtual StatusCode initialize();
  virtual StatusCode fill( const LHCb::MCParticle*
			   , const LHCb::MCParticle*
			   , const std::string&
			   , Tuples::Tuple& );


protected:

private:
  std::vector<std::pair<const LHCb::ProtoParticle*,double> > getProtos(const LHCb::MCParticle*) const;
private:
  IMCReconstructible* m_recible ; ///< Reconstructible tool
  IMCReconstructed* m_rected ; ///< Reconstructed too
  ProtoParticle2MCLinker* m_pCPPAsct ; ///< Charged protoparticle associator
  ProtoParticle2MCLinker* m_pNPPAsct ; ///< Neutral protoparticle associator             
  bool m_associate ; ///< Do association
};
#endif // MCTUPLETOOLMCRECONSTRUCTED_H

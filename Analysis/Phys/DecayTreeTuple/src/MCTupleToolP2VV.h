// $Id: MCTupleToolP2VV.h,v 1.1 2009-01-19 18:07:45 pkoppenb Exp $
#ifndef MCTUPLETOOLP2VV_H 
#define MCTUPLETOOLP2VV_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IMCParticleTupleTool.h"            // Interface

/** @class MCTupleToolP2VV MCTupleToolP2VV.h
 *  
 *  
 *  Fill MC Particle 
 *
 * See the properties inline documentation to toggle on/off some of
 * the columns
 *
 * - head_XXXX : 
 * 
 * \sa TupleToolMCTruth, DecayTreeTuple
 *
 *  @author Patrick Koppenburg
 *  @date   2009-01-19
 */
class IP2VVMCPartAngleCalculator ;
class MCTupleToolP2VV : public GaudiTool, virtual public IMCParticleTupleTool {
public: 
  /// Standard constructor
  MCTupleToolP2VV( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~MCTupleToolP2VV( ); ///< Destructor
  virtual StatusCode initialize();
  virtual StatusCode fill( const LHCb::MCParticle*
			   , const LHCb::MCParticle*
			   , const std::string&
			   , Tuples::Tuple& );


protected:

private:
  IP2VVMCPartAngleCalculator*   m_angleTool;
  std::string m_calculator; //!< Name of the tool for the angles calculation

};
#endif // MCTUPLETOOLP2VV_H

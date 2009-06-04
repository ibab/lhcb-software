// $Id: TupleToolMCTruth.h,v 1.7 2009-06-04 10:54:46 rlambert Exp $
#ifndef JBOREL_TUPLETOOLMCTRUTH_H
#define JBOREL_TUPLETOOLMCTRUTH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IMCParticleTupleTool.h"            // Interface
#include "Kernel/IParticleTupleTool.h"            // Interface

//struct Particle2MCLinker;
#include "Kernel/Particle2MCLinker.h"
#include "Kernel/IDaVinciAssociatorsWrapper.h"
#include "Kernel/IParticle2MCWeightedAssociator.h"
#include "Kernel/MCAssociation.h"
#include <vector>
/** @class TupleToolMCTruth TupleToolMCTruth.h jborel/TupleToolMCTruth.h
 *
 * \brief Fill MC truth info if a link is present
 *
 * Uses the DaVinci smart associator to perform the association.
 *
 *
 * - head_TRUEID : true pid
 *
 
 * To add more entries, add the appropriate MCTupleTool

 * Configure the option ToolList to add MCTupleTools

 * The MCAssociation is run only once, then these tuple tools are called


 * \sa DecayTreeTuple
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
 *  2008-09-23 Adlene Hicheur - Added true angles information for P2VV
 *  2009-06-03 Rob Lambert - Major Changes
 */

class TupleToolMCTruth : public GaudiTool, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolMCTruth( const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual ~TupleToolMCTruth(){}; ///< Destructor

  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );

  virtual StatusCode initialize();
  //  virtual StatusCode finalize();

private:

  IParticle2MCWeightedAssociator* m_smartAssociation;

  std::vector< std::string > m_toolList;///<names of all MCTupleTools, set by the option ToolList
  
  std::vector< IMCParticleTupleTool* > m_mcTools;///<vector of MCTools to fill
  
};

#endif // JBOREL_TUPLETOOLMCTRUTH_H

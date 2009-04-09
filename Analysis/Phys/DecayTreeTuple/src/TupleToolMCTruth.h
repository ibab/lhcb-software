// $Id: TupleToolMCTruth.h,v 1.6 2009-04-09 19:10:07 gligorov Exp $
#ifndef JBOREL_TUPLETOOLMCTRUTH_H
#define JBOREL_TUPLETOOLMCTRUTH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

//struct Particle2MCLinker;
#include "Kernel/Particle2MCLinker.h"
#include "Kernel/IDaVinciAssociatorsWrapper.h"
#include "Kernel/IParticle2MCWeightedAssociator.h"
#include "Kernel/MCAssociation.h"
/** @class TupleToolMCTruth TupleToolMCTruth.h jborel/TupleToolMCTruth.h
 *
 * \brief Fill MC truth info if a link is present
 *
 * Whenever their is more than one associated candidate, stores only
 * the first one.
 *
 * See the properties inline documentation to toggle on/off some of
 * the columns
 *
 * - head_TRUEID : true pic
 *
 * - head_MC_ASSOCNUMBER : the number of associated mc candidates
     (only tell how many you missed)

 * - head_TRUEP[E|X|Y|Z] : true four vector momentum

 * - head_TRUEORIGINVERTEX_[X|Y|Z] : position of the true origin vertex.

 * - head_TRUEENDVERTEX_[X|Y|Z] : position of the true end vertex (the
     first one)

 * - head_TRUETAU : true propertime

 * - head_TRUE{ThetaL,ThetaK,Phi} : true helicity angles

 * - head_TRUE{ThetaTr,PhiTr,ThetaVtr} : true transversity angles

 * \sa DecayTreeTuple
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
 *  2008-09-23 Adlene Hicheur - Added true angles information for P2VV
 */

class IP2VVMCPartAngleCalculator;
class IBackgroundCategory;

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

  //  const GaudiAlgorithm* getParent() const ;

  IParticle2MCWeightedAssociator* m_smartAssociation;

  IP2VVMCPartAngleCalculator*   m_angleTool;

  std::vector<std::string> m_assocInputs;
  bool m_storeNumberOfAssoc; //!< Store in the tuple the number of MC Association found;
  bool m_storeKinetic; //!< Store kinetic information from the associated candidate
  bool m_storePT; //!< Store the propertime information for associated composite particle
  bool m_storeVertexes; //!< Store the end and origin true vertex information
  bool m_fillangles; //!< Store the true angular information (from P2VV)
  std::string m_calculator; //!< Name of the tool for the angles calculation

};

#endif // JBOREL_TUPLETOOLMCTRUTH_H

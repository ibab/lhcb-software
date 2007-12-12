// $Id: TupleToolMCTruth.h,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLMCTRUTH_H
#define JBOREL_TUPLETOOLMCTRUTH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

//struct Particle2MCLinker;
#include "Kernel/Particle2MCLinker.h"
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

 * \sa DecayTreeTuple
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
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

  //  const GaudiAlgorithm* getParent() const ;

  Particle2MCLinker *m_pLink;
  Particle2MCLinker *m_pComp;
  Particle2MCLinker *m_pChi2;

  std::vector<std::string> m_assocInputs;
  bool m_useChi2Method; //!< Use the chi2 associator instead of the link and composite associator
  bool m_storeNumberOfAssoc; //!< Store in the tuple the number of MC Association found;
  bool m_storeKinetic; //!< Store kinetic information from the associated candidate
  bool m_storePT; //!< Store the propertime information for associated composite particle
  bool m_storeVertexes; //!< Store the end and origin true vertex information
};

#endif // JBOREL_TUPLETOOLMCTRUTH_H

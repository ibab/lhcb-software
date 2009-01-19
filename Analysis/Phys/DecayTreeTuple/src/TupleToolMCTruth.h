// $Id: TupleToolMCTruth.h,v 1.4 2009-01-19 18:07:45 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLMCTRUTH_H
#define JBOREL_TUPLETOOLMCTRUTH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface
#include "Kernel/IMCParticleTupleTool.h"

//struct Particle2MCLinker;
#include "Kernel/Particle2MCLinker.h"
#include "Kernel/IDaVinciAssociatorsWrapper.h"
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
 * - head_MC_ASSOCNUMBER : the number of associated mc candidates
     (only to tell how many you missed)
 * - head_MC_CHI2WHEIGHT : the chi2 weight if applicable
 *
 * On succesful association it calls MCTupleToolMCTruth
 *
 * \sa DecayTreeTuple, MCTupleToolMCTruth
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

  IDaVinciAssociatorsWrapper* m_linkerTool_Links;
  IDaVinciAssociatorsWrapper* m_linkerTool_Chi2;
  IDaVinciAssociatorsWrapper* m_linkerTool_Composite;
  IMCParticleTupleTool* m_mcTool ; ///< Tool that does the MC
  IMCParticleTupleTool* m_mcAngles ; ///< Tool that does the MC angles

  std::vector<std::string> m_assocInputs;
  bool m_useChi2Method; //!< Use the chi2 associator instead of the link and composite associator
  bool m_storeNumberOfAssoc; //!< Store in the tuple the number of MC Association found;
  bool m_fillangles; //!< Store the true angular information (from P2VV)

};

#endif // JBOREL_TUPLETOOLMCTRUTH_H

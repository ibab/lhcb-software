// $Id: TupleToolMCHierarchy.h,v 1.2 2008-07-08 11:18:33 sposs Exp $
#ifndef SPOSS_TUPLETOOLMCHIERARCHY_H
#define SPOSS_TUPLETOOLMCHIERARCHY_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

//struct Particle2MCLinker;
#include "Kernel/Particle2MCLinker.h"
#include "Kernel/IDaVinciAssociatorsWrapper.h"

/** @class TupleToolMCHierarchy
 *
 * \brief Fill MC hierarchy info if a link is present
 *
 * Based on the same association than the TupleToolMCTruth
 *
 *
 * - head_MC_MOTHER_ID : true mc mother ID
 
 * - head_MC_MOTHER_KEY : true mc mother key
 
 * - head_MC_GD_MOTHER_ID : grand mother ID

 * - head_MC_GD_MOTHER_KEY : grand mother key

 * - head_MC_GD_GD_MOTHER_ID : grand grand mother ID

 * - head_MC_GD_GD_MOTHER_KEY : grand grand mother key

 * \sa DecayTreeTuple
 *
 *  @author Stephane Poss
 *  @date   2008-02-28
 */
class TupleToolMCHierarchy : public GaudiTool, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolMCHierarchy( const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual ~TupleToolMCHierarchy(){}; ///< Destructor

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

  std::vector<std::string> m_assocInputs;
  bool m_useChi2Method; //!< Use the chi2 associator instead of the link and composite associator
};

#endif // SPOSS_TUPLETOOLMCHIERARCHY_H

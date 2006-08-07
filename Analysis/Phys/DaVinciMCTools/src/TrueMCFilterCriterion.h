// $Id: TrueMCFilterCriterion.h,v 1.8 2006-08-07 13:57:17 jpalac Exp $
#ifndef TRUEMCFILTERCRITERION_H 
#define TRUEMCFILTERCRITERION_H 1


// base class
#include "Kernel/FilterCriterionBase.h"

class IMCDecayFinder;
#include "Relations/RelationWeighted1D.h"

/** @class TrueMCFilterCriterion TrueMCFilterCriterion.h
 *  
 *  Filters particles that come from the wanted true decay
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-09-13
 *
 *  Reviewed to have gammas 2005-03-20 : Luis Fernandez
 *
 */
class TrueMCFilterCriterion : public FilterCriterionBase {

public: 

  /// Standard constructor
  TrueMCFilterCriterion( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);

  virtual ~TrueMCFilterCriterion( ); ///< Destructor
  /// Initialize
  StatusCode initialize( );
  

  /// Finalize
  StatusCode finalize( );

protected: // methods

  /// Test if filter is satisfied.
  virtual bool testParticle( const LHCb::Particle* const & part );

private:
  bool findMCParticle(const LHCb::MCParticle* MC);

  IMCDecayFinder* m_pMCDecFinder ;        ///<  MCDecayFinder
  Particle2MCLinker* m_pLinker;   ///<  Link associator
  bool m_filterOut ;                      ///<  If true signal will be vetoed
  typedef LHCb::Calo2MC::IClusterTable Table ;
  const Table* m_pCl2MCPTable; 
  std::string m_particlePath;

};
#endif // TRUEMCFILTERCRITERION_H

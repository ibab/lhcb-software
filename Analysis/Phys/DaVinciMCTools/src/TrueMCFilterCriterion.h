// $Id: TrueMCFilterCriterion.h,v 1.3 2005-02-08 17:12:38 pkoppenb Exp $
#ifndef TRUEMCFILTERCRITERION_H 
#define TRUEMCFILTERCRITERION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IFilterCriterion.h"            // Interface
class IMCDecayFinder;

/** @class TrueMCFilterCriterion TrueMCFilterCriterion.h
 *  
 *  Filters particles that come from the wanted true decay
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-09-13
 */
class TrueMCFilterCriterion : public GaudiTool, virtual public IFilterCriterion {
public: 
  /// Standard constructor
  TrueMCFilterCriterion( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);

  virtual ~TrueMCFilterCriterion( ); ///< Destructor
  /// Initialize
  StatusCode initialize( );

  /// Test if particle is assocaited with MC truth
  inline bool isSatisfied( const Particle* const & part );

  /// Test if particle is assocaited with MC truth
  inline bool operator()( const Particle* const & part );

  /// Finalize
  StatusCode finalize( );

protected:

private:
  bool findMCParticle(const MCParticle* MC);

  IMCDecayFinder* m_pMCDecFinder ;        ///<  MCDecayFinder
  Particle2MCLinksAsct::IAsct* m_pAsct;   ///<  Composite associator
  bool m_filterOut ;                      ///<  If true signal will be vetoed

};
#endif // TRUEMCFILTERCRITERION_H

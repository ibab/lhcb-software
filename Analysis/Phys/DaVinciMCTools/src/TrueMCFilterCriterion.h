// $Id: TrueMCFilterCriterion.h,v 1.5 2005-06-28 15:37:27 lazzecri Exp $
#ifndef TRUEMCFILTERCRITERION_H 
#define TRUEMCFILTERCRITERION_H 1


// base class
#include "Kernel/FilterCriterionBase.h"

class IMCDecayFinder;
#include "Event/CaloCluster.h"

/** @class TrueMCFilterCriterion TrueMCFilterCriterion.h
 *  
 *  Filters particles that come from the wanted true decay
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-09-13
 *
 *  Reviewed to have gammas 2005-03-20 : Luis Fernandez
 *
 *  FIXME : 
 *  - association of online gammas requires offline CaloClusters for now
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
  virtual bool testParticle( const Particle* const & part );

private:
  bool findMCParticle(const MCParticle* MC);

  IMCDecayFinder* m_pMCDecFinder ;        ///<  MCDecayFinder
  Particle2MCLinksAsct::IAsct* m_pAsct;   ///<  Link associator
  bool m_filterOut ;                      ///<  If true signal will be vetoed
  int m_gammaID; // gamma particle ID
  typedef IAssociatorWeighted<CaloCluster, MCParticle, float> IAsctCl2MCP;
  typedef IAsctCl2MCP::DirectType DirectType;
  const IAsctCl2MCP* m_pAsctCl2MCP; 
  const std::string m_CaloClustersPath; // Calo clusters path

};
#endif // TRUEMCFILTERCRITERION_H

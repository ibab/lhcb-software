// $Id: TrueMCFilterCriterion.h,v 1.4 2005-03-20 23:20:35 lfernan Exp $
#ifndef TRUEMCFILTERCRITERION_H 
#define TRUEMCFILTERCRITERION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IFilterCriterion.h"            // Interface
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
  Particle2MCLinksAsct::IAsct* m_pAsct;   ///<  Link associator
  bool m_filterOut ;                      ///<  If true signal will be vetoed
  int m_gammaID; // gamma particle ID
  typedef IAssociatorWeighted<CaloCluster, MCParticle, float> IAsctCl2MCP;
  typedef IAsctCl2MCP::DirectType DirectType;
  const IAsctCl2MCP* m_pAsctCl2MCP; 
  const std::string m_CaloClustersPath; // Calo clusters path

};
#endif // TRUEMCFILTERCRITERION_H

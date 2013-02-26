// $Id: HepMCJets2Jets.cpp,v 1.1 2009-12-14 12:34:33 cocov Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
#include "LoKi/MCMatchObj.h"
#include "Kernel/IJets2Jets.h"            // Interface
#include "Event/Particle.h"
#include "LoKi/Kinematics.h"
#include "LoKi/Geometry.h"

#include "Relations/Relation1D.h"
#include "Relations/Relation2D.h"

#include "LoKi/PhysMCParticleCuts.h"
#include "LoKi/LoKiPhys.h"

#include <math.h>
namespace LoKi
{
  class HepMCJets2JetsDeltaR
    : public virtual IJets2Jets
    , public         GaudiTool
  {
    // ========================================================================
    /// friend factory for instantiation
    friend class ToolFactory<LoKi::HepMCJets2JetsDeltaR> ;
  public:

    // ========================================================================
    /// standard initialization of the tool
    void makeRelation( const IJets2Jets::Jets& StdPrimaryJets,
                       const IJets2Jets::Jets& StdSecondaryJets,
                       IJets2Jets::Table &table ) const ;
  protected:
    // ========================================================================
    /// standard constructor
    HepMCJets2JetsDeltaR
    ( const std::string& type   , ///< tool type ???
      const std::string& name   , ///< tool name
      const IInterface*  parent )
      : GaudiTool ( type, name , parent )
    {
      declareInterface<IJets2Jets>(this);
    }
    /// virtual protected destructor
    virtual ~HepMCJets2JetsDeltaR() {}
    // ========================================================================
  private:
    // ========================================================================
    // default constructor is disabled
    HepMCJets2JetsDeltaR() ;
    // copy constructor is disabled
    HepMCJets2JetsDeltaR( const HepMCJets2JetsDeltaR& ) ;
    // assignement operator is disabled
    HepMCJets2JetsDeltaR& operator=( const HepMCJets2JetsDeltaR& ) ;
    // ========================================================================
  private:
    // ========================================================================
  };
}
// end of namespace LoKi

void 
LoKi::HepMCJets2JetsDeltaR::makeRelation( const IJets2Jets::Jets& StdPrimaryJets,
                                          const IJets2Jets::Jets& StdSecondaryJets,
                                          IJets2Jets::Table &table ) const
{
  for( IJets2Jets::Jets::const_iterator secjet =StdSecondaryJets.begin(); 
       StdSecondaryJets.end()!= secjet ; ++secjet )
  {
    LoKi::Types::Fun DR2 = LoKi::Cuts::DR2((*secjet)->momentum());
    for(IJets2Jets::Jets::const_iterator primjet =StdPrimaryJets.begin(); 
        StdPrimaryJets.end()!= primjet ; ++primjet )
    {
      table.relate(*primjet,*secjet,std::sqrt(DR2(*primjet)));
    }
  }
}

// ============================================================================
/// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,HepMCJets2JetsDeltaR)
// ============================================================================
// The END
// ============================================================================

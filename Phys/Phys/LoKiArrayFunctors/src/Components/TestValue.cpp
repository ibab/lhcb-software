// $Id:$ 
// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
#include "Kernel/IParticleValue.h"            // Interface
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class SpecialValue 
   *  the simplest possible implementation of IPArticleValue integrface 
   *  @see IParticlValue 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2015-03-12
   */
  class SpecialValue: public extends1<GaudiTool, IParticleValue>
  {
    // ========================================================================
    /// friend class factory for instantiation
    friend class ToolFactory<LoKi::SpecialValue>;
    // ========================================================================
  protected:
    // ========================================================================
    /// Standard constructor
    SpecialValue( const std::string& type,
                  const std::string& name,
                  const IInterface* parent)
      : base_class ( type , name , parent )
      , m_value    ( 42 )
    {
      declareProperty ( "Value" , m_value , "Value to be used" );
    }
    virtual ~SpecialValue() {}; ///< Destructor
    // ========================================================================
  private:
    // ========================================================================
    SpecialValue() ; 
    SpecialValue           ( const SpecialValue& ) ;
    SpecialValue& operator=( const SpecialValue& ) ;
    // ========================================================================    
  public:  // IParticleValue
    // ========================================================================    
    virtual double operator()( const LHCb::Particle* /* p */ ) const 
    { return m_value ; }
  // ========================================================================    
  private:
    // ========================================================================    
    /// the value 
    double m_value ; // the value    
    // ========================================================================
  };
  // ==========================================================================
}
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,SpecialValue) 
// ============================================================================
// The END 
// ============================================================================

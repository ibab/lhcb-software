// $Id:$ 
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/VectorMap.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
#include "Kernel/IParticleValue.h"            // Interface
#include "Kernel/IParticleDictTool.h"
// ============================================================================
// LoKi                                                                                            
// ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Hybrid
  {
    // ========================================================================
    /** @class DictValue DictValue.h Components/DictValue.h
     * Accessing one field of the dictionary
     * used to construct cuts on dictionary values produced by DictTransform
     * in collaboration with the VALUE adapter
     * always check if there already exists a LoKi functor to get the variable directly
     *
     *  @author Sebastian Neubert
     *  @date   2013-08-05
     */
    class DictValue : public GaudiTool, virtual public IParticleValue 
    {
    public:
      // ======================================================================
      /// friend class factory for instantiation
      friend class ToolFactory<LoKi::Hybrid::DictValue>;
      
      /// Standard constructor
      DictValue( const std::string& type,
                 const std::string& name,
                 const IInterface* parent)
        : GaudiTool ( type, name , parent )
        , m_sourcename (   )  
        , m_source     ( 0 )
        , m_key        (   )
      {
        declareInterface<IParticleValue>(this);
        
        ///
        declareProperty
          ( "Source" , m_sourcename ,
            "Type/Name for Source Dictionary Tool" );
        declareProperty
          ( "Key"    , m_key,
            "Name of the variable that should be returned");
      }
      virtual ~DictValue( ){}; ///< Destructor
      // ======================================================================
    public:
      // ======================================================================
      /** get the value
       *  @see IParticleValue
       *  @param particle the particle which the dictionary tool-chain will operate on
       *  @return value
       */
      virtual double operator() ( const LHCb::Particle* particle ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// initialization of the tool
      virtual StatusCode initialize ()
      {
        StatusCode sc = GaudiTool::initialize() ;
        if ( sc.isFailure() ) {  return sc ; }               // RETURN 
        // get the dicttool
        m_source = tool<IParticleDictTool>(m_sourcename, this);
        if(m_source == NULL) 
        { return Error("Unable to find the source DictTool " + m_sourcename , sc ) ; }
        return sc;
      }
      // ======================================================================
    private:
      // ======================================================================
      std::string        m_sourcename ;
      IParticleDictTool* m_source     ;
      std::string        m_key        ; /// name of the variable we want to access
      // ======================================================================
    };
    // ========================================================================
  }
  // ==========================================================================
}
// ============================================================================
/*  Fill the tuple.
 *  @see IParticleTupleTool
 *  @param top      the top particle of the decay.
 *  @param particle the particle about which some info are filled.
 *  @param head     prefix for the tuple column name.
 *  @param tuple    the tuple to fill
 *  @return status code
 */
// ============================================================================
double LoKi::Hybrid::DictValue::operator() 
  ( const LHCb::Particle* particle ) const
{
  /// call IParticleDictTool to aquire the dictionary
  /// request the dictionary of variables from the source
  IParticleDictTool::DICT dict;
  m_source->fill(particle,dict).ignore();
  
  // return the value for the given key
  return dict[m_key];
}
// ============================================================================
/// Declaration of the Tool Factory
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,DictValue)
// ============================================================================

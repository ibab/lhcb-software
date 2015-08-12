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
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/Tuples.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
#include "Kernel/IParticleTupleTool.h"            // Interface
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
    /** @class Dict2Tuple Dict2Tuple.h Components/Dict2Tuple.h
     *  Writing a Dictionary into the ntuple
     *
     *  @author Sebastian Neubert
     *  @date   2013-07-11
     */
    class Dict2Tuple : public GaudiTool, virtual public IParticleTupleTool 
    {
    public: 
      // ======================================================================
      /// friend class factory for instantiation
      friend class ToolFactory<LoKi::Hybrid::Dict2Tuple>;
      // ======================================================================
    protected:
      // ======================================================================
      /// Standard constructor
      Dict2Tuple( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent ) 
        : GaudiTool ( type, name , parent )
        , m_sourcename (   ) 
        , m_source     ( 0 )
      {
        declareInterface<IParticleTupleTool>(this);
        ///
        declareProperty 
          ( "Source"     , 
            m_sourcename , 
            "Type/Name for Source Dictionary Tool" );
      }
      // ======================================================================
      virtual ~Dict2Tuple( ){}; ///< Destructor
      // ======================================================================      
    public:
      // ======================================================================
      /** Fill the tuple. 
       *  @see IParticleTupelTool 
       *  @param top      the top particle of the decay.
       *  @param particle the particle about which some info are filled.
       *  @param head     prefix for the tuple column name.
       *  @param tuple    the tuple to fill
       *  @return status code
       */
      virtual StatusCode fill
      ( const LHCb::Particle* top      , 
        const LHCb::Particle* particle , 
        const std::string&    head     , 
        Tuples::Tuple&        tuple    ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// initialization of the tool 
      virtual StatusCode initialize () 
      {
        StatusCode sc = GaudiTool::initialize() ;
        if ( sc.isFailure() ) {  return sc ; }               // RETURN 
        
        // get the dicttool
        m_source = tool<IParticleDictTool>( m_sourcename, this);
        if ( m_source == NULL ) 
        { return Error("Unable to find the source DictTool " + m_sourcename , sc ) ; }
        //
        return StatusCode::SUCCESS;
      }
      // ======================================================================
    private:
      // ======================================================================
      std::string m_sourcename;
      IParticleDictTool* m_source;
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
StatusCode LoKi::Hybrid::Dict2Tuple::fill
( const LHCb::Particle* top      , 
  const LHCb::Particle* particle , 
  const std::string&    head     , 
  Tuples::Tuple&        tuple    ) 
{
  if ( 0 == top       ) { Warning ( "LHCb::Particle* 'Top' points to NULL" ) ; }
  if ( 0 == particle  ) { Warning ( "LHCb::Particle*       points to NULL" ) ; }
  //
  if ( !tuple.valid() ) { return Error ( "Invalid tuple " ) ; } 
  
  // call IParticleDictTool to aquire the dictionary
  /// request the dictionary of variables from the source 
  IParticleDictTool::DICT dict;
  m_source->fill( particle , dict ).ignore();
  
  // prepend the head of the current branch to the variable to make sure
  // columns are uniquely named
  std::string head_ = head + "_" ;
  // Put the items in the dictionary into the tuple
  for ( IParticleDictTool::DICT::const_iterator item = dict.begin() ; 
        dict.end() != item ; ++item ) 
  {
    // fill N-tuple 
    tuple -> column ( head_ + item->first , item->second ) ; 
  }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// Declaration of the Tool Factory
// ============================================================================ 
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,Dict2Tuple)
// ============================================================================ 

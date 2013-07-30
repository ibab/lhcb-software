
// Include files
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/VectorMap.h"
// GaudiAlg                                                                                                                              
// ============================================================================ 
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/Tuples.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiAlg/GaudiTool.h"

#include "Kernel/IParticleTupleTool.h"            // Interface
#include "Kernel/IParticleDictTool.h"
// LoKi                                                                                                                                   // ============================================================================
#include "LoKi/PhysTypes.h"
#include "LoKi/IHybridFactory.h"


// ============================================================================     
namespace LoKi
{
  // ==========================================================================                                                                                                      
  namespace Hybrid
  {

/** @class Dict2TupleTool Dict2TupleTool.h Components/Dict2TupleTool.h
 *  Writing a Dictionary into the ntuple
 *
 *  @author Sebastian Neubert
 *  @date   2013-07-11
 */
class Dict2TupleTool : public GaudiTool, virtual public IParticleTupleTool {
public: 
 /// friend class factory for instantiation
  friend class ToolFactory<LoKi::Hybrid::Dict2TupleTool>;

  /// Standard constructor
  Dict2TupleTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent) :
    GaudiTool ( type, name , parent )
  {
    declareInterface<IParticleTupleTool>(this);

    ///
    declareProperty 
      ( "Source" , m_sourcename , 
	"Type/Name for Source Dictionary Tool" );
  }

  virtual ~Dict2TupleTool( ){}; ///< Destructor

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
	m_source = tool<IParticleDictTool>(m_sourcename, this);
	if(m_source == NULL) return Error("Unable to find the source DictTool " + m_sourcename , sc ) ;

	info() << "Successfully initialized Dict2TupleTool" << endmsg;

	return StatusCode::SUCCESS;


      }
      // ======================================================================
      /// finalization of the tool 
      virtual StatusCode finalize () 
      {
        return GaudiTool::finalize () ;
      }
      // ======================================================================
  
protected:

private:
  std::string m_sourcename;
  IParticleDictTool* m_source;

};


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
StatusCode LoKi::Hybrid::Dict2TupleTool::fill
( const LHCb::Particle* top      , 
  const LHCb::Particle* particle , 
  const std::string&    head     , 
  Tuples::Tuple&        tuple    ) 
{
  if ( 0 == top       ) { Warning ( "LHCb::Particle* 'Top' points to NULL" ) ; }
  if ( 0 == particle  ) { Warning ( "LHCb::Particle*       points to NULL" ) ; }
  //
  if ( !tuple.valid() ) { return Error ( "Invalid tuple " ) ; } 
  
  // call the IParticleDictTool to aquire the dictionaire
 /// request the dictionary of variables from the DictTool
  IParticleDictTool::DICT dict;
  m_source->fill(particle,head,dict).ignore();
  
  // append the head of the current branch to the variable to make sure
  // columns are uniquely named
  std::string head_ = head + "_" ;
  // Put the items in the dictionaire into the tuple
  for ( IParticleDictTool::DICT::const_iterator item = dict.begin() ; 
        dict.end() != item ; ++item ) 
  {
    // fill N-tuple 
    tuple -> column ( head_ + item->first , item->second ) ; 
  }
  //
  return StatusCode::SUCCESS ;
}


  } // end namespace Hybrid
} // end namespace LoKi
/// Declaration of the Tool Factory                                                                 
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi::Hybrid,Dict2TupleTool)

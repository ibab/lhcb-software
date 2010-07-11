// $Id: $
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// local
// ============================================================================
#include "VertexFitter.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @class FastVertexFitter 
   *  Preconfigured instance of LoKi::VertexFitetr with smaller number 
   *  of iterations and loose tolerances 
   * 
   *  It used 'loose' convergency srettria:
   *    - delta-distance as 3.0 micrometers 
   *    - delta-chi2     as 0.1 
   *    - maximal number of iteration for 'fit'    is 5 
   *    - maximal number of iteration for 'add'    is 3 
   *    - maximal number of iteration for 'remove' is 3 
   * 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2010-07-11
   *  @see LoKi::VertexFitter 
   */
  class FastVertexFitter: public LoKi::VertexFitter 
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class ToolFactory<LoKi::FastVertexFitter> ; // the friend factory 
    // ========================================================================
    protected:
    //=========================================================================
    /** strandard constructor 
     *  @see LoKi::VertexFitter
     *  @see GaudiTool
     *  @see AlgTool 
     *  @see ToolFactory
     */
    FastVertexFitter 
    ( const std::string& type   , 
      const std::string& name   , 
      const IInterface*  parent ) ;
    /// virtual & protected destrcutor
    virtual ~FastVertexFitter() ;            // virtual & protected destructor
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    FastVertexFitter () ;                // the default constructor is disabled 
    /// copiy constructoir is disabled 
    FastVertexFitter ( const FastVertexFitter& ) ;          // copy is disabled 
    /// assignement operator is disabled 
    FastVertexFitter& operator=( const FastVertexFitter& ); //         disabled 
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
/*  standard constructor 
 *  @see LoKi::VertexFitter
 *  @see GaudiTool
 *  @see AlgTool 
 *  @see ToolFactory
 */
// ============================================================================
LoKi::FastVertexFitter::FastVertexFitter 
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : LoKi::VertexFitter ( type , name , parent ) 
{
  StatusCode sc = setProperty ( "MaxIterations"    ,  5 ) ;
  Assert ( sc.isSuccess() , "Unable to redefine property 'MaxIterations'"    ) ;
  sc            = setProperty ( "MaxIterForAdd"    ,  3 ) ;
  Assert ( sc.isSuccess() , "Unable to redefine property 'MaxIterForAdd'"    ) ;
  sc            = setProperty ( "MaxIterForRemove" ,  3 ) ;
  Assert ( sc.isSuccess() , "Unable to redefine property 'MaxIterForRemove'" ) ;   
  sc            = setProperty ( "DeltaDistance"    ,  3 * Gaudi::Units::micrometer ) ;
  Assert ( sc.isSuccess() , "Unable to redefine property 'DeltaDistance'"    ) ;   
  sc            = setProperty ( "DeltaChi2"        , 10 * Gaudi::Units::perCent    ) ;
  Assert ( sc.isSuccess() , "Unable to redefine property 'DeltaChi2'"        ) ;   
}
// ============================================================================
// destructor 
// ============================================================================
LoKi::FastVertexFitter::~FastVertexFitter(){}
// ============================================================================
/// the factory needed for instantiation
DECLARE_NAMESPACE_TOOL_FACTORY ( LoKi , FastVertexFitter ) ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================




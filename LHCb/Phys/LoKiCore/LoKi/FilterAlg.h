// ============================================================================
#ifndef LOKI_FILTERALG_H 
#define LOKI_FILTERALG_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
/** @file LoKi/FilterAlg.h
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 */
// ============================================================================
namespace LoKi 
{
  // ===========================================================================
  /** @class FilterAlg LoKi/FilterAlg.h
   *  The base class for implementation of various 
   *  "hybrid" filter algorithms
   *
   *  The basic algorithm properties properties  
   *   (in additon to properties form GaudiAlgortuhm base class) are:
   *   - "Factory"   : the type/name of LoKi/Bender "hybrid" factory
   *   - "Code"      : the string represenattion of the functor 
   *   - "Preambulo" : the list of strings for "preambulo"-code
   * 
   *  The methods, convinient for defined class are:
   *     - <c>factory()</c> : gets the access to LoKi/Bende "hybrid" 
   *                          factory type/name 
   *     - <c>code()</c> : get the access to the code string itself
   *     - <c>updateRequired()</c> : simple function which indicated that 
   *                      the properties has been modified and the recreation
   *                      of functor is required 
   *     - <c>i_decode()</c> : helper function for implementation 
   *                           of the actual functor decoding.
   *
   *  The clients are requires to implement pure abstract 
   *   method "decode()".
   *  The "typical" implementation of <c>decode()</c> is:
   *
   *  @code 
   *
   *  StatusCode MyAlg::decode() 
   *  {
   *
   *     typedef XXXXXX FACTORY ;
   *
   *     return i_decode<FACTORY>( m_functor ) ;
   *  }
   *
   *  @endcode 
   *  where <c>m_functor</c> is a data member (placeholder) 
   *  of the type, known to the factory.
   *
   *  The "typical" implementation of <c>execute()</c> is:
   *  
   *  @code 
   *
   *  StatusCode MyAlg::execute() 
   *   {
   *     if ( updateRequired() ) 
   *       { Assert( decode().isSuccess() , "Unable to decode" + code() ) ; }
   *   
   *     ...
   *     return StatusCode::SUCCESS ;
   *   }
   *
   *  @endcode 
   * 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-09-23
   */
  class FilterAlg : public GaudiAlgorithm 
  {
  public:
    // ========================================================================
    /// the initialization of the algorithm 
    virtual StatusCode initialize () ;
    /// the finalization of the algorithm 
    virtual StatusCode finalize   () ;
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor 
     *  @see GaudiAlgorithm 
     *  @see      Algorithm 
     *  @see      AlgFactory
     *  @see     IAlgFactory
     *  @param name the algorithm instance name 
     *  @param pSvc pointer to Service Locator 
     */
    FilterAlg ( const std::string& name ,   // the algorithm instance name 
                ISvcLocator*       pSvc ) ; // pointer to the service locator
    /// virtual and protected destructor 
    virtual ~FilterAlg() ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    FilterAlg () ;                       // the default constructor is disabled 
    /// the copy constructor is disabled 
    FilterAlg ( const FilterAlg& ) ;        // the copy constructor is disabled 
    /// the assignement operator is disabled 
    FilterAlg& operator=( const FilterAlg& ) ;   // the assignement is disabled
    // ========================================================================
  public: // property update handlers 
    // ========================================================================
    /// update the factory 
    void updateFactory   ( Property& /* p */ ) ;          // update the factory 
    /// update the code  
    void updateCode      ( Property& /* p */ ) ;             // update the code 
    /// update the preambulo 
    void updatePreambulo ( Property& /* p */ ) ;        // update the preambulo 
    // ========================================================================
  protected:
    // ========================================================================
    /// get the type/name of the factory 
    const std::string& factory   () const { return m_factory ; }
    /// get the code itself 
    const std::string& code      () const { return m_code    ; }
    /// get the preambulo
    const std::vector<std::string>& preambulo_() const { return m_preambulo_ ; }
    /// get the preambulo
    const std::string& preambulo() const { return m_preambulo ; }    
    // check the nesessity of updated 
    inline bool updateRequired () const 
    { return m_factory_updated || m_code_updated || m_preambulo_updated ; }
    // add to preambulo 
    void addToPreambulo ( const std::string&              item ) ;
    // set the preambulo
    void setPreambulo   ( const std::vector<std::string>& items ) ;
    // ========================================================================
  public:
    /// =======================================================================
    /** decode the functor
     *  it is assumed that this method is implemented as
     *  
     *  StatusCode MyAlg::decode() 
     *  {
     *     typedef XXXXXX FACTORY ;
     *     tydedef LoKi::BasicFunctor<XXXX>::PredicateFromPredicate  FUNCTOR ;
     *     // or...
     *     // tydedef LoKi::BasicFunctor<XXXX>::FunctionFromFunction FUNCTOR ;
     *
     *     FUNCTOR& functor = ... ; 
     *   
     *     return i_decode<FACTORY>( functor ) ;
     *  }
     */
    virtual StatusCode decode () = 0 ;
    /// =======================================================================
  protected:
    // ========================================================================
    /** perform the actual decoding 
     *  @param functor placeholder where to decode the fucntor 
     */
    template <class FACTORY, class FUNCTOR>
    StatusCode i_decode ( FUNCTOR& functor )
    {
      // get the factory 
      FACTORY* _factory = tool<FACTORY> ( factory() , this ) ;
      // use the factory 
      StatusCode sc = _factory-> get ( code() , functor , preambulo () ) ;
      // release the factory (not needed anymore)
      this->releaseTool ( _factory ).ignore() ;
      if ( sc.isFailure() ) 
      { return Error("Unable to decode functor '" + code() + "'" , sc ) ; }
      //
      if( msgLevel(MSG::DEBUG) ) debug() << "Requested code: \n'" 
                                         << code () 
                                         << "'\n -> Decoded functor :\n'" 
                                         << functor << "'" << endmsg ;
      //
      m_factory_updated   = false ;
      m_code_updated      = false ;
      m_preambulo_updated = false ;
      //
      return sc ;
    }
    /// =======================================================================
  protected:
    // ========================================================================
    void set_code_updated      ( const bool value ) 
    { m_code_updated      = value ; }
    void set_factory_updated   ( const bool value )
    { m_factory_updated   = value ; }
    void set_preambulo_updated ( const bool value )
    { m_preambulo_updated = value ; }
    /// =======================================================================
  private:
    // ========================================================================
    /// the type/name for LoKi/Bender "hybrid" factory 
    std::string m_factory   ; // the type/name for LoKi/Bender "hybrid" factory 
    /// the filter/code criteria itself 
    std::string m_code      ;                // the filter/code criteria itself
    /// the preambulo itself 
    std::vector<std::string> m_preambulo_ ;             // the preambulo itself
    /// the preambulo itself 
    std::string m_preambulo ;                           // the preambulo itself
    // ========================================================================
  private:
    // ========================================================================
    /// flag which indicated that factory has been updated 
    bool m_factory_updated   ;                  // the factory has been updated 
    /// flag which indicated that code has been updated 
    bool m_code_updated      ;                     // the code has been updated 
    /// flag which indicated that preambulo has been updated 
    bool m_preambulo_updated ;                // the preambulo has been updated 
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END
// ============================================================================
#endif // LOKI_FILTERALG_H
// ============================================================================


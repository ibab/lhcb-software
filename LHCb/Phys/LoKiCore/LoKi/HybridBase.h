// ============================================================================
#ifndef LOKIHYBRID_HYBRIDBASE_H 
#define LOKIHYBRID_HYBRIDBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg/GaudiTools 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hybrid 
  {
    // =========================================================================
    /** @class Base LoKi/HybridBase.h
     *
     *  The base class for "hybrid" (C++/Python) tools  
     *
     *  This file is a part of LoKi project - 
     *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
     *
     *  The package has been designed with the kind help from
     *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
     *  contributions and advices from G.Raven, J.van Tilburg, 
     *  A.Golutvin, P.Koppenburg have been used in the design.
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date   2004-06-29
     */
    class Base : public                 GaudiTool 
    {
    public:
      typedef std::vector<std::string> Strings ;
    public:
      /// initialization of the tool 
      virtual StatusCode initialize () ;
      /// finalization   of the tool 
      virtual StatusCode finalize  () ;
    protected:
      /** execute the python code 
       *  @attention the method must be invoked *ONLY* with the appropriate scope!!!
       *  @param code python code 
       *  @return status code 
       */
      StatusCode executeCode ( const std::string& code ) const ;
    protected:    
      /// Standard constructor
      Base 
      ( const std::string& type   , 
        const std::string& name   ,
        const IInterface*  parent ) ;
      /// destructor: virtual and protected 
      virtual ~Base( );
    protected:
      /// helper method to decrease amount of the typing
      template <class TYPE>
      inline void       _set 
      ( TYPE*& local , const TYPE& right ) ;
    public:
      // flag to show the prepared code 
      bool showCode () const { return m_showCode ; }
    protected:
      /** prepare the actual python code from the functor:
       *  @param modules list of modules 
       *  @param actor the actual actor
       *  @param code the code for the functor 
       *  @param lines more python lines to be used 
       *  @param context  even more python code to be used 
       *  @return the valid python code 
       */
      std::string makeCode 
      ( const Strings&     modules             , 
        const std::string& actor               ,
        const std::string& code                , 
        const Strings&     lines   = Strings() , 
        const std::string& context = ""        ) const ;
    private:
      // the default constructor is disabled 
      Base () ; ///< the default constructor is disabled 
      // copy constructor is disabled 
      Base           ( const Base& ) ; ///< copy constructor is disabled 
      // assignement operator is disabled 
      Base& operator=( const Base& ) ; ///< assignement operator is disabled 
    protected:
      /// execute the code 
      StatusCode executeCode () ;
    private: 
      bool                     m_pyInit  ;
      // flag to displaythe prepared code 
      bool                     m_showCode ; ///< flag to displa ythe prepared code 
    } ;  
  } // end of namespace LoKi::Hybrid 
} // end of namespace LoKi 
// ============================================================================
// helper method to decrease the typing
// ============================================================================
template <class TYPE>
inline void LoKi::Hybrid::Base::_set 
( TYPE*& local , const TYPE& right ) 
{ 
  if ( 0 != local ) 
  { 
    if ( msgLevel ( MSG::DEBUG ) ) 
    { Warning ( "setCut/Fun(): Existing 'Function/Predicate' is substituted !" ).ignore() ; } ;
    delete local;
    local = 0 ;
  }
  // clone it! 
  local = right.clone() ;
  // debug printput:
  if ( msgLevel ( MSG::DEBUG ) ) 
  {
    debug() << "The 'cut' is set to be '" << (*local) << "' = '"
            << System::typeinfoName( typeid( *local) ) << endmsg  ;
  } ;
  //
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKIHYBRID_HYBRIDTOOL_H
// ============================================================================

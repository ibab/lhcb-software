// $Id$
// ============================================================================
#ifndef LOKI_PARTICLES42_H 
#define LOKI_PARTICLES42_H 1
// ============================================================================
// Include files
// ============================================================================
// DaVinciInterfaces
// ============================================================================
#include "Kernel/ITriggerTisTos.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/PhysTypes.h"
// ============================================================================
/** @file LoKi/Particles42.h
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *   By usage of this code one clearly states the disagreement 
 *    with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-06-06
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class TisTosTobDec LoKi/Particles42.h
     *  
     *  Simple wrapper for the famous TisTos-tool 
     *
     *  @see LoKi::Cuts::TISTOSTOBDEC
     *  @see ITisTos::TisTosTob::value
     *  @see ITisTos::TisTosTob
     *  @see ITriggerTisTos
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2012-06-08
     */
    class GAUDI_API TisTosTobDec 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function
    {
    public: 
      // ======================================================================
      /// constructor from the lines and the tool 
      TisTosTobDec
      ( const std::string&    lines  ,
        const ITriggerTisTos* tistos ) ;
      /// MANDATORY : virtual destructor 
      virtual ~TisTosTobDec () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TisTosTobDec* clone() const ;
      /// MANDATORY: the only one important method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// protected  constructor without tool 
      TisTosTobDec ( const std::string&     lines  ) ;
      /// define/reset the tool
      void setTool ( const ITriggerTisTos*  tool   ) ;
      // ======================================================================
    public:
      // ======================================================================
      /// the major method 
      ITisTos::TisTosTob tistos ( const LHCb::Particle* p ) const ;
      /// the lines 
      const std::string& lines() const { return m_lines ; }    
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TisTosTobDec ( ) ;                  // the default constructo is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// ther trigger lines to be tested  
      std::string                     m_lines ;            // the trigger lines  
      /// the tool itself 
      LoKi::Interface<ITriggerTisTos> m_tistos ;             // the tool itself 
      // ======================================================================
    };
    // ========================================================================
    /** @class TisTosTobbing LoKi/Particles42.h
     *  
     *  Simple wrapper for the famous TisTos-tool 
     *
     *  @see LoKi::Cuts::TISTOSTOB
     *  @see ITisTos::TisTosTob::value
     *  @see ITisTos::TisTosTob
     *  @see ITriggerTisTos
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2012-06-08
     */
    class GAUDI_API TisTosTobbing : public LoKi::Particles::TisTosTobDec 
    {
    public: 
      // ======================================================================
      /// constructor from the lines and the tool 
      TisTosTobbing
      ( const std::string&    lines ,
        const std::string&    tool   ) ;
      /// MANDATORY : virtual destructor 
      virtual ~TisTosTobbing () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TisTosTobbing* clone() const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      TisTosTobbing ( ) ;               // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// the tool name 
      const std::string& toolName () const { return m_nick ; }    
      // ======================================================================
    private:
      // ======================================================================
      /// the tool name 
      std::string m_nick ;                                    // the tool name 
      // ======================================================================
    };
    // ========================================================================
    /** @class Tis LoKi/Particles42.h
     *  
     *  Simple wrapper for the famous TisTos-tool to egt easy access to TIS 
     *  @see LoKi::Cuts::TIS 
     *  @see ITisTos::TisTosTob::tis
     *  @see ITisTos::TisTosTob
     *  @see ITriggerTisTos
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2012-06-08
     */
    class GAUDI_API Tis 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate
    {
    public: 
      // ======================================================================
      /// constructor from the lines and the tool 
      Tis
      ( const std::string&    lines ,
        const std::string&    tool   ) ;
      /// MANDATORY : virtual destructor 
      virtual ~Tis () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Tis* clone() const ;
      /// MANDATORY: the only one important method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// the major method 
      ITisTos::TisTosTob tistos ( const LHCb::Particle* p ) const 
      { return  m_fun.tistos ( p ) ; }
      /// the tool name 
      const std::string& toolName () const { return m_fun.toolName () ; }    
      /// the lines 
      const std::string& lines    () const { return m_fun.lines    () ; }    
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Tis ( ) ;                         // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual functor 
      TisTosTobbing m_fun ;                               // the actual functor 
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class Tos LoKi/Particles42.h
     *  
     *  Simple wrapper for the famous TisTos-tool to egt easy access to TOS
     *  @see LoKi::Cuts::TOS 
     *  @see ITisTos::TisTosTob::tos
     *  @see ITisTos::TisTosTob
     *  @see ITriggerTisTos
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2012-06-08
     */
    class GAUDI_API Tos : public LoKi::Particles::Tis 
    {
    public: 
      // ======================================================================
      /// constructor from the lines and the tool 
      Tos
      ( const std::string&    lines ,
        const std::string&    tool   ) ;
      /// MANDATORY : virtual destructor 
      virtual ~Tos () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Tos* clone() const ;
      /// MANDATORY: the only one important method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Tos () ;                           // the default constructor is disabled 
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class Tps LoKi/Particles42.h
     *  
     *  Simple wrapper for the famous TisTos-tool to egt easy access to TPS
     *  @see LoKi::Cuts::TPS 
     *  @see ITisTos::TisTosTob::tps
     *  @see ITisTos::TisTosTob
     *  @see ITriggerTisTos
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2012-06-08
     */
    class GAUDI_API Tps : public LoKi::Particles::Tos 
    {
    public: 
      // ======================================================================
      /// constructor from the lines and the tool 
      Tps
      ( const std::string&    lines ,
        const std::string&    tool   ) ;
      /// MANDATORY : virtual destructor 
      virtual ~Tps () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Tps* clone() const ;
      /// MANDATORY: the only one important method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Tps () ;                           // the default constructor is disabled 
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class Tus LoKi/Particles42.h
     *  
     *  Simple wrapper for the famous TisTos-tool to egt easy access to TUS
     *  @see LoKi::Cuts::TUS 
     *  @see ITisTos::TisTosTob::tps
     *  @see ITisTos::TisTosTob
     *  @see ITriggerTisTos
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2012-06-08
     */
    class GAUDI_API Tus : public LoKi::Particles::Tps 
    {
    public: 
      // ======================================================================
      /// constructor from the lines and the tool 
      Tus
      ( const std::string&    lines ,
        const std::string&    tool   ) ;
      /// MANDATORY : virtual destructor 
      virtual ~Tus () ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Tus* clone() const ;
      /// MANDATORY: the only one important method 
      virtual  result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Tus () ;                           // the default constructor is disabled 
      // ======================================================================
    } ;  
    // ========================================================================
  } //                                         end of namespace LoKi::Particles 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef TISTOSTOBDEC 
     *  accessor to ITisTos::TisTosTob
     *  @code 
     * 
     *  ITriggerTisTos* tool = ... ;
     * 
     *  const  TISTOSTOBDEC fun = TISTOSTOBDEC ( "L0MuonDecision" , tool ) ;
     *  
     *  const LHCb::Particle* p = ... ; 
     *  const double value = fun ( p) ;
     * 
     *  ITisTos::TisTosTob ttt = ITisTos::TisTosTob ( int( value ) ) ;
     *
     *  @endcode 
     *  @see LoKi::Particles::TisTosTobDec
     *  @see ITisTos::TisTosTob
     *  @see ITriggerTisTos
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-06-07
     */
    typedef LoKi::Particles::TisTosTobDec                        TISTOSTOBDEC ;
    // ========================================================================
    /** @typedef TISTOSTOB
     *  accessor to ITisTos::TisTosTob
     *  @code 
     * 
     *  const  TISTOSTOB fun = TISTOSTOBDEC ( "L0MuonDecision" , tool ) ;
     *  
     *  const LHCb::Particle* p = ... ; 
     *  const double value = fun ( p) ;
     * 
     *  ITisTos::TisTosTob ttt = ITisTos::TisTosTob ( int( value ) ) ;
     *
     *  @endcode 
     *  @see LoKi::Particles::TisTosTobbing
     *  @see ITisTos::TisTosTob
     *  @see ITriggerTisTos
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-06-07
     */
    typedef LoKi::Particles::TisTosTobbing                          TISTOSTOB ;
    // ========================================================================
    /** @typedef TIS
     *  accessor to ITisTos::TisTosTob
     *  @code 
     * 
     *  const  TIS pred = TIS ( "L0MuonDecision" , tool ) ;
     *  
     *  const LHCb::Particle* p = ... ; 
     *  const bool tis = pred ( p) ;
     * 
     *  @endcode 
     *  @see LoKi::Particles::Tis
     *  @see ITisTos::TisTosTob
     *  @see ITriggerTisTos
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-06-07
     */
    typedef LoKi::Particles::Tis                                          TIS ;
    // ========================================================================
    /** @typedef TOS
     *  accessor to ITisTos::TisTosTob
     *  @code 
     * 
     *  const  TOS pred = TOS ( "L0MuonDecision" , tool ) ;
     *  
     *  const LHCb::Particle* p = ... ; 
     *  const bool tos = pred ( p) ;
     * 
     *  @endcode 
     *  @see LoKi::Particles::Tos
     *  @see ITisTos::TisTosTob
     *  @see ITriggerTisTos
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-06-07
     */
    typedef LoKi::Particles::Tos                                          TOS ;
    // ========================================================================
    /** @typedef TPS
     *  accessor to ITisTos::TisTosTob
     *  @code 
     * 
     *  const  TPS pred = TPS ( "L0MuonDecision" , tool ) ;
     *  
     *  const LHCb::Particle* p = ... ; 
     *  const bool tps = pred ( p) ;
     * 
     *  @endcode 
     *  @see LoKi::Particles::Tps
     *  @see ITisTos::TisTosTob
     *  @see ITriggerTisTos
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-06-07
     */
    typedef LoKi::Particles::Tps                                          TPS ;
    // ========================================================================
    /** @typedef TUS
     *  accessor to ITisTos::TisTosTob
     *  @code 
     * 
     *  const  TUS pred = TUS ( "L0MuonDecision" , tool ) ;
     *  
     *  const LHCb::Particle* p = ... ; 
     *  const bool tus = pred ( p) ;
     * 
     *  @endcode 
     *  @see LoKi::Particles::Tus
     *  @see ITisTos::TisTosTob
     *  @see ITriggerTisTos
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-06-07
     */
    typedef LoKi::Particles::Tus                                          TUS ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PARTICLES42_H
// ============================================================================

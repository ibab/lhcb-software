// ============================================================================
#ifndef LOKI_ALGFUNCTORS_H 
#define LOKI_ALGFUNCTORS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <vector>
// ============================================================================
// Gaudi Kernel 
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Interface.h"
// ============================================================================
/** @file 
 *
 *  Collection of various functors which test the status of "algorithm".
 *  The idea comes form Gerhard "The Great" Raven 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @see IAlgorithm
 *  @see  Algorithm
 *  @see IAlgManager
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-010-14 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  /** @namespace LoKi::Algortithms       
   *
   *  Collection of various functors which test the status of "algorithm".
   *  The idea comes form Gerhard "The Great" Raven 
   *  @see LoKi::Cuts::ALG_PASSED 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-010-14 
   */
  namespace Algorithms
  {
    // =========================================================================
    /** @class Passed 
     *  simple check of the certain algorithm to pass the filter  
     *  
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-14
     */
    // =========================================================================
    class GAUDI_API Passed : public LoKi::BasicFunctors<void>::Predicate 
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      Passed ( const std::string& name ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Passed () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Passed* clone() const { return new Passed ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// set the algorithm 
      const LoKi::Interface<IAlgorithm>& setAlgorithm 
      ( const IAlgorithm* a ) const ;                      // set the algorithm 
      /// get the algorithm 
      const LoKi::Interface<IAlgorithm>& algorithm () const 
      { return m_algorithm ; }                             // get the algorithm 
      /// get the algorithm name 
      const std::string& algName () const { return m_name ; }
      /// get the algorithm 
      const LoKi::Interface<IAlgorithm>& 
      getAlgorithm ( const std::string& name ) const ; // get the algorithm 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Passed () ;                        // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the algorithm name 
      std::string                         m_name      ;
      /// the algorithm itself 
      mutable LoKi::Interface<IAlgorithm> m_algorithm ;
      // ======================================================================
    } ;    
    // =========================================================================
    /** @class Enabled 
     *  simple check oif the algorithm is enabled 
     *  @see LoKi::Cuts::ALG_ENABLED 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-14
     */
    // =========================================================================
    class GAUDI_API Enabled : public Passed 
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      Enabled ( const std::string& name ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Enabled () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Enabled* clone() const { return new Enabled ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Enabled () ;                       // the default constructor is disabled 
      // ======================================================================
    } ;
    // =========================================================================
    /** @class Executed
     *  simple check if the algorithm is enabled 
     *  @see LoKi::Cuts::ALG_EXECUTED
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-14
     */
    // =========================================================================
    class GAUDI_API Executed : public Passed 
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      Executed ( const std::string& name ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Executed () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Executed* clone() const { return new Executed ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Executed () ;                       // the default constructor is disabled 
      // ======================================================================
    } ;
    // =========================================================================
    /** @class Run
     *  A bit complicated action:
     *   - algorithm must be enabled, SUCCESSFULLY executed and passed 
     *   - if not executed, execuye it 
     *  @see LoKi::Cuts::ALG_RUN
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-14
     */
    // =========================================================================
    class GAUDI_API Run : public Passed 
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      Run ( const std::string& name ) ;
      /// MANDATORY: virtual destructor 
      virtual ~Run () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  Run* clone() const { return new Run ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Run () ;                          // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class AnyPassed 
     *  simple check of the certain algorithm to pass the filter  
     *  @see LoKi::Cuts::ALG_ANYPASSED
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-14
     */
    // =========================================================================
    class GAUDI_API AnyPassed : public LoKi::BasicFunctors<void>::Predicate 
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      AnyPassed ( const std::string& name1 , 
                  const std::string& name2 ) ;
      AnyPassed ( const std::string& name1 , 
                  const std::string& name2 ,
                  const std::string& name3 ) ;
      AnyPassed ( const std::string& name1 , 
                  const std::string& name2 ,
                  const std::string& name3 ,
                  const std::string& name4 ) ;
      AnyPassed ( const std::vector<std::string>& name ) ;
      /// MANDATORY: virtual destructor 
      virtual ~AnyPassed () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AnyPassed* clone() const { return new AnyPassed ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return print ( "ALG_ANYPASSED" , s ) ; } 
      // ======================================================================
    protected:
      // ======================================================================
      /// the actual type of vector of algorithms 
      bool empty() const { return m_algorithms.empty () ; }
      /// get the algorithm name 
      const std::string& algName ( const size_t i ) const { return m_names[i] ; }
      /// get the algorithm
      LoKi::Interface<IAlgorithm>
      getAlgorithm ( const std::string& name ) const ; // get the algorithm 
      // ======================================================================
    public:
      // ======================================================================
      virtual std::ostream& print 
      ( const std::string&  name , 
        std::ostream&       s    ) const ;
      // ======================================================================
    public:
      // ======================================================================
      typedef std::vector< LoKi::Interface<IAlgorithm> > Algorithms ;
      // ======================================================================
      Algorithms::const_iterator begin () const 
      { return m_algorithms.begin () ; }
      Algorithms::const_iterator end   () const
      { return m_algorithms.end   () ; }
      /// get the algorithms 
      const Algorithms& algorithms () const { return m_algorithms ; } 
      /// get the algorithm name 
      const std::vector<std::string>& algNames () const { return m_names ; }
      /// get the algorithms
      void getAlgorithms () const ; // get the algorithms 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      AnyPassed () ;                     // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the algorithm names 
      std::vector<std::string> m_names ;               // the algorithm names 
      /// the algorithms themself 
      mutable Algorithms m_algorithms ;
      // ======================================================================
    } ;    
    // ========================================================================
    /** @class AllPassed 
     *  simple check of the certain algorithm to pass the filter  
     *  @see LoKi::Cuts::ALG_ALLPASSED 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-14
     */
    // =========================================================================
    class GAUDI_API AllPassed : public AnyPassed 
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      AllPassed ( const std::string& name1 , 
                  const std::string& name2 ) 
        : AnyPassed ( name1 , name2 ) {}
      /// constructor from the algorithm name      
      AllPassed ( const std::string& name1 , 
                  const std::string& name2 ,
                  const std::string& name3 ) 
        : AnyPassed ( name1 , name2 , name3 ) {}
      /// constructor from the algorithm name      
      AllPassed ( const std::string& name1 , 
                  const std::string& name2 ,
                  const std::string& name3 ,
                  const std::string& name4 ) 
        : AnyPassed ( name1 , name2 , name3 , name4 ) {}
      /// constructor from the algorithm name      
      AllPassed ( const std::vector<std::string>& name ) 
        : AnyPassed ( name ) {}
      /// MANDATORY: virtual destructor 
      virtual ~AllPassed ()  {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AllPassed* clone() const { return new AllPassed ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return print ( "ALG_ANYPASSED" , s ) ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      AllPassed () ;                        // the default constructor is disabled 
      // ======================================================================
    } ;    
    // ========================================================================
    /** @class AnyEnabled
     *  simple check of the certain algorithm to pass the filter  
     *  @see LoKi::Cuts::ALG_ANYENABLED
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-14
     */
    // =========================================================================
    class GAUDI_API AnyEnabled : public AnyPassed 
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      AnyEnabled ( const std::string& name1 , 
                   const std::string& name2 ) 
        : AnyPassed ( name1 , name2 ) {}
      /// constructor from the algorithm name 
      AnyEnabled ( const std::string& name1 , 
                   const std::string& name2 ,
                   const std::string& name3 ) 
        : AnyPassed ( name1 , name2 , name3 ) {}
      /// constructor from the algorithm name 
      AnyEnabled ( const std::string& name1 , 
                   const std::string& name2 ,
                   const std::string& name3 ,
                   const std::string& name4 ) 
        : AnyPassed ( name1 , name2 , name3 , name4 ) {}
      /// constructor from the algorithm name 
      AnyEnabled ( const std::vector<std::string>& name ) 
        : AnyPassed ( name ) {}
      /// MANDATORY: virtual destructor 
      virtual ~AnyEnabled () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AnyEnabled* clone() const { return new AnyEnabled ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return print ( "ALG_ANYENABLED" , s ) ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      AnyEnabled () ;                        // the default constructor is disabled 
      // ======================================================================
    } ;    
    // ========================================================================
    /** @class AllEnabled
     *  simple check of the certain algorithm to pass the filter  
     *  @see LoKi::Cuts::ALG_ALLENABLED
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-14
     */
    // =========================================================================
    class GAUDI_API AllEnabled : public AnyPassed 
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      AllEnabled ( const std::string& name1 , 
                   const std::string& name2 ) 
        : AnyPassed ( name1 , name2 ) {}
      /// constructor from the algorithm name 
      AllEnabled ( const std::string& name1 , 
                   const std::string& name2 ,
                   const std::string& name3 ) 
        : AnyPassed ( name1 , name2 , name3 ) {}
      /// constructor from the algorithm name 
      AllEnabled ( const std::string& name1 , 
                   const std::string& name2 ,
                   const std::string& name3 ,
                   const std::string& name4 ) 
        : AnyPassed ( name1 , name2 , name3 , name4 ) {}
      /// constructor from the algorithm name 
      AllEnabled ( const std::vector<std::string>& name ) 
        : AnyPassed ( name ) {}
      /// MANDATORY: virtual destructor 
      virtual ~AllEnabled () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AllEnabled* clone() const { return new AllEnabled ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return print ( "ALG_ALLENABLED" , s ) ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      AllEnabled () ;                        // the default constructor is disabled 
      // ======================================================================
    } ;    
    // ========================================================================
    /** @class AnyExecuted
     *  simple check of the certain algorithm to pass the filter  
     *  @see LoKi::Cuts::ALG_ANYEXECUTED
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-14
     */
    // =========================================================================
    class GAUDI_API AnyExecuted : public AnyPassed 
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      AnyExecuted ( const std::string& name1 , 
                    const std::string& name2 ) 
        : AnyPassed ( name1 , name2 ) {}
      /// constructor from the algorithm name 
      AnyExecuted ( const std::string& name1 , 
                    const std::string& name2 ,
                    const std::string& name3 ) 
        : AnyPassed ( name1 , name2 , name3 ) {}
      /// constructor from the algorithm name 
      AnyExecuted ( const std::string& name1 , 
                    const std::string& name2 ,
                    const std::string& name3 ,
                    const std::string& name4 ) 
        : AnyPassed ( name1 , name2 , name3 , name4 ) {}
      /// constructor from the algorithm name 
      AnyExecuted ( const std::vector<std::string>& name ) 
        : AnyPassed ( name ) {}
      /// MANDATORY: virtual destructor 
      virtual ~AnyExecuted () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AnyExecuted* clone() const { return new AnyExecuted ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return print ( "ALG_ANYEXECUTED" , s ) ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      AnyExecuted () ;                        // the default constructor is disabled 
      // ======================================================================
    } ;    
    // ========================================================================
    /** @class AllExecuted
     *  simple check of the certain algorithm to pass the filter  
     *  @see LoKi::Cuts::ALG_ALLEXECUTED
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-14
     */
    // =========================================================================
    class GAUDI_API AllExecuted : public AnyPassed 
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      AllExecuted ( const std::string& name1 , 
                    const std::string& name2 ) 
        : AnyPassed ( name1 , name2 ) {}
      /// constructor from the algorithm name 
      AllExecuted ( const std::string& name1 , 
                    const std::string& name2 ,
                    const std::string& name3 ) 
        : AnyPassed ( name1 , name2 , name3 ) {}
      /// constructor from the algorithm name 
      AllExecuted ( const std::string& name1 , 
                    const std::string& name2 ,
                    const std::string& name3 ,
                    const std::string& name4 ) 
        : AnyPassed ( name1 , name2 , name3 , name4 ) {}
      /// constructor from the algorithm name 
      AllExecuted ( const std::vector<std::string>& name ) 
        : AnyPassed ( name ) {}
      /// MANDATORY: virtual destructor 
      virtual ~AllExecuted () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  AllExecuted* clone() const { return new AllExecuted ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return print ( "ALG_ALLEXECUTED" , s ) ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      AllExecuted () ;                        // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class RunAll
     *  @see LoKi::Cuts::ALG_RUNALL
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-14
     */
    // =========================================================================
    class GAUDI_API RunAll : public AllExecuted
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      RunAll ( const std::string& name1 , 
               const std::string& name2 ) ;
      /// constructor from the algorithm name 
      RunAll ( const std::string& name1 , 
               const std::string& name2 ,
               const std::string& name3 ) ;
      /// constructor from the algorithm name 
      RunAll ( const std::string& name1 , 
               const std::string& name2 ,
               const std::string& name3 ,
               const std::string& name4 ) ;
      /// constructor from the algorithm name 
      RunAll ( const std::vector<std::string>& name ) ;
      /// MANDATORY: virtual destructor 
      virtual ~RunAll () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RunAll* clone() const { return new RunAll ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return print ( "ALG_RUNALL" , s ) ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      RunAll () ;                        // the default constructor is disabled 
      // ======================================================================
    } ;
    // =========================================================================
    /** @class NumPassed 
     *  Simple functor to count number of passed algorithms from the list 
     *  @see LoKi::Cuts::ALG_NUMPASSED 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    class GAUDI_API NumPassed : public LoKi::BasicFunctors<void>::Function 
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      NumPassed ( const std::string& name1 , 
                  const std::string& name2 ) ;
      /// constructor from the algorithm name 
      NumPassed ( const std::string& name1 , 
                  const std::string& name2 ,
                  const std::string& name3 ) ;
      /// constructor from the algorithm name 
      NumPassed ( const std::string& name1 , 
                  const std::string& name2 ,
                  const std::string& name3 ,
                  const std::string& name4 ) ;
      /// constructor from the algorithm name 
      NumPassed ( const std::vector<std::string>& name ) ;
      // ======================================================================      
      /// MANDATORY: virtual destructor 
      virtual ~NumPassed () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  NumPassed* clone() const { return new NumPassed ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return print ( "ALG_NUMPASSED" , s ) ; } 
      // ======================================================================
    public:
      // ======================================================================
      virtual std::ostream& print 
      ( const std::string& name , 
        std::ostream& s ) const { return m_fun.print ( name , s ) ; }
      // ======================================================================
    public:
      // ======================================================================
      AnyPassed::Algorithms::const_iterator begin () const 
      { return m_fun.begin () ; }
      AnyPassed::Algorithms::const_iterator end   () const
      { return m_fun.end   () ; }
      /// get the algorithms 
      const AnyPassed::Algorithms& algorithms () const 
      { return m_fun.algorithms() ; } 
      /// get the algorithm name 
      const std::vector<std::string>& algNames () const 
      { return m_fun.algNames() ; }
      /// get the algorithms
      void getAlgorithms () const { m_fun.getAlgorithms () ; }
      // =======================================================================
    private:
      // =======================================================================
      /// the default constructor is disabled 
      NumPassed () ;                      // the default constructor is disabled 
      // =======================================================================
    private:
      // =======================================================================
      /// the actual functor 
      LoKi::Algorithms::AnyPassed m_fun ;                  // the actual functor
      // =======================================================================       
    } ;
    // =========================================================================
    /** @class NumEnabled 
     *  Simple functor to count number of enabled algorithms from the list 
     *  @see LoKi::Cuts::ALG_NUMENABLED 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    class GAUDI_API NumEnabled  : public NumPassed 
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      NumEnabled ( const std::string& name1 , 
                   const std::string& name2 ) 
        : NumPassed ( name1 , name2 ) {}
      /// constructor from the algorithm name 
      NumEnabled ( const std::string& name1 , 
                   const std::string& name2 ,
                   const std::string& name3 ) 
        : NumPassed ( name1 , name2 , name3 ) {}
      /// constructor from the algorithm name 
      NumEnabled ( const std::string& name1 , 
                   const std::string& name2 ,
                   const std::string& name3 ,
                   const std::string& name4 ) 
        : NumPassed ( name1 , name2 , name3 , name4 ) {}
      /// constructor from the algorithm name 
      NumEnabled ( const std::vector<std::string>& name ) 
        : NumPassed ( name ) {}
      // ======================================================================      
      /// MANDATORY: virtual destructor 
      virtual ~NumEnabled () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  NumEnabled* clone() const { return new NumEnabled ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return print ( "ALG_NUMENABLED" , s ) ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      NumEnabled () ;                   // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class NumExecuted
     *  Simple functor to count number of executed algorithms from the list 
     *  @see LoKi::Cuts::ALG_NUMEXECUTED 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-10-19
     */
    class GAUDI_API NumExecuted  : public NumPassed 
    {
    public:
      // ======================================================================
      /// constructor from the algorithm name 
      NumExecuted ( const std::string& name1 , 
                    const std::string& name2 ) 
        : NumPassed ( name1 , name2 ) {}
      /// constructor from the algorithm name 
      NumExecuted ( const std::string& name1 , 
                    const std::string& name2 ,
                    const std::string& name3 ) 
        : NumPassed ( name1 , name2 , name3 ) {}
      /// constructor from the algorithm name 
      NumExecuted ( const std::string& name1 , 
                    const std::string& name2 ,
                    const std::string& name3 ,
                    const std::string& name4 ) 
        : NumPassed ( name1 , name2 , name3 , name4 ) {}
      /// constructor from the algorithm name 
      NumExecuted ( const std::vector<std::string>& name ) 
        : NumPassed ( name ) {}
      // ======================================================================      
      /// MANDATORY: virtual destructor 
      virtual ~NumExecuted () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  NumExecuted* clone() const { return new NumExecuted ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() () const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const 
      { return print ( "ALG_NUMEXECUTED" , s ) ; } 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      NumExecuted () ;                   // the default constructor is disabled 
      // ======================================================================
    } ;
    // ========================================================================
  } //                                        end of namespace LoKi::Algorithms
  // ==========================================================================
} //                                                     end of namaespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_ALGFUNCTORS_H
// ============================================================================

// $Id:$
// ============================================================================
// Include files 
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/NBodyCompare.h"
// ============================================================================
// local 
// ============================================================================
#include "CombineParticles.h"
#include "Counters.h"
// ============================================================================
/** @file
 *  Attempt to make more fine instumentation of generic 
 *  CombineParticle algorithm for multibody decays.
 *  The new functionality allows to provide the cuts for 
 *  incomplete combinations. Being used smart way, 
 *  this functionality could allow some gain in CPU perfomance.
 *  For N-body decays the follwoing new properties are added :
 *  - <code>Combination12Cut</code>
 *  - <code>Combination123Cut</code>
 *  - ... up to <code>Combination123...(N-1)Cut</code>
 *  These cuts are applied during the explicit loops 
 *  (it is bad, but generic looper here make no gain...)
 *  Honestly speaking the implementation is a bit ugly, 
 *  but I've seen even less nice lines in LHCb software
 *
 *  These new properties are mandatory, and of course 
 *  they makes the configuration more complicated, but 
 *  they really could imporve CPU performance.
 *
 *  @attention  Please be sure what you are actually doing, if use these algorithms 
 *
 *  @code 
 *  from Configurables import DaVinci__N4BodyDecays as ALGO
 *  alg = ALGO ( 
 *    'MyDecay' , 
 *    DecayDescriptor = "D0 -> pi+ pi+ pi- pi-" , 
 *    Combination12Cut  = "ACHI2DOCA(1,2)<16"                       , ## new one 
 *    Combination123Cut = "(ACHI2DOCA(1,3)<16)&(ACHI2DOCA(2,3)<16)" , ## new one 
 *    CombinationCut    = "(ACHI2DOCA(1,4)<16)&(ACHI2DOCA(2,4)<16)&(ACHI2DOCA(3,4)<16)" , 
 *    MotherCut         = "..." )
 *  @endcode 
 *
 *  @see DaVinci::N3BodyDecay
 *  @see DaVinci::N4BodyDecay
 *  @see DaVinci::N5BodyDecay
 *  @see DaVinci::N6BodyDecay
 *  @see DaVinci::N7BodyDecay
 *  @see DaVinci::N8BodyDecay
 *
 *
 *  I've tested a bit artificial (in terms of numerical values for 
 *  the cuts, clearly chi2(DOCA)<9 is much better than chi2(DOCA)<25) 
 *  but realistic (in terms of decay structure and variables to cut on) 
 *  configurations:
 *
 *  @code 
 *
 *  ## NEW ONE 
 *
 *  from GaudiConfUtils.ConfigurableGenerators import DaVinci__N5BodyDecays
 *  alg_5n = DaVinci__N5BodyDecays (
 *      ##
 *      DecayDescriptors = [
 *      "[ B_s0 -> J/psi(1S) K+ pi+ pi- pi- ]cc" ,
 *      "  B_s0 -> J/psi(1S) K+ pi+ K-  pi-    " ,
 *      "[ B_s0 -> J/psi(1S) K+ K+  K-  pi- ]cc"
 *      ] ,
 *      ##
 *      Preambulo = ... , 
 *      ##
 *      Combination12Cut  = """
 *      ( AM<5.550 * GeV    ) &
 *      ( ACHI2DOCA(1,2)<25 )
 *      """,
 *      Combination123Cut = """
 *      ( AM<5.550 * GeV    ) &
 *      ( ACHI2DOCA(1,3)<25 ) & 
 *      ( ACHI2DOCA(2,3)<25 ) 
 *      """ ,
 *      Combination1234Cut = """
 *      ( AM<5.550 * GeV   ) &
 *      ( ACHI2DOCA(1,4)<25) & 
 *      ( ACHI2DOCA(2,4)<25) &
 *      ( ACHI2DOCA(3,4)<25) 
 *      """ ,
 *      CombinationCut   = """
 *      mb0_acut & 
 *      ( ACHI2DOCA(1,5)<25) & 
 *      ( ACHI2DOCA(2,5)<25) &
 *      ( ACHI2DOCA(3,5)<25) &
 *      ( ACHI2DOCA(4,5)<25) 
 *      """ ,
 *      ## 
 *      MotherCut = "..." 
 *      ## 
 *      )
 *
 *  ## OLD ONE:
 *  from GaudiConfUtils.ConfigurableGenerators import CombineParticles
 *  alg_5 = CombineParticles (
 *      ##
 *      DecayDescriptors = [
 *          "[ B_s0 -> J/psi(1S) K+ pi+ pi- pi- ]cc" ,
 *          "  B_s0 -> J/psi(1S) K+ pi+ K-  pi-    " ,
 *          "[ B_s0 -> J/psi(1S) K+ K+  K-  pi- ]cc"
 *      ] ,
 *      ##
 *      Preambulo = ... ,  
 *      ##
 *      CombinationCut = """
 *      mb0_acut & 
 *      ADOCACHI2CUT ( 25 , '' )
 *      """ ,
 *      ## 
 *      MotherCut = ... 
 *   )
 *
 *  @endcode 
 *
 *  The configurations have been compared with the DIMUON-stream,
 *  using <code>FullDSTDiMuonJpsi2MuMuDetachedLine</code> stripping 
 *  line and <code>StdLooseKaons</code> and <code>StdLoosePions</code>
 *
 *  The performance of algorithms for 5000 events is 
 *   - 269s for standard CombineParticles algorithm 
 *   - 2-4s for N5BodyDecays 
 *
 *  @author Vanya BELYAEV  Ivan.Belyaev@itep.ru
 *  @date 2013-10-06
 *
 *                    $Revision:$
 *  Last modification $Date:$
 *                 by $Author:$
 */
namespace DaVinci 
{
  // ==========================================================================
  /** @class N3BodyDecays
   *  Specialization of CombineParitcle algorithm 
   *  for more effective treatment of 3-body decays 
   *
   *  It adds new (mandatory) property  <code>Combination12Cut</code>
   *  that is applied for combination of the 1st and 2nd daughters 
   *
   *  @code 
   *  from Configurables import DaVinci__N3BodyDecays as ALGO
   *  alg = ALGO ( 
   *    'MyDecay' , 
   *    DecayDescriptor = "D0 -> pi+ pi- KS0" , 
   *    Combination12Cut  = "ACHI2DOCA(1,2)<16"  , ## new one 
   *    CombinationCut    = "..." , 
   *    MotherCut         = "..." )
   *  @endcode 
   *
   *  @attention: note the order of particles
   *
   *  @attention  Please be sure what you are actually doing, if use these algorithms 
   *
   *
   *  @author Vanya Belyaev
   *  @date   2013-10-06
   *
   *                    $Revision:$
   *  Last modification $Date:$
   *                 by $Author:$
   */
  class N3BodyDecays : public CombineParticles 
  {
    // ========================================================================
    // the friend factory, needed for instantiation
    friend class AlgFactory<N3BodyDecays> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the standard initialization of the algorithm
    virtual StatusCode initialize () ;                 // standard initialization
    /// the standard execution      of the algorithm
    virtual StatusCode execute    () ;                 //      standard execution
    // ========================================================================
  protected:
    // ========================================================================
    /** the standard constructor
     *  @param name algorithm instance name
     *  @param pSvc the service locator
     */
    N3BodyDecays
    ( const std::string& name ,   // the algorithm instance name
      ISvcLocator*       pSvc ) ; // the service locator
    /// virtual and protected destrcutor
    virtual ~N3BodyDecays () {} // virtual and protected destrcutor
    // ========================================================================
  protected: 
    // ========================================================================
    /// the handle for the property update
    void propertyHandler3 ( Property& p ) { propertyHandler1 ( p ) ; }
    /// decode the specific information 
    virtual StatusCode decodeRest    ( LoKi::IHybridFactory* factory ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    N3BodyDecays () ;                            // no default constructor!
    /// the copy constructor is disabled
    N3BodyDecays ( const N3BodyDecays& ) ;      // no copy constructor!
    /// the assignement is disabled
    N3BodyDecays& operator=( const N3BodyDecays& ) ; // no assignement!
    // ========================================================================
  protected:
    // ========================================================================
    /// the cut to be applied for combination of 1st and 2nd 
    std::string m_combination12Cut ; // the cut to be applied for 1st & 2nd 
    /// the actual cut for combination of 1st and 2nd particles 
    LoKi::Types::ACut m_acut12 ; // the actual cut for combination of 1st&2nd 
    // ========================================================================
  };
  // ==========================================================================
  /** @class N4BodyDecays
   *  Specialization of CombineParitcle algorithm 
   *  for more effective treatment of 4-body decays 
   *
   *  It adds new (mandatory) properties  
   *  - <code>Combination12Cut</code>
   *  - <code>Combination123Cut</code>
   *  that is applied for combination of the 1st and 2nd daughters
   *  and 1st, 2nd and 3rd daughters, respectively 
   *
   *  @code 
   *  from Configurables import DaVinci__N4BodyDecays as ALGO
   *  alg = ALGO ( 
   *    'MyDecay' , 
   *    DecayDescriptor = "D0 -> pi+ pi+ pi- pi-" , 
   *    Combination12Cut  = "ACHI2DOCA(1,2)<16"                       , ## new one 
   *    Combination123Cut = "(ACHI2DOCA(1,3)<16)&(ACHI2DOCA(2,3)<16)" , ## new one 
   *    CombinationCut    = "(ACHI2DOCA(1,4)<16)&(ACHI2DOCA(2,4)<16)&(ACHI2DOCA(3,4)<16)" , 
   *    MotherCut         = "..." )
   *  @endcode 
   *
   *  @author Vanya Belyaev
   *  @date   2013-10-06
   *
   *  @attention  Please be sure what you are actually doing, if use these algorithms 
   *
   *                    $Revision:$
   *  Last modification $Date:$
   *                 by $Author:$
   */
  class N4BodyDecays : public N3BodyDecays
  {
    // ========================================================================
    // the friend factory, needed for instantiation
    friend class AlgFactory<N4BodyDecays> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the standard initialization of the algorithm
    virtual StatusCode initialize () ;                 // standard initialization
    /// the standard execution      of the algorithm
    virtual StatusCode execute    () ;                 //      standard execution
    // ========================================================================
  protected:
    // ========================================================================
    /** the standard constructor
     *  @param name algorithm instance name
     *  @param pSvc the service locator
     */
    N4BodyDecays
    ( const std::string& name ,   // the algorithm instance name
      ISvcLocator*       pSvc ) ; // the service locator
    /// virtual and protected destrcutor
    virtual ~N4BodyDecays () {} // virtual and protected destrcutor
    // ========================================================================
  protected: 
    // ========================================================================
    /// the handle for the property update
    void propertyHandler4 ( Property& p ) { propertyHandler3 ( p ) ; }
    /// decode the specific information 
    virtual StatusCode decodeRest    ( LoKi::IHybridFactory* factory ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    N4BodyDecays () ;                            // no default constructor!
    /// the copy constructor is disabled
    N4BodyDecays ( const N4BodyDecays& ) ;      // no copy constructor!
    /// the assignement is disabled
    N4BodyDecays& operator=( const N4BodyDecays& ) ; // no assignement!
    // ========================================================================
  protected:
    // ========================================================================
    /// the cut to be applied for combination of 1st,2nd and 3rd
    std::string m_combination123Cut ; // the cut to be applied for 1st,2nd&3rd
    /// the actual cut for combination of 1st and 2nd particles 
    LoKi::Types::ACut m_acut123 ; // the actual cut for combination of 1st,2nd&3rd
    // ========================================================================
  };
  // ==========================================================================
  /** @class N5BodyDecays
   *  Specialization of CombineParitcle algorithm 
   *  for more effective treatment of 5-body decays 
   *
   *  It adds new (mandatory) properties  
   *  - <code>Combination12Cut</code>
   *  - <code>Combination123Cut</code>
   *  - <code>Combination1234Cut</code>
   *
   *  @code 
   *  from Configurables import DaVinci__N5BodyDecays as ALGO
   *  alg = ALGO ( 
   *    'MyDecay' , 
   *    DecayDescriptor    = "B_s0 -> J/psi(1S) pi+ pi+ pi- pi-" , 
   *    Combination12Cut   = "ACHI2DOCA(1,2)<16"  , ## new one 
   *    Combination123Cut  = "(ACHI2DOCA(1,3)<16)&(ACHI2DOCA(2,3)<16)" , ## new one 
   *    Combination1234Cut = "(ACHI2DOCA(1,4)<16)&(ACHI2DOCA(2,4)<16)&(ACHI2DOCA(3,4)<16)" , 
   *    CombinationCut     = "..." , 
   *    MotherCut          = "..." )
   *  @endcode 
   *
   *  @author Vanya Belyaev
   *  @date   2013-10-06
   *
   *  @attention  Please be sure what you are actually doing, if use these algorithms 
   *
   *  I've tested a bit artificial (in terms of numerical values for 
   *  the cuts, clearly chi2(DOCA)<9 is much better than chi2(DOCA)<25) 
   *  but realistic (in terms of decay structure and variables to cut on) 
   *  configurations:
   *
   *  @code 
   *
   *  ## NEW ONE 
   *
   *  from GaudiConfUtils.ConfigurableGenerators import DaVinci__N5BodyDecays
   *  alg_5n = DaVinci__N5BodyDecays (
   *      ##
   *      DecayDescriptors = [
   *      "[ B_s0 -> J/psi(1S) K+ pi+ pi- pi- ]cc" ,
   *      "  B_s0 -> J/psi(1S) K+ pi+ K-  pi-    " ,
   *      "[ B_s0 -> J/psi(1S) K+ K+  K-  pi- ]cc"
   *      ] ,
   *      ##
   *      Preambulo = ... , 
   *      ##
   *      Combination12Cut  = """
   *      ( AM<5.550 * GeV    ) &
   *      ( ACHI2DOCA(1,2)<25 )
   *      """,
   *      Combination123Cut = """
   *      ( AM<5.550 * GeV    ) &
   *      ( ACHI2DOCA(1,3)<25 ) & 
   *      ( ACHI2DOCA(2,3)<25 ) 
   *      """ ,
   *      Combination1234Cut = """
   *      ( AM<5.550 * GeV   ) &
   *      ( ACHI2DOCA(1,4)<25) & 
   *      ( ACHI2DOCA(2,4)<25) &
   *      ( ACHI2DOCA(3,4)<25) 
   *      """ ,
   *      CombinationCut   = """
   *      mb0_acut & 
   *      ( ACHI2DOCA(1,5)<25) & 
   *      ( ACHI2DOCA(2,5)<25) &
   *      ( ACHI2DOCA(3,5)<25) &
   *      ( ACHI2DOCA(4,5)<25) 
   *      """ ,
   *      ## 
   *      MotherCut = "..." 
   *      ## 
   *      )
   *
   *  ## OLD ONE:
   *  from GaudiConfUtils.ConfigurableGenerators import CombineParticles
   *  alg_5 = CombineParticles (
   *      ##
   *      DecayDescriptors = [
   *          "[ B_s0 -> J/psi(1S) K+ pi+ pi- pi- ]cc" ,
   *          "  B_s0 -> J/psi(1S) K+ pi+ K-  pi-    " ,
   *          "[ B_s0 -> J/psi(1S) K+ K+  K-  pi- ]cc"
   *      ] ,
   *      ##
   *      Preambulo = ... ,  
   *      ##
   *      CombinationCut = """
   *      mb0_acut & 
   *      ADOCACHI2CUT ( 25 , '' )
   *      """ ,
   *      ## 
   *      MotherCut = ... 
   *   )
   *
   *  @endcode 
   *
   *  The configurations have been compared with the DIMUON-stream,
   *  using <code>FullDSTDiMuonJpsi2MuMuDetachedLine</code> stripping 
   *  line and <code>StdLooseKaons</code> and <code>StdLoosePions</code>
   *
   *  The performance of algorithms for 5000 events is 
   *   - 269s for standard CombineParticles algorithm 
   *   - 2-4s for N5BodyDecays 
   *
   *
   *
   *                    $Revision:$
   *  Last modification $Date:$
   *                 by $Author:$
   */
  class N5BodyDecays : public N4BodyDecays
  {
    // ========================================================================
    // the friend factory, needed for instantiation
    friend class AlgFactory<N5BodyDecays> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the standard initialization of the algorithm
    virtual StatusCode initialize () ;                 // standard initialization
    /// the standard execution      of the algorithm
    virtual StatusCode execute    () ;                 //      standard execution
    // ========================================================================
  protected:
    // ========================================================================
    /** the standard constructor
     *  @param name algorithm instance name
     *  @param pSvc the service locator
     */
    N5BodyDecays
    ( const std::string& name ,   // the algorithm instance name
      ISvcLocator*       pSvc ) ; // the service locator
    /// virtual and protected destrcutor
    virtual ~N5BodyDecays () {} // virtual and protected destrcutor
    // ========================================================================
  protected: 
    // ========================================================================
    /// the handle for the property update
    void propertyHandler5 ( Property& p ) { propertyHandler4 ( p ) ; }
    /// decode the specific information 
    virtual StatusCode decodeRest    ( LoKi::IHybridFactory* factory ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    N5BodyDecays () ;                            // no default constructor!
    /// the copy constructor is disabled
    N5BodyDecays ( const N5BodyDecays& ) ;      // no copy constructor!
    /// the assignement is disabled
    N5BodyDecays& operator=( const N5BodyDecays& ) ; // no assignement!
    // ========================================================================
  protected:
    // ========================================================================
    /// the cut to be applied for combination of 1st,2nd,3rd and 4th 
    std::string m_combination1234Cut ; // the cut to be applied for 1-4
    /// the actual cut for combination of 1st and 2nd particles 
    LoKi::Types::ACut m_acut1234 ; // the actual cut for combination of 1-4
    // ========================================================================
  };
  // ==========================================================================
  /** @class N6BodyDecays
   *  Specialization of CombineParitcle algorithm 
   *  for more effective treatment of 6-body decays 
   *
   *  It adds new (mandatory) properties  
   *  - <code>Combination12Cut</code>
   *  - <code>Combination123Cut</code>
   *  - <code>Combination1234Cut</code>
   *  - <code>Combination12345Cut</code>
   *
   *  @code 
   *  from Configurables import DaVinci__N6BodyDecays as ALGO
   *  alg = ALGO ( 
   *    'MyDecay' , 
   *    DecayDescriptor     = "B_c+ -> J/psi(1S) pi+ pi+ pi+ pi- pi-" , 
   *    Combination12Cut    = "ACHI2DOCA(1,2)<16"  , ## new one 
   *    Combination123Cut   = "(ACHI2DOCA(1,3)<16)&(ACHI2DOCA(2,3)<16)" , ## new one 
   *    Combination1234Cut  = "(ACHI2DOCA(1,4)<16)&(ACHI2DOCA(2,4)<16)&(ACHI2DOCA(3,4)<16)" , 
   *    Combination12345Cut = "... " , 
   *    CombinationCut      = "..." , 
   *    MotherCut           = "..." )
   *  @endcode 
   *
   *  @attention  Please be sure what you are actually doing, if use these algorithms 
   *
   *  @author Vanya Belyaev
   *  @date   2013-10-06
   *
   *                    $Revision:$
   *  Last modification $Date:$
   *                 by $Author:$
   */
  class N6BodyDecays : public N5BodyDecays
  {
    // ========================================================================
    // the friend factory, needed for instantiation
    friend class AlgFactory<N6BodyDecays> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the standard initialization of the algorithm
    virtual StatusCode initialize () ;                 // standard initialization
    /// the standard execution      of the algorithm
    virtual StatusCode execute    () ;                 //      standard execution
    // ========================================================================
  protected:
    // ========================================================================
    /** the standard constructor
     *  @param name algorithm instance name
     *  @param pSvc the service locator
     */
    N6BodyDecays
    ( const std::string& name ,   // the algorithm instance name
      ISvcLocator*       pSvc ) ; // the service locator
    /// virtual and protected destrcutor
    virtual ~N6BodyDecays () {} // virtual and protected destrcutor
    // ========================================================================
  protected: 
    // ========================================================================
    /// the handle for the property update
    void propertyHandler6 ( Property& p ) { propertyHandler5 ( p ) ; }
    /// decode the specific information 
    virtual StatusCode decodeRest    ( LoKi::IHybridFactory* factory ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    N6BodyDecays () ;                            // no default constructor!
    /// the copy constructor is disabled
    N6BodyDecays ( const N6BodyDecays& ) ;      // no copy constructor!
    /// the assignement is disabled
    N6BodyDecays& operator=( const N6BodyDecays& ) ; // no assignement!
    // ========================================================================
  protected:
    // ========================================================================
    /// the cut to be applied for combination of 1st,2nd,3rd,4th and 5th 
    std::string m_combination12345Cut ; // the cut to be applied for 1-5
    /// the actual cut for combination of 1st and 2nd particles 
    LoKi::Types::ACut m_acut12345 ; // the actual cut for combination of 1-5
    // ========================================================================
  };
  // ==========================================================================
  /** @class N7BodyDecays
   *  Specialization of CombineParitcle algorithm 
   *  for more effective treatment of 7-body decays 
   *
   *  It adds new (mandatory) properties  
   *  - <code>Combination12Cut</code>
   *  - <code>Combination123Cut</code>
   *  - <code>Combination1234Cut</code>
   *  - <code>Combination12345Cut</code>
   *  - <code>Combination123456Cut</code>
   *
   *  @code 
   *  from Configurables import DaVinci__N7BodyDecays as ALGO
   *  alg = ALGO ( 
   *    'MyDecay' , 
   *    DecayDescriptor      = "B_s0 -> J/psi(1S) pi+ pi+ pi+ pi- pi- pi-" , 
   *    Combination12Cut     = "ACHI2DOCA(1,2)<16"  , ## new one 
   *    Combination123Cut    = "(ACHI2DOCA(1,3)<16)&(ACHI2DOCA(2,3)<16)" , ## new one 
   *    Combination1234Cut   = "(ACHI2DOCA(1,4)<16)&(ACHI2DOCA(2,4)<16)&(ACHI2DOCA(3,4)<16)" , 
   *    Combination12345Cut  = "..." , 
   *    Combination123456Cut = "..." , 
   *    CombinationCut       = "..." , 
   *    MotherCut            = "..." )
   *  @endcode 
   *
   *  @author Vanya Belyaev
   *  @date   2013-10-06
   *
   *  @attention  Please be sure what you are actually doing, if use these algorithms 
   *
   *                    $Revision:$
   *  Last modification $Date:$
   *                 by $Author:$
   */
  class N7BodyDecays : public N6BodyDecays
  {
    // ========================================================================
    // the friend factory, needed for instantiation
    friend class AlgFactory<N7BodyDecays> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the standard initialization of the algorithm
    virtual StatusCode initialize () ;                 // standard initialization
    /// the standard execution      of the algorithm
    virtual StatusCode execute    () ;                 //      standard execution
    // ========================================================================
  protected:
    // ========================================================================
    /** the standard constructor
     *  @param name algorithm instance name
     *  @param pSvc the service locator
     */
    N7BodyDecays
    ( const std::string& name ,   // the algorithm instance name
      ISvcLocator*       pSvc ) ; // the service locator
    /// virtual and protected destrcutor
    virtual ~N7BodyDecays () {} // virtual and protected destrcutor
    // ========================================================================
  protected: 
    // ========================================================================
    /// the handle for the property update
    void propertyHandler7 ( Property& p ) { propertyHandler6 ( p ) ; }
    /// decode the specific information 
    virtual StatusCode decodeRest    ( LoKi::IHybridFactory* factory ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    N7BodyDecays () ;                            // no default constructor!
    /// the copy constructor is disabled
    N7BodyDecays ( const N7BodyDecays& ) ;      // no copy constructor!
    /// the assignement is disabled
    N7BodyDecays& operator=( const N7BodyDecays& ) ; // no assignement!
    // ========================================================================
  protected:
    // ========================================================================
    /// the cut to be applied for combination of 1st,2nd,3rd,4th,5th&6th 
    std::string m_combination123456Cut ; // the cut to be applied for 1-6
    /// the actual cut for combination of 1st and 2nd particles 
    LoKi::Types::ACut m_acut123456 ; // the actual cut for combination of 1-6
    // ========================================================================
  };
  // ==========================================================================
  /** @class N8BodyDecays
   *  Specialization of CombineParitcle algorithm 
   *  for more effective treatment of 7-body decays 
   *
   *  It adds new (mandatory) properties  
   *  - <code>Combination12Cut</code>
   *  - <code>Combination123Cut</code>
   *  - <code>Combination1234Cut</code>
   *  - <code>Combination12345Cut</code>
   *  - <code>Combination123456Cut</code>
   *  - <code>Combination1234567Cut</code>
   *
   *  @code 
   *  from Configurables import DaVinci__N8BodyDecays as ALGO
   *  alg = ALGO ( 
   *    'MyDecay' , 
   *    DecayDescriptor       = "B_c+ -> J/psi(1S) pi+ pi+ pi+ pi+ pi- pi- pi-" , 
   *    Combination12Cut      = "ACHI2DOCA(1,2)<16"  , ## new one 
   *    Combination123Cut     = "(ACHI2DOCA(1,3)<16)&(ACHI2DOCA(2,3)<16)" , ## new one 
   *    Combination1234Cut    = "(ACHI2DOCA(1,4)<16)&(ACHI2DOCA(2,4)<16)&(ACHI2DOCA(3,4)<16)" , 
   *    Combination12345Cut   = "..." , 
   *    Combination123456Cut  = "..." , 
   *    Combination1234567Cut = "..." , 
   *    CombinationCut        = "..." , 
   *    MotherCut             = "..." )
   *  @endcode 
   *
   *  @author Vanya Belyaev
   *  @date   2014-02-10
   *
   *  @attention  Please be sure what you are actually doing, if use these algorithms 
   *
   *                    $Revision:$
   *  Last modification $Date:$
   *                 by $Author:$
   */
  class N8BodyDecays : public N7BodyDecays
  {
    // ========================================================================
    // the friend factory, needed for instantiation
    friend class AlgFactory<N8BodyDecays> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the standard initialization of the algorithm
    virtual StatusCode initialize () ;                 // standard initialization
    /// the standard execution      of the algorithm
    virtual StatusCode execute    () ;                 //      standard execution
    // ========================================================================
  protected:
    // ========================================================================
    /** the standard constructor
     *  @param name algorithm instance name
     *  @param pSvc the service locator
     */
    N8BodyDecays
    ( const std::string& name ,   // the algorithm instance name
      ISvcLocator*       pSvc ) ; // the service locator
    /// virtual and protected destrcutor
    virtual ~N8BodyDecays () {} // virtual and protected destrcutor
    // ========================================================================
  protected: 
    // ========================================================================
    /// the handle for the property update
    void propertyHandler8 ( Property& p ) { propertyHandler7 ( p ) ; }
    /// decode the specific information 
    virtual StatusCode decodeRest    ( LoKi::IHybridFactory* factory ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled
    N8BodyDecays () ;                            // no default constructor!
    /// the copy constructor is disabled
    N8BodyDecays ( const N8BodyDecays& ) ;      // no copy constructor!
    /// the assignement is disabled
    N8BodyDecays& operator=( const N8BodyDecays& ) ; // no assignement!
    // ========================================================================
  protected:
    // ========================================================================
    /// the cut to be applied for combination of 1-7 
    std::string m_combination1234567Cut ; // the cut to be applied for 1-7
    /// the actual cut for combination of 1st and 2nd particles 
    LoKi::Types::ACut m_acut1234567 ; // the actual cut for combination of 1-7
    // ========================================================================
  };
  // ==========================================================================
} //                                               the end of namespace DaVinci
// ============================================================================
namespace 
{
  // ========================================================================== 
  /// check the N-body decay structure 
  bool _check_decays_ ( const std::vector<Decays::Decay>& decays , 
                        const unsigned short N ) 
  {
    //
    for (  std::vector<Decays::Decay>::const_iterator idecay = 
             decays.begin() ; decays.end() != idecay ; ++idecay ) 
    { if ( idecay->children().size() != N ) { return false ; } }
    //
    return true ;
  }
  // ==========================================================================
  /** @var compare 
   *  comparison criteria to remove of double counts for the same pid
   *  @see LoKi::Particles::NBodyCompare
   */
  const LoKi::Particles::NBodyCompare compare = LoKi::Particles::NBodyCompare () ;
  // ==========================================================================
} // end of anonymous namespace
// ============================================================================

// ============================================================================
// ==================  3-body stuff ===========================================
// ============================================================================

// ============================================================================
/*  the standard constructor
 *  @param name algorithm instance name
 *  @param pSvc the service locator
 */
// ============================================================================
DaVinci::N3BodyDecays::N3BodyDecays
( const std::string& name ,   // the algorithm instance name
  ISvcLocator*       pSvc )   // the service locator
  : CombineParticles   (  name , pSvc )
  , m_combination12Cut ( "Configure me!" ) // the cut for 1st&2nd 
  , m_acut12 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( false ) )
{
  //
  declareProperty
    ( "Combination12Cut"   , m_combination12Cut ,
      "The cut to be applied for combination of 1st and 2nd daughters "   )
    -> declareUpdateHandler ( &DaVinci::N3BodyDecays::propertyHandler3 , this ) ;
}
// ============================================================================
/// the standard initialization of the algorithm
StatusCode DaVinci::N3BodyDecays::initialize () // standard initialization
{
  //
  StatusCode sc = CombineParticles::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( !_check_decays_ ( m_decays , 3 ) ) 
  { return Error ( "Invalid decay sructure is specified!") ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  helper function for further decoding 
 *  @param factory the hybrid factory for decoding 
 */
// ============================================================================
StatusCode DaVinci::N3BodyDecays::decodeRest ( LoKi::IHybridFactory* factory ) 
{
  //
  if ( 0 == factory ) { return Error ("Invalid factory!") ; }
  //
  StatusCode sc = factory -> get ( m_combination12Cut , m_acut12 , preambulo () ) ;
  if ( sc.isFailure () )
  { return Error ( "Unable to  decode 'Combination12Cut': "
                   + m_combination12Cut  , sc ) ; }
  debug () << "The decoded 'Combination12Cut' is: "
           << m_acut12 << endmsg ;
  //
  return CombineParticles::decodeRest ( factory ) ;
  //
}
// ============================================================================

// ============================================================================
// ==================  4-body stuff ===========================================
// ============================================================================

// ============================================================================
/*  the standard constructor
 *  @param name algorithm instance name
 *  @param pSvc the service locator
 */
// ============================================================================
DaVinci::N4BodyDecays::N4BodyDecays
( const std::string& name ,   // the algorithm instance name
  ISvcLocator*       pSvc )   // the service locator
  : DaVinci::N3BodyDecays (  name , pSvc )
  , m_combination123Cut ( "Configure me!" ) // the cut for 1st&2nd 
  , m_acut123 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( false ) )
{
  //
  declareProperty
    ( "Combination123Cut"   , m_combination123Cut ,
      "The cut to be applied for combination of 1st,2nd and 3rd daughters "   )
    -> declareUpdateHandler ( &DaVinci::N4BodyDecays::propertyHandler4 , this ) ;
}
// ============================================================================
/// the standard initialization of the algorithm
StatusCode DaVinci::N4BodyDecays::initialize () // standard initialization
{
  //
  StatusCode sc = CombineParticles::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( !_check_decays_ ( m_decays , 4 ) ) 
  { return Error ( "Invalid decay sructure is specified!") ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  helper function for further decoding 
 *  @param factory the hybrid factory for decoding 
 */
// ============================================================================
StatusCode DaVinci::N4BodyDecays::decodeRest ( LoKi::IHybridFactory* factory ) 
{
  //
  if ( 0 == factory ) { return Error ("Invalid factory!") ; }
  //
  StatusCode sc = factory -> get ( m_combination123Cut , m_acut123 , preambulo () ) ;
  if ( sc.isFailure () )
  { return Error ( "Unable to  decode 'Combination123Cut': "
                   + m_combination123Cut  , sc ) ; }
  debug () << "The decoded 'Combination123Cut' is: "
           << m_acut123 << endmsg ;
  //
  return DaVinci::N3BodyDecays::decodeRest ( factory ) ;
  //
}
// ============================================================================

// ============================================================================
// ==================  5-body stuff ===========================================
// ============================================================================

// ============================================================================
/*  the standard constructor
 *  @param name algorithm instance name
 *  @param pSvc the service locator
 */
// ============================================================================
DaVinci::N5BodyDecays::N5BodyDecays
( const std::string& name ,   // the algorithm instance name
  ISvcLocator*       pSvc )   // the service locator
  : DaVinci::N4BodyDecays (  name , pSvc )
  , m_combination1234Cut ( "Configure me!" ) // the cut for 1-4
  , m_acut1234 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( false ) )
{
  //
  declareProperty
    ( "Combination1234Cut"   , m_combination1234Cut ,
      "The cut to be applied for combination of 1st,2nd,3rd&4th daughters "   )
    -> declareUpdateHandler ( &DaVinci::N5BodyDecays::propertyHandler5 , this ) ;
}
// ============================================================================
/// the standard initialization of the algorithm
StatusCode DaVinci::N5BodyDecays::initialize () // standard initialization
{
  //
  StatusCode sc = CombineParticles::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( !_check_decays_ ( m_decays , 5 ) ) 
  { return Error ( "Invalid decay sructure is specified!") ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  helper function for further decoding 
 *  @param factory the hybrid factory for decoding 
 */
// ============================================================================
StatusCode DaVinci::N5BodyDecays::decodeRest ( LoKi::IHybridFactory* factory ) 
{
  //
  if ( 0 == factory ) { return Error ("Invalid factory!") ; }
  //
  StatusCode sc = factory -> get ( m_combination1234Cut , 
                                   m_acut1234 , preambulo () ) ;
  if ( sc.isFailure () )
  { return Error ( "Unable to  decode 'Combination1234Cut': "
                   + m_combination1234Cut  , sc ) ; }
  debug () << "The decoded 'Combination1234Cut' is: "
           << m_acut1234 << endmsg ;
  //
  return DaVinci::N4BodyDecays::decodeRest ( factory ) ;
  //
}
// ============================================================================

// ============================================================================
// ==================  6-body stuff ===========================================
// ============================================================================

// ============================================================================
/*  the standard constructor
 *  @param name algorithm instance name
 *  @param pSvc the service locator
 */
// ============================================================================
DaVinci::N6BodyDecays::N6BodyDecays
( const std::string& name ,   // the algorithm instance name
  ISvcLocator*       pSvc )   // the service locator
  : DaVinci::N5BodyDecays (  name , pSvc )
  , m_combination12345Cut ( "Configure me!" ) // the cut for 1-5
  , m_acut12345 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( false ) )
{
  //
  declareProperty
    ( "Combination12345Cut"   , m_combination12345Cut ,
      "The cut to be applied for combination of 1st,2nd,3rd,4th&5th daughters "   )
    -> declareUpdateHandler ( &DaVinci::N6BodyDecays::propertyHandler6 , this ) ;
}
// ============================================================================
// the standard initialization of the algorithm
// ============================================================================
StatusCode DaVinci::N6BodyDecays::initialize () // standard initialization
{
  //
  StatusCode sc = CombineParticles::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( !_check_decays_ ( m_decays , 6 ) ) 
  { return Error ( "Invalid decay sructure is specified!") ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  helper function for further decoding 
 *  @param factory the hybrid factory for decoding 
 */
// ============================================================================
StatusCode DaVinci::N6BodyDecays::decodeRest ( LoKi::IHybridFactory* factory ) 
{
  //
  if ( 0 == factory ) { return Error ("Invalid factory!") ; }
  //
  StatusCode sc = factory -> get ( m_combination12345Cut , 
                                   m_acut12345 , preambulo () ) ;
  if ( sc.isFailure () )
  { return Error ( "Unable to  decode 'Combination12345Cut': "
                   + m_combination12345Cut  , sc ) ; }
  debug () << "The decoded 'Combination12345Cut' is: "
           << m_acut12345 << endmsg ;
  //
  return DaVinci::N5BodyDecays::decodeRest ( factory ) ;
  //
}
// ============================================================================


// ============================================================================
// ==================  7-body stuff ===========================================
// ============================================================================

// ============================================================================
/*  the standard constructor
 *  @param name algorithm instance name
 *  @param pSvc the service locator
 */
// ============================================================================
DaVinci::N7BodyDecays::N7BodyDecays
( const std::string& name ,   // the algorithm instance name
  ISvcLocator*       pSvc )   // the service locator
  : DaVinci::N6BodyDecays (  name , pSvc )
  , m_combination123456Cut ( "Configure me!" ) // the cut for 1-6
  , m_acut123456 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( false ) )
{
  //
  declareProperty
    ( "Combination123456Cut"   , m_combination123456Cut ,
      "The cut to be applied for combination of 1st,2nd,3rd,4th,5th&6th daughters "   )
    -> declareUpdateHandler ( &DaVinci::N7BodyDecays::propertyHandler7 , this ) ;
}
// ============================================================================
// the standard initialization of the algorithm
// ============================================================================
StatusCode DaVinci::N7BodyDecays::initialize () // standard initialization
{
  //
  StatusCode sc = CombineParticles::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( !_check_decays_ ( m_decays , 7 ) ) 
  { return Error ( "Invalid decay sructure is specified!") ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  helper function for further decoding 
 *  @param factory the hybrid factory for decoding 
 */
// ============================================================================
StatusCode DaVinci::N7BodyDecays::decodeRest ( LoKi::IHybridFactory* factory ) 
{
  //
  if ( 0 == factory ) { return Error ("Invalid factory!") ; }
  //
  StatusCode sc = factory -> get ( m_combination123456Cut , 
                                   m_acut123456 , preambulo () ) ;
  if ( sc.isFailure () )
  { return Error ( "Unable to  decode 'Combination123456Cut': "
                   + m_combination123456Cut  , sc ) ; }
  debug () << "The decoded 'Combination123456Cut' is: "
           << m_acut123456 << endmsg ;
  //
  return DaVinci::N6BodyDecays::decodeRest ( factory ) ;
  //
}
// ============================================================================

// ============================================================================
// ==================  8-body stuff ===========================================
// ============================================================================

// ============================================================================
/*  the standard constructor
 *  @param name algorithm instance name
 *  @param pSvc the service locator
 */
// ============================================================================
DaVinci::N8BodyDecays::N8BodyDecays
( const std::string& name ,   // the algorithm instance name
  ISvcLocator*       pSvc )   // the service locator
  : DaVinci::N7BodyDecays (  name , pSvc )
  , m_combination1234567Cut ( "Configure me!" ) // the cut for 1-7
  , m_acut1234567 ( LoKi::BasicFunctors<LoKi::ATypes::Combination>::BooleanConstant ( false ) )
{
  //
  declareProperty
    ( "Combination1234567Cut"   , m_combination1234567Cut ,
      "The cut to be applied for combination of 1st,2nd,3rd,4th,5th,6th&7th daughters "   )
    -> declareUpdateHandler ( &DaVinci::N8BodyDecays::propertyHandler8 , this ) ;
}
// ============================================================================
// the standard initialization of the algorithm
// ============================================================================
StatusCode DaVinci::N8BodyDecays::initialize () // standard initialization
{
  //
  StatusCode sc = CombineParticles::initialize () ;
  if ( sc.isFailure() ) { return sc ; }
  //
  if ( !_check_decays_ ( m_decays , 8 ) ) 
  { return Error ( "Invalid decay sructure is specified!") ; }
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  helper function for further decoding 
 *  @param factory the hybrid factory for decoding 
 */
// ============================================================================
StatusCode DaVinci::N8BodyDecays::decodeRest ( LoKi::IHybridFactory* factory ) 
{
  //
  if ( 0 == factory ) { return Error ("Invalid factory!") ; }
  //
  StatusCode sc = factory -> get ( m_combination1234567Cut , 
                                   m_acut1234567 , preambulo () ) ;
  if ( sc.isFailure () )
  { return Error ( "Unable to  decode 'Combination1234567Cut': "
                   + m_combination1234567Cut  , sc ) ; }
  debug () << "The decoded 'Combination1234567Cut' is: "
           << m_acut1234567 << endmsg ;
  //
  return DaVinci::N7BodyDecays::decodeRest ( factory ) ;
  //
}
// ============================================================================



// ============================================================================
//  ==================  EXECUTE ===============================================
// ============================================================================

// ============================================================================
// the standard execution of the algorithm
// ============================================================================
StatusCode DaVinci::N3BodyDecays::execute()
{
  //
  // check the execution conditions 
  //
  StatusCode sc = pre_action () ;
  if ( sc.isFailure() ) 
  { return Error("Error from pre-action conditions", sc ) ; }
  
  //
  // preselect all daughter particles 
  //
  Selected daughters ;
  sc = get_daughters ( daughters ) ;
  if ( sc.isFailure() ) 
  { return Error("Error from get-daughters method", sc ) ; }
  
  //
  // start some local activity 
  //
  
  /** get the default particle combiner/creator
   *  @attention Particle Combiner is used for creation of Mother Particle!
   */
  const IParticleCombiner* combiner = particleCombiner() ; // get the particle combiner
  
  // the counter of recontructed/selected decays:
  size_t nTotal = 0 ;
  
  // Flag to indicate if processing is aborted
  bool problem = false ;
  
  LHCb::Particle::ConstVector saved ;
  saved.reserve ( 100 ) ; // CRJ : Was 1000. Seems a bit big ?
  
  StatEntity& cnt_c12 = counter ( s_combination12 ) ;
  StatEntity& cnt_c   = counter ( s_combination   ) ;
  StatEntity& cnt_m   = counter ( s_mother        ) ;
  
  // loop over all decays
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin() ;
        m_decays.end() != idecay && !problem ; ++idecay )
  {
    // the counter of "good" selected decays
    size_t nGood = 0 ;
    
    // fill it with the input data
    const Decays::Decay::Items& items = idecay->children() ;
    Selected::Range child1 = daughters ( items [0].name() ) ;
    if ( child1.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child2 = daughters ( items [1].name() ) ;
    if ( child2.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child3 = daughters ( items [2].name() ) ;
    if ( child3.empty() ) { continue ; }                  // CONTINUE
    
    /// check  combinatoric 
    double ncomb = 1.0 * child1.size() * child2.size() * child3.size() ;
    problem = problem || tooMuchCombinations ( ncomb , *idecay ) ;
    if ( problem ) { continue  ; } //  CONTINUE, go to next decay
    
    //
    // make explicit 3-body loop
    //
    for ( Selected::Range::const_iterator it1 = child1.begin() ;
          child1.end() != it1 && !problem ; ++it1 ) 
    {
      //
      problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
      if ( problem ) { break ; }  // BREAK the loop
      //
      const LHCb::Particle* c1 = *it1 ;
      if ( 0  == c1 ) { continue ; }                   // CONTINUE, next c1  
      //
      // get the actual('current') combination:
      LHCb::Particle::ConstVector comb12      ( 2 ) ;
      // get the actual('current') combination:
      LHCb::Particle::ConstVector combination ( 3 ) ;
      comb12      [0] = c1 ;
      combination [0] = c1 ;
      //
      // loop over 2nd particle 
      for ( Selected::Range::const_iterator it2 = child2.begin() ;
            child2.end() != it2 && !problem ; ++it2 ) 
      {
        //
        problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
        if ( problem ) { break ; }  // BREAK the loop
        //
        const LHCb::Particle* c2 = *it2 ;
        //
        // good and unique ? 
        if ( !compare ( c1 , c2 ) ) { continue ; }     // CONTINUE, next c2 
        //
        comb12      [1] = c2 ;
        combination [1] = c2 ;
        //
        // Use combination cuts!  
        // it is a heart of all game: 
        // here we have the combination and can apply the cut:
        const bool pass_comb12 = m_acut12.fun ( comb12 ) ;
        cnt_c12 += pass_comb12 ;
        if ( !pass_comb12 ) { continue ; } // CONTINUE, next c2 
        //
        //
        // loop over 3rd particle 
        for ( Selected::Range::const_iterator it3 = child3.begin() ;
              child3.end() != it3 && !problem ; ++it3 ) 
        {
          //
          problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
          if ( problem ) { break ; }  // BREAK the loop
          //
          const LHCb::Particle* c3 = *it3 ;
          //        
          // good and unique ? 
          if ( !compare ( c1 , c2 , c3 ) ) { continue ; }   // CONTINUE, next c3
          //
          combination [2] = c3 ;
          
          //
          // check the combinations for overlaps and cuts 
          //
          const bool pass_combination = treat_combination ( combination ) ;
          cnt_c += pass_combination ;
          if ( !pass_combination ) { continue ; }
          
          // here we can create the mother particle, the vertex, 
          // check them and save in TES 
          const bool pass_mother = treat_mother ( idecay->mother().pid() , 
                                                  combination            , 
                                                  combiner               ,
                                                  saved                  ) ;
          cnt_m += pass_mother ;
          if ( !pass_mother ) { continue ; }
          
          // increment number of good decays
          ++nGood; 

          // ==================================================================
        }                                  // end of the loop over 3rd particle 
        // ====================================================================
      }                                    // end of the loop over 2nd particle 
      // ======================================================================
    }                                      // end of the loop over 1st particle
    // ========================================================================
    
    //
    // some statistics
    //
    if ( problem ) { nGood = 0 ; } // Nothing saved if a problem occurred
    counter ( "#" + idecay->toString() ) += nGood;
    nTotal += nGood;
    
  } // the loop over the decays
  
  //
  // Final!
  // 
  return post_action ( nTotal , problem , saved ) ;
}

// ============================================================================
// the standard execution of the algorithm
// ============================================================================
StatusCode DaVinci::N4BodyDecays::execute()
{
  //
  // check the execution conditions 
  //
  StatusCode sc = pre_action () ;
  if ( sc.isFailure() ) 
  { return Error("Error from pre-action conditions", sc ) ; }
  
  //
  // preselect all daughter particles 
  //
  Selected daughters ;
  sc = get_daughters ( daughters ) ;
  if ( sc.isFailure() ) 
  { return Error("Error from get-daughters method", sc ) ; }
  
  //
  // start some local activity 
  //
  
  /** get the default particle combiner/creator
   *  @attention Particle Combiner is used for creation of Mother Particle!
   */
  const IParticleCombiner* combiner = particleCombiner() ; // get the particle combiner
  
  // the counter of recontructed/selected decays:
  size_t nTotal = 0 ;
  
  // Flag to indicate if processing is aborted
  bool problem = false ;
  
  LHCb::Particle::ConstVector saved ;
  saved.reserve ( 100 ) ; // CRJ : Was 1000. Seems a bit big ?
  
  StatEntity& cnt_c12  = counter ( s_combination12  ) ;
  StatEntity& cnt_c123 = counter ( s_combination123 ) ;
  StatEntity& cnt_c    = counter ( s_combination    ) ;
  StatEntity& cnt_m    = counter ( s_mother         ) ;
  
  // loop over all decays
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin() ;
        m_decays.end() != idecay && !problem ; ++idecay )
  {
    // the counter of "good" selected decays
    size_t nGood = 0 ;
    
    // fill it with the input data
    const Decays::Decay::Items& items = idecay->children() ;    
    Selected::Range child1 = daughters ( items [0].name() ) ;
    if ( child1.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child2 = daughters ( items [1].name() ) ;
    if ( child2.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child3 = daughters ( items [2].name() ) ;
    if ( child3.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child4 = daughters ( items [3].name() ) ;
    if ( child4.empty() ) { continue ; }                  // CONTINUE
    
    /// check  combinatoric 
    double ncomb  = 1.0 * child1.size() * child2.size() ;
    ncomb        *=       child3.size() * child4.size() ;
    problem = problem || tooMuchCombinations ( ncomb , *idecay ) ;
    if ( problem ) { continue  ; } //  CONTINUE, go to next decay
    
    //
    // make explicit 4-body loop
    //
    for ( Selected::Range::const_iterator it1 = child1.begin() ;
          child1.end() != it1 && !problem ; ++it1 ) 
    {
      //
      problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
      if ( problem ) { break ; }  // BREAK the loop
      //
      const LHCb::Particle* c1 = *it1 ;
      if ( 0  == c1 ) { continue ; }                   // CONTINUE, next c1  
      //
      // get the actual('current') combinations 
      LHCb::Particle::ConstVector comb12      ( 2 ) ;
      LHCb::Particle::ConstVector comb123     ( 3 ) ;
      LHCb::Particle::ConstVector combination ( 4 ) ;
      comb12      [0] = c1 ;
      comb123     [0] = c1 ;
      combination [0] = c1 ;
      //
      // loop over 2nd particle 
      for ( Selected::Range::const_iterator it2 = child2.begin() ;
            child2.end() != it2 && !problem ; ++it2 ) 
      {
        //
        problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
        if ( problem ) { break ; }  // BREAK the loop
        //
        const LHCb::Particle* c2 = *it2 ;
        //
        // good & unique ?
        if ( !compare ( c1 , c2 ) ) { continue ; } // CONTINUE, next c2 
        //
        comb12      [1] = c2 ;
        comb123     [1] = c2 ;
        combination [1] = c2 ;
        //
        // Use combination cuts! it is a heart of all game: 
        // here we have the combination and can apply the cut:
        const bool pass_combination12 = m_acut12.fun ( comb12 ) ;
        cnt_c12 += pass_combination12 ;
        if ( !pass_combination12 ) { continue ; } // CONTINUE, next c2 
        //
        //
        // loop over 3rd particle 
        for ( Selected::Range::const_iterator it3 = child3.begin() ;
              child3.end() != it3 && !problem ; ++it3 ) 
        {
          //
          problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
          if ( problem ) { break ; }  // BREAK the loop
          //
          const LHCb::Particle* c3 = *it3 ;
          //        
          // good & unique ? 
          if ( ! compare ( c1 ,c2 , c3 ) ) { continue ; } // CONTINUE, next c3 
          //
          comb123     [2] = c3 ;
          combination [2] = c3 ;
          
          // Use combination cuts! it is a heart of all game: 
          // here we have the combination and can apply the cut:
          const bool pass_combination123 = m_acut123.fun ( comb123 ) ;
          cnt_c123 += pass_combination123 ;
          if ( !pass_combination123 ) { continue ; } // CONTINUE, next c3 
          //
          
          // loop over 4th particle 
          for ( Selected::Range::const_iterator it4 = child4.begin() ;
                child4.end() != it4 && !problem ; ++it4 ) 
          {
            //
            problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
            if ( problem ) { break ; }  // BREAK the loop
            //
            const LHCb::Particle* c4 = *it4 ;
            //        
            // good & unique ? 
            if ( !compare ( c1 ,c2 , c3 , c4 ) ) { continue ; } // CONTINUE, next c4 
            //
            combination [3] = c4 ;
            
            //
            // check the combinations for overlaps and cuts 
            // 
            const bool pass_combination = treat_combination ( combination ) ;
            cnt_c += pass_combination ;
            if ( !pass_combination ) { continue ; }
            
            // here we can create the mother particle, the vertex, 
            // check them and save in TES 
            const bool pass_mother = treat_mother ( idecay->mother().pid() , 
                                                    combination            , 
                                                    combiner               ,
                                                    saved                  ) ;
            cnt_m += pass_mother ;
            if ( !pass_mother ) { continue ; }
            
            // increment number of good decays
            ++nGood;            
            // ================================================================
          }                                // end of the loop over 4th particle 
          // ==================================================================
        }                                  // end of the loop over 3rd particle 
        // ====================================================================
      }                                    // end of the loop over 2nd particle 
      // ======================================================================
    }                                      // end of the loop over 1st particle
    // ========================================================================
    
    //
    // some statistics
    //
    if ( problem ) { nGood = 0 ; } // Nothing saved if a problem occurred
    counter ( "#" + idecay->toString() ) += nGood;
    nTotal += nGood;
    
  } // the loop over the decays
  
  //
  // Final!
  // 
  return post_action ( nTotal , problem , saved ) ;
}

// ============================================================================
// the standard execution of the algorithm
// ============================================================================
StatusCode DaVinci::N5BodyDecays::execute()
{
  //
  // check the execution conditions 
  //
  StatusCode sc = pre_action () ;
  if ( sc.isFailure() ) 
  { return Error("Error from pre-action conditions", sc ) ; }
  
  //
  // preselect all daughter particles 
  //
  Selected daughters ;
  sc = get_daughters ( daughters ) ;
  if ( sc.isFailure() ) 
  { return Error("Error from get-daughters method", sc ) ; }
  
  //
  // start some local activity 
  //
  
  /** get the default particle combiner/creator
   *  @attention Particle Combiner is used for creation of Mother Particle!
   */
  const IParticleCombiner* combiner = particleCombiner() ; // get the particle combiner
  
  // the counter of recontructed/selected decays:
  size_t nTotal = 0 ;
  
  // Flag to indicate if processing is aborted
  bool problem = false ;
  
  LHCb::Particle::ConstVector saved ;
  saved.reserve ( 100 ) ; // CRJ : Was 1000. Seems a bit big ?
  
  StatEntity& cnt_c12   = counter ( s_combination12   ) ;
  StatEntity& cnt_c123  = counter ( s_combination123  ) ;
  StatEntity& cnt_c1234 = counter ( s_combination1234 ) ;
  StatEntity& cnt_c     = counter ( s_combination     ) ;
  StatEntity& cnt_m     = counter ( s_mother          ) ;
  
  
  // loop over all decays
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin() ;
        m_decays.end() != idecay && !problem ; ++idecay )
  {
    // the counter of "good" selected decays
    size_t nGood = 0 ;
    
    // fill it with the input data
    const Decays::Decay::Items& items = idecay->children() ;    
    Selected::Range child1 = daughters ( items [0].name() ) ;
    if ( child1.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child2 = daughters ( items [1].name() ) ;
    if ( child2.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child3 = daughters ( items [2].name() ) ;
    if ( child3.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child4 = daughters ( items [3].name() ) ;
    if ( child4.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child5 = daughters ( items [4].name() ) ;
    if ( child5.empty() ) { continue ; }                  // CONTINUE
    
    /// check  combinatoric 
    double ncomb  = 1.0 * child1.size() * child2.size() ;
    ncomb        *=       child3.size() * child4.size() ;
    ncomb        *=                       child5.size() ;
    problem = problem || tooMuchCombinations ( ncomb , *idecay ) ;
    if ( problem ) { continue  ; } //  CONTINUE, go to next decay
    
    //
    // make explicit 5-body loop
    //
    for ( Selected::Range::const_iterator it1 = child1.begin() ;
          child1.end() != it1 && !problem ; ++it1 ) 
    {
      //
      problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
      if ( problem ) { break ; }  // BREAK the loop
      //
      const LHCb::Particle* c1 = *it1 ;
      if ( 0  == c1 ) { continue ; }                   // CONTINUE, next c1  
      //
      // get the actual('current') combinations 
      LHCb::Particle::ConstVector comb12      ( 2 ) ;
      LHCb::Particle::ConstVector comb123     ( 3 ) ;
      LHCb::Particle::ConstVector comb1234    ( 4 ) ;
      LHCb::Particle::ConstVector combination ( 5 ) ;
      comb12      [0] = c1 ;
      comb123     [0] = c1 ;
      comb1234    [0] = c1 ;
      combination [0] = c1 ;
      //
      // loop over 2nd particle 
      for ( Selected::Range::const_iterator it2 = child2.begin() ;
            child2.end() != it2 && !problem ; ++it2 ) 
      {
        //
        problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
        if ( problem ) { break ; }  // BREAK the loop
        //
        const LHCb::Particle* c2 = *it2 ;
        //
        // good & unique ?
        if ( !compare ( c1 , c2 ) ) { continue ; } // CONTINUE, next c2 
        //
        comb12      [1] = c2 ;
        comb123     [1] = c2 ;
        comb1234    [1] = c2 ;
        combination [1] = c2 ;
        //
        // Use combination cuts! it is a heart of all game: 
        // here we have the combination and can apply the cut:
        const bool pass_combination12 = m_acut12.fun ( comb12 ) ;
        cnt_c12 += pass_combination12 ;
        if ( !pass_combination12 ) { continue ; } // CONTINUE, next c2 
        //
        // loop over 3rd particle 
        for ( Selected::Range::const_iterator it3 = child3.begin() ;
              child3.end() != it3 && !problem ; ++it3 ) 
        {
          //
          problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
          if ( problem ) { break ; }  // BREAK the loop
          //
          const LHCb::Particle* c3 = *it3 ;
          //        
          // good & unique ? 
          if ( !compare ( c1 ,c2 , c3 ) ) { continue ; } // CONTINUE, next c3 
          //
          comb123     [2] = c3 ;
          comb1234    [2] = c3 ;
          combination [2] = c3 ;
          
          // Use combination cuts! it is a heart of all game: 
          // here we have the combination and can apply the cut:
          const bool pass_combination123 = m_acut123.fun ( comb123 ) ;
          cnt_c123 += pass_combination123 ;
          if ( !pass_combination123 ) { continue ; } // CONTINUE, next c3 
          //
          
          // loop over 4th particle 
          for ( Selected::Range::const_iterator it4 = child4.begin() ;
                child4.end() != it4 && !problem ; ++it4 ) 
          {
            //
            problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
            if ( problem ) { break ; }  // BREAK the loop
            //
            const LHCb::Particle* c4 = *it4 ;
            //        
            // good & unique ? 
            if ( !compare ( c1 ,c2 , c3 , c4 ) ) { continue ; } // CONTINUE, next c4 
            //
            comb1234    [3] = c4 ;
            combination [3] = c4 ;
            
            // Use combination cuts! it is a heart of all game: 
            // here we have the combination and can apply the cut:
            const bool pass_combination1234 = m_acut1234.fun ( comb1234 ) ;
            cnt_c1234 += pass_combination1234 ;
            if ( !pass_combination1234 ) { continue ; } // CONTINUE, next c4 
            //
            
            // loop over 5th particle 
            for ( Selected::Range::const_iterator it5 = child5.begin() ;
                  child5.end() != it5 && !problem ; ++it5 ) 
            {
              
              problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
              if ( problem ) { break ; }  // BREAK the loop
              //
              const LHCb::Particle* c5 = *it5 ;
              //        
              // good & unique ? 
              if ( !compare ( c1 ,c2 , c3 , c4 , c5 ) ) { continue ; } // CONTINUE, next c5
              //
              combination [4] = c5 ;
              
              //
              // check the combinations for overlaps and cuts 
              // 
              const bool pass_combination = treat_combination ( combination ) ;
              cnt_c += pass_combination ;
              if ( !pass_combination ) { continue ; }
              
              // here we can create the mother particle, the vertex, 
              // check them and save in TES 
              const bool pass_mother = treat_mother ( idecay->mother().pid() , 
                                                      combination            , 
                                                      combiner               ,
                                                      saved                  ) ;
              cnt_m += pass_mother ;
              if ( !pass_mother ) { continue ; }
              
              // increment number of good decays
              ++nGood;
              // ==============================================================
            }                              // end of the loop over 5th particle 
            // ================================================================
          }                                // end of the loop over 4th particle 
          // ==================================================================
        }                                  // end of the loop over 3rd particle 
        // ====================================================================
      }                                    // end of the loop over 2nd particle 
      // ======================================================================
    }                                      // end of the loop over 1st particle
    // ========================================================================
    
    //
    // some statistics
    //
    if ( problem ) { nGood = 0 ; } // Nothing saved if a problem occurred
    counter ( "#" + idecay->toString() ) += nGood;
    nTotal += nGood;
    
  } // the loop over the decays
  
  //
  // Final!
  // 
  return post_action ( nTotal , problem , saved ) ;
}

// ============================================================================
// the standard execution of the algorithm
// ============================================================================
StatusCode DaVinci::N6BodyDecays::execute()
{
  //
  // check the execution conditions 
  //
  StatusCode sc = pre_action () ;
  if ( sc.isFailure() ) 
  { return Error("Error from pre-action conditions", sc ) ; }
  
  //
  // preselect all daughter particles 
  //
  Selected daughters ;
  sc = get_daughters ( daughters ) ;
  if ( sc.isFailure() ) 
  { return Error("Error from get-daughters method", sc ) ; }
  
  //
  // start some local activity 
  //
  
  /** get the default particle combiner/creator
   *  @attention Particle Combiner is used for creation of Mother Particle!
   */
  const IParticleCombiner* combiner = particleCombiner() ; // get the particle combiner
  
  // the counter of recontructed/selected decays:
  size_t nTotal = 0 ;
  
  // Flag to indicate if processing is aborted
  bool problem = false ;
  
  LHCb::Particle::ConstVector saved ;
  saved.reserve ( 100 ) ; // CRJ : Was 1000. Seems a bit big ?
  
  // a bit of monitoring 
  StatEntity& cnt_c12    = counter ( s_combination12    ) ;
  StatEntity& cnt_c123   = counter ( s_combination123   ) ;
  StatEntity& cnt_c1234  = counter ( s_combination1234  ) ;
  StatEntity& cnt_c12345 = counter ( s_combination12345 ) ;
  StatEntity& cnt_c      = counter ( s_combination      ) ;
  StatEntity& cnt_m      = counter ( s_mother           ) ;

  
  // loop over all decays
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin() ;
        m_decays.end() != idecay && !problem ; ++idecay )
  {
    // the counter of "good" selected decays
    size_t nGood = 0 ;
    
    // fill it with the input data
    const Decays::Decay::Items& items = idecay->children() ;    
    Selected::Range child1 = daughters ( items [0].name() ) ;
    if ( child1.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child2 = daughters ( items [1].name() ) ;
    if ( child2.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child3 = daughters ( items [2].name() ) ;
    if ( child3.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child4 = daughters ( items [3].name() ) ;
    if ( child4.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child5 = daughters ( items [4].name() ) ;
    if ( child5.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child6 = daughters ( items [5].name() ) ;
    if ( child6.empty() ) { continue ; }                  // CONTINUE
    
    /// check  combinatoric 
    double ncomb  = 1.0 * child1.size() * child2.size () ;
    ncomb        *=       child3.size() * child4.size () ;
    ncomb        *=       child5.size() * child6.size () ;
    problem = problem || tooMuchCombinations ( ncomb , *idecay ) ;
    if ( problem ) { continue  ; } //  CONTINUE, go to next decay
    
    //
    // make explicit 6-body loop
    //
    for ( Selected::Range::const_iterator it1 = child1.begin() ;
          child1.end() != it1 && !problem ; ++it1 ) 
    {
      //
      problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
      if ( problem ) { break ; }  // BREAK the loop
      //
      const LHCb::Particle* c1 = *it1 ;
      if ( 0  == c1 ) { continue ; }                   // CONTINUE, next c1  
      //
      // get the actual('current') combinations 
      LHCb::Particle::ConstVector comb12      ( 2 ) ;
      LHCb::Particle::ConstVector comb123     ( 3 ) ;
      LHCb::Particle::ConstVector comb1234    ( 4 ) ;
      LHCb::Particle::ConstVector comb12345   ( 5 ) ;
      LHCb::Particle::ConstVector combination ( 6 ) ;
      comb12      [0] = c1 ;
      comb123     [0] = c1 ;
      comb1234    [0] = c1 ;
      comb12345   [0] = c1 ;
      combination [0] = c1 ;
      //
      // loop over 2nd particle 
      for ( Selected::Range::const_iterator it2 = child2.begin() ;
            child2.end() != it2 && !problem ; ++it2 ) 
      {
        //
        problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
        if ( problem ) { break ; }  // BREAK the loop
        //
        const LHCb::Particle* c2 = *it2 ;
        //
        // good & unique ?
        if ( !compare ( c1 , c2 ) ) { continue ; } // CONTINUE, next c2 
        //
        comb12      [1] = c2 ;
        comb123     [1] = c2 ;
        comb1234    [1] = c2 ;
        comb12345   [1] = c2 ;
        combination [1] = c2 ;
        //
        // Use combination cuts! it is a heart of all game: 
        // here we have the combination and can apply the cut:
        const bool pass_combination12 = m_acut12.fun ( comb12 ) ;
        cnt_c12 += pass_combination12 ;
        if ( !pass_combination12 ) { continue ; } // CONTINUE, next c2 
        //
        //
        // loop over 3rd particle 
        for ( Selected::Range::const_iterator it3 = child3.begin() ;
              child3.end() != it3 && !problem ; ++it3 ) 
        {
          //
          problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
          if ( problem ) { break ; }  // BREAK the loop
          //
          const LHCb::Particle* c3 = *it3 ;
          //        
          // good & unique ? 
          if ( !compare ( c1 ,c2 , c3 ) ) { continue ; } // CONTINUE, next c3 
          //
          comb123     [2] = c3 ;
          comb1234    [2] = c3 ;
          comb12345   [2] = c3 ;
          combination [2] = c3 ;
          
          // Use combination cuts! it is a heart of all game: 
          // here we have the combination and can apply the cut:
          const bool pass_combination123 = m_acut123.fun ( comb123 ) ;
          cnt_c123 += pass_combination123 ;
          if ( !pass_combination123 ) { continue ; } // CONTINUE, next c3 
          //
          
          // loop over 4th particle 
          for ( Selected::Range::const_iterator it4 = child4.begin() ;
                child4.end() != it4 && !problem ; ++it4 ) 
          {
            //
            problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
            if ( problem ) { break ; }  // BREAK the loop
            //
            const LHCb::Particle* c4 = *it4 ;
            //        
            // good & unique ? 
            if ( !compare ( c1 ,c2 , c3 , c4 ) ) { continue ; } // CONTINUE, next c4 
            //
            comb1234    [3] = c4 ;
            comb12345   [3] = c4 ;
            combination [3] = c4 ;
            
            // Use combination cuts! it is a heart of all game: 
            // here we have the combination and can apply the cut:
            const bool pass_combination1234 = m_acut1234.fun ( comb1234 ) ;
            cnt_c1234 += pass_combination1234 ;
            if ( !pass_combination1234 ) { continue ; } // CONTINUE, next c4 
            //
            
            // loop over 5th particle 
            for ( Selected::Range::const_iterator it5 = child5.begin() ;
                  child5.end() != it5 && !problem ; ++it5 ) 
            {
              
              problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
              if ( problem ) { break ; }  // BREAK the loop
              //
              const LHCb::Particle* c5 = *it5 ;
              //        
              // good & unique ? 
              if ( !compare ( c1 ,c2 , c3 , c4 , c5 ) ) { continue ; } // CONTINUE, next c5
              //
              comb12345   [4] = c5 ;
              combination [4] = c5 ;
              
              // Use combination cuts! it is a heart of all game: 
              // here we have the combination and can apply the cut:
              const bool pass_combination12345 = m_acut12345.fun ( comb12345 ) ;
              cnt_c12345 += pass_combination12345 ;
              if ( !pass_combination12345 ) { continue ; } // CONTINUE, next c5
              //
              
              // loop over 6th particle 
              for ( Selected::Range::const_iterator it6 = child6.begin() ;
                    child6.end() != it6 && !problem ; ++it6 ) 
              {
                
                problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
                if ( problem ) { break ; }  // BREAK the loop
                //
                const LHCb::Particle* c6 = *it6 ;
                //        
                // good & unique ? 
                if ( !compare( c1 ,c2 , c3 , c4 , c5 , c6 ) )
                { continue ; } // CONTINUE, next c6
                //
                combination [5] = c6 ;               
                
                //
                // check the combinations for overlaps and cuts 
                // 
                const bool pass_combination = treat_combination ( combination ) ;
                cnt_c += pass_combination ;
                if ( !pass_combination ) { continue ; }
                
                // here we can create the mother particle, the vertex, 
                // check them and save in TES 
                const bool pass_mother = treat_mother ( idecay->mother().pid() , 
                                                        combination            , 
                                                        combiner               ,
                                                        saved                  ) ;
                cnt_m += pass_mother ;
                if ( !pass_mother ) { continue ; }
                
                // increment number of good decays
                ++nGood;                
                // ============================================================ 
              }                            // end of the loop over 6th particle 
              // ==============================================================
            }                              // end of the loop over 5th particle 
            // ================================================================
          }                                // end of the loop over 4th particle 
          // ==================================================================
        }                                  // end of the loop over 3rd particle 
        // ====================================================================
      }                                    // end of the loop over 2nd particle 
      // ======================================================================
    }                                      // end of the loop over 1st particle
    // ========================================================================
    
    //
    // some statistics
    //
    if ( problem ) { nGood = 0 ; } // Nothing saved if a problem occurred
    counter ( "#" + idecay->toString() ) += nGood;
    nTotal += nGood;
    
  } // the loop over the decays
  
  //
  // Final!
  // 
  return post_action ( nTotal , problem , saved ) ;
}

// ============================================================================
// the standard execution of the algorithm
// ============================================================================
StatusCode DaVinci::N7BodyDecays::execute()
{
  //
  // check the execution conditions 
  //
  StatusCode sc = pre_action () ;
  if ( sc.isFailure() ) 
  { return Error("Error from pre-action conditions", sc ) ; }
  
  //
  // preselect all daughter particles 
  //
  Selected daughters ;
  sc = get_daughters ( daughters ) ;
  if ( sc.isFailure() ) 
  { return Error("Error from get-daughters method", sc ) ; }
  
  //
  // start some local activity 
  //
  
  /** get the default particle combiner/creator
   *  @attention Particle Combiner is used for creation of Mother Particle!
   */
  const IParticleCombiner* combiner = particleCombiner() ; // get the particle combiner
  
  // the counter of recontructed/selected decays:
  size_t nTotal = 0 ;
  
  // Flag to indicate if processing is aborted
  bool problem = false ;
  
  LHCb::Particle::ConstVector saved ;
  saved.reserve ( 100 ) ; // CRJ : Was 1000. Seems a bit big ?
  
  // a bit of monitoring
  StatEntity& cnt_c12     = counter ( s_combination12     ) ;
  StatEntity& cnt_c123    = counter ( s_combination123    ) ;
  StatEntity& cnt_c1234   = counter ( s_combination1234   ) ;
  StatEntity& cnt_c12345  = counter ( s_combination12345  ) ;
  StatEntity& cnt_c123456 = counter ( s_combination123456 ) ;
  StatEntity& cnt_c       = counter ( s_combination       ) ;
  StatEntity& cnt_m       = counter ( s_mother            ) ;
  
  // loop over all decays
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin() ;
        m_decays.end() != idecay && !problem ; ++idecay )
  {
    // the counter of "good" selected decays
    size_t nGood = 0 ;
    
    // fill it with the input data
    const Decays::Decay::Items& items = idecay->children() ;    
    Selected::Range child1 = daughters ( items [0].name() ) ;
    if ( child1.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child2 = daughters ( items [1].name() ) ;
    if ( child2.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child3 = daughters ( items [2].name() ) ;
    if ( child3.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child4 = daughters ( items [3].name() ) ;
    if ( child4.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child5 = daughters ( items [4].name() ) ;
    if ( child5.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child6 = daughters ( items [5].name() ) ;
    if ( child6.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child7 = daughters ( items [6].name() ) ;
    if ( child7.empty() ) { continue ; }                  // CONTINUE
    
    /// check  combinatoric 
    double ncomb  = 1.0 * child1.size() * child2.size () ;
    ncomb        *=       child3.size() * child4.size () ;
    ncomb        *=       child5.size() * child6.size () ;
    ncomb        *=                       child7.size () ;
    problem = problem || tooMuchCombinations ( ncomb , *idecay ) ;
    if ( problem ) { continue  ; } //  CONTINUE, go to next decay
    
    //
    // make explicit 7-body loop
    //
    for ( Selected::Range::const_iterator it1 = child1.begin() ;
          child1.end() != it1 && !problem ; ++it1 ) 
    {
      //
      problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
      if ( problem ) { break ; }  // BREAK the loop
      //
      const LHCb::Particle* c1 = *it1 ;
      if ( 0  == c1 ) { continue ; }                   // CONTINUE, next c1  
      //
      // get the actual('current') combinations 
      LHCb::Particle::ConstVector comb12      ( 2 ) ;
      LHCb::Particle::ConstVector comb123     ( 3 ) ;
      LHCb::Particle::ConstVector comb1234    ( 4 ) ;
      LHCb::Particle::ConstVector comb12345   ( 5 ) ;
      LHCb::Particle::ConstVector comb123456  ( 6 ) ;
      LHCb::Particle::ConstVector combination ( 7 ) ;
      comb12      [0] = c1 ;
      comb123     [0] = c1 ;
      comb1234    [0] = c1 ;
      comb12345   [0] = c1 ;
      comb123456  [0] = c1 ;
      combination [0] = c1 ;
      //
      // loop over 2nd particle 
      for ( Selected::Range::const_iterator it2 = child2.begin() ;
            child2.end() != it2 && !problem ; ++it2 ) 
      {
        //
        problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
        if ( problem ) { break ; }  // BREAK the loop
        //
        const LHCb::Particle* c2 = *it2 ;
        //
        // good & unique ?
        if ( !compare ( c1 , c2 ) ) { continue ; } // CONTINUE, next c2 
        //
        comb12      [1] = c2 ;
        comb123     [1] = c2 ;
        comb1234    [1] = c2 ;
        comb12345   [1] = c2 ;
        comb123456  [1] = c2 ;
        combination [1] = c2 ;
        //
        // Use combination cuts! it is a heart of all game: 
        // here we have the combination and can apply the cut:
        const bool pass_combination12 = m_acut12.fun ( comb12 ) ;
        cnt_c12 += pass_combination12 ;
        if ( !pass_combination12 ) { continue ; } // CONTINUE, next c2 
        //
        //
        // loop over 3rd particle 
        for ( Selected::Range::const_iterator it3 = child3.begin() ;
              child3.end() != it3 && !problem ; ++it3 ) 
        {
          //
          problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
          if ( problem ) { break ; }  // BREAK the loop
          //
          const LHCb::Particle* c3 = *it3 ;
          //        
          // good & unique ? 
          if ( !compare ( c1 ,c2 , c3 ) ) { continue ; } // CONTINUE, next c3 
          //
          comb123     [2] = c3 ;
          comb1234    [2] = c3 ;
          comb12345   [2] = c3 ;
          comb123456  [2] = c3 ;
          combination [2] = c3 ;
          
          // Use combination cuts! it is a heart of all game: 
          // here we have the combination and can apply the cut:
          const bool pass_combination123 = m_acut123.fun ( comb123 ) ;
          cnt_c123 += pass_combination123 ;
          if ( !pass_combination123 ) { continue ; } // CONTINUE, next c3
          //
          
          // loop over 4th particle 
          for ( Selected::Range::const_iterator it4 = child4.begin() ;
                child4.end() != it4 && !problem ; ++it4 ) 
          {
            //
            problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
            if ( problem ) { break ; }  // BREAK the loop
            //
            const LHCb::Particle* c4 = *it4 ;
            //        
            // good & unique ? 
            if ( !compare ( c1 ,c2 , c3 , c4 ) ) { continue ; } // CONTINUE, next c4 
            //
            comb1234    [3] = c4 ;
            comb12345   [3] = c4 ;
            comb123456  [3] = c4 ;
            combination [3] = c4 ;
            
            // Use combination cuts! it is a heart of all game: 
            // here we have the combination and can apply the cut:
            const bool pass_combination1234 = m_acut1234.fun ( comb1234 ) ;
            cnt_c1234 += pass_combination1234 ;
            if ( !pass_combination1234 ) { continue ; } // CONTINUE, next c4
            //
            
            // loop over 5th particle 
            for ( Selected::Range::const_iterator it5 = child5.begin() ;
                  child5.end() != it5 && !problem ; ++it5 ) 
            {
              
              problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
              if ( problem ) { break ; }  // BREAK the loop
              //
              const LHCb::Particle* c5 = *it5 ;
              //        
              // good & unique ? 
              if ( !compare ( c1 ,c2 , c3 , c4 , c5 ) ) { continue ; } // CONTINUE, next c5
              //
              comb12345   [4] = c5 ;
              comb123456  [4] = c5 ;
              combination [4] = c5 ;
              
              // Use combination cuts! it is a heart of all game: 
              // here we have the combination and can apply the cut:
              const bool pass_combination12345 = m_acut12345.fun ( comb12345 ) ;
              cnt_c12345 += pass_combination12345 ;
              if ( !pass_combination12345 ) { continue ; } // CONTINUE, next c5
              //
              
              // loop over 6th particle 
              for ( Selected::Range::const_iterator it6 = child6.begin() ;
                    child6.end() != it6 && !problem ; ++it6 ) 
              {
                
                problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
                if ( problem ) { break ; }  // BREAK the loop
                //
                const LHCb::Particle* c6 = *it6 ;
                //        
                // good & unique ? 
                if ( !compare ( c1 ,c2 , c3 , c4 , c5 , c6 ) )
                { continue ; } // CONTINUE, next c6
                //
                comb123456  [5] = c6 ;
                combination [5] = c6 ;               
                
             
                // Use combination cuts! it is a heart of all game: 
                // here we have the combination and can apply the cut:
                const bool pass_combination123456 = m_acut123456.fun ( comb123456 ) ;
                cnt_c123456 += pass_combination123456 ;
                if ( !pass_combination123456 ) { continue ; } // CONTINUE, next c6
                //
                
                // loop over 7th particle 
                for ( Selected::Range::const_iterator it7 = child7.begin() ;
                      child7.end() != it7 && !problem ; ++it7 ) 
                {
                  
                  problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
                  if ( problem ) { break ; }  // BREAK the loop
                  //
                  const LHCb::Particle* c7 = *it7 ;
                  //        
                  // good & unique ? 
                  if ( !compare ( c1 ,c2 , c3 , c4 , c5 , c6 , c7 ) )
                  { continue ; } // CONTINUE, next c6
                  //
                  combination [6] = c7 ;            
                  
                  //
                  // check the combinations for overlaps and cuts 
                  // 
                  const bool pass_combination = treat_combination ( combination ) ;
                  cnt_c += pass_combination ;
                  if ( !pass_combination ) { continue ; }
                  
                  // here we can create the mother particle, the vertex, 
                  // check them and save in TES 
                  const bool pass_mother = treat_mother ( idecay->mother().pid() , 
                                                          combination            , 
                                                          combiner               ,
                                                          saved                  ) ;
                  cnt_m += pass_mother ;
                  if ( !pass_mother ) { continue ; }
                  
                  // increment number of good decays
                  ++nGood ;
                  // ==========================================================
                }                          // end of the loop over 7th particle 
                // ============================================================
              }                            // end of the loop over 6th particle 
              // ==============================================================
            }                              // end of the loop over 5th particle 
            // ================================================================
          }                                // end of the loop over 4th particle 
          // ==================================================================
        }                                  // end of the loop over 3rd particle 
        // ====================================================================
      }                                    // end of the loop over 2nd particle 
      // ======================================================================
    }                                      // end of the loop over 1st particle
    // ========================================================================
    
    //
    // some statistics
    //
    if ( problem ) { nGood = 0 ; } // Nothing saved if a problem occurred
    counter ( "#" + idecay->toString() ) += nGood;
    nTotal += nGood;
    
  } // the loop over the decays
  
  //
  // Final!
  // 
  return post_action ( nTotal , problem , saved ) ;
}
// ============================================================================

// ============================================================================
// the standard execution of the algorithm
// ============================================================================
StatusCode DaVinci::N8BodyDecays::execute()
{
  //
  // check the execution conditions 
  //
  StatusCode sc = pre_action () ;
  if ( sc.isFailure() ) 
  { return Error("Error from pre-action conditions", sc ) ; }
  
  //
  // preselect all daughter particles 
  //
  Selected daughters ;
  sc = get_daughters ( daughters ) ;
  if ( sc.isFailure() ) 
  { return Error("Error from get-daughters method", sc ) ; }
  
  //
  // start some local activity 
  //
  
  /** get the default particle combiner/creator
   *  @attention Particle Combiner is used for creation of Mother Particle!
   */
  const IParticleCombiner* combiner = particleCombiner() ; // get the particle combiner
  
  // the counter of recontructed/selected decays:
  size_t nTotal = 0 ;
  
  // Flag to indicate if processing is aborted
  bool problem = false ;
  
  LHCb::Particle::ConstVector saved ;
  saved.reserve ( 100 ) ; // CRJ : Was 1000. Seems a bit big ?
  
  StatEntity& cnt_c12      = counter ( s_combination12      ) ;
  StatEntity& cnt_c123     = counter ( s_combination123     ) ;
  StatEntity& cnt_c1234    = counter ( s_combination1234    ) ;
  StatEntity& cnt_c12345   = counter ( s_combination12345   ) ;
  StatEntity& cnt_c123456  = counter ( s_combination123456  ) ;
  StatEntity& cnt_c1234567 = counter ( s_combination1234567 ) ;
  StatEntity& cnt_c        = counter ( s_combination        ) ;
  StatEntity& cnt_m        = counter ( s_mother             ) ;

  // loop over all decays
  for ( std::vector<Decays::Decay>::const_iterator idecay = m_decays.begin() ;
        m_decays.end() != idecay && !problem ; ++idecay )
  {
    // the counter of "good" selected decays
    size_t nGood = 0 ;
    
    // fill it with the input data
    const Decays::Decay::Items& items = idecay->children() ;    
    Selected::Range child1 = daughters ( items [0].name() ) ;
    if ( child1.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child2 = daughters ( items [1].name() ) ;
    if ( child2.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child3 = daughters ( items [2].name() ) ;
    if ( child3.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child4 = daughters ( items [3].name() ) ;
    if ( child4.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child5 = daughters ( items [4].name() ) ;
    if ( child5.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child6 = daughters ( items [5].name() ) ;
    if ( child6.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child7 = daughters ( items [6].name() ) ;
    if ( child7.empty() ) { continue ; }                  // CONTINUE
    Selected::Range child8 = daughters ( items [7].name() ) ;
    if ( child8.empty() ) { continue ; }                  // CONTINUE
    
    /// check  combinatoric 
    double ncomb  = 1.0 * child1.size () * child2.size () ;
    ncomb        *=       child3.size () * child4.size () ;
    ncomb        *=       child5.size () * child6.size () ;
    ncomb        *=       child7.size () * child8.size () ;
    problem = problem || tooMuchCombinations ( ncomb , *idecay ) ;
    if ( problem ) { continue  ; } //  CONTINUE, go to next decay
    
    //
    // make explicit 8-body loop
    //
    for ( Selected::Range::const_iterator it1 = child1.begin() ;
          child1.end() != it1 && !problem ; ++it1 ) 
    {
      //
      problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
      if ( problem ) { break ; }  // BREAK the loop
      //
      const LHCb::Particle* c1 = *it1 ;
      if ( 0  == c1 ) { continue ; }                   // CONTINUE, next c1  
      //
      // get the actual('current') combinations 
      LHCb::Particle::ConstVector comb12      ( 2 ) ;
      LHCb::Particle::ConstVector comb123     ( 3 ) ;
      LHCb::Particle::ConstVector comb1234    ( 4 ) ;
      LHCb::Particle::ConstVector comb12345   ( 5 ) ;
      LHCb::Particle::ConstVector comb123456  ( 6 ) ;
      LHCb::Particle::ConstVector comb1234567 ( 7 ) ;
      LHCb::Particle::ConstVector combination ( 8 ) ;
      comb12      [0] = c1 ;
      comb123     [0] = c1 ;
      comb1234    [0] = c1 ;
      comb12345   [0] = c1 ;
      comb123456  [0] = c1 ;
      comb1234567 [0] = c1 ;
      combination [0] = c1 ;
      //
      // loop over 2nd particle 
      for ( Selected::Range::const_iterator it2 = child2.begin() ;
            child2.end() != it2 && !problem ; ++it2 ) 
      {
        //
        problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
        if ( problem ) { break ; }  // BREAK the loop
        //
        const LHCb::Particle* c2 = *it2 ;
        //
        // good & unique ?
        if ( !compare ( c1 , c2 ) ) { continue ; } // CONTINUE, next c2 
        //
        comb12      [1] = c2 ;
        comb123     [1] = c2 ;
        comb1234    [1] = c2 ;
        comb12345   [1] = c2 ;
        comb123456  [1] = c2 ;
        comb1234567 [1] = c2 ;
        combination [1] = c2 ;
        //
        // Use combination cuts! it is a heart of all game: 
        // here we have the combination and can apply the cut:
        const bool pass_combination12 = m_acut12.fun ( comb12 ) ;
        cnt_c12 += pass_combination12 ;
        if ( !pass_combination12 ) { continue ; } // CONTINUE, next c2 
        //
        //
        // loop over 3rd particle 
        for ( Selected::Range::const_iterator it3 = child3.begin() ;
              child3.end() != it3 && !problem ; ++it3 ) 
        {
          //
          problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
          if ( problem ) { break ; }  // BREAK the loop
          //
          const LHCb::Particle* c3 = *it3 ;
          //        
          // good & unique ? 
          if ( !compare ( c1 ,c2 , c3 ) ) { continue ; } // CONTINUE, next c3 
          //
          comb123     [2] = c3 ;
          comb1234    [2] = c3 ;
          comb12345   [2] = c3 ;
          comb123456  [2] = c3 ;
          comb1234567 [2] = c3 ;
          combination [2] = c3 ;
          
          // Use combination cuts! it is a heart of all game: 
          // here we have the combination and can apply the cut:
          const bool pass_combination123 = m_acut123.fun ( comb123 ) ;
          cnt_c123 += pass_combination123 ;
          if ( !pass_combination123 ) { continue ; } // CONTINUE, next c3 
          //
          
          // loop over 4th particle 
          for ( Selected::Range::const_iterator it4 = child4.begin() ;
                child4.end() != it4 && !problem ; ++it4 ) 
          {
            //
            problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
            if ( problem ) { break ; }  // BREAK the loop
            //
            const LHCb::Particle* c4 = *it4 ;
            //        
            // good & unique ? 
            if ( !compare ( c1 ,c2 , c3 , c4 ) ) { continue ; } // CONTINUE, next c4 
            //
            comb1234    [3] = c4 ;
            comb12345   [3] = c4 ;
            comb123456  [3] = c4 ;
            comb1234567 [3] = c4 ;
            combination [3] = c4 ;
            
            // Use combination cuts! it is a heart of all game: 
            // here we have the combination and can apply the cut:
            const bool pass_combination1234 = m_acut1234.fun ( comb1234 ) ;
            cnt_c1234 += pass_combination1234 ;
            if ( !pass_combination1234 ) { continue ; } // CONTINUE, next c4
            //
            
            // loop over 5th particle 
            for ( Selected::Range::const_iterator it5 = child5.begin() ;
                  child5.end() != it5 && !problem ; ++it5 ) 
            {
              
              problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
              if ( problem ) { break ; }  // BREAK the loop
              //
              const LHCb::Particle* c5 = *it5 ;
              //        
              // good & unique ? 
              if ( !compare ( c1 ,c2 , c3 , c4 , c5 ) ) { continue ; } // CONTINUE, next c5
              //
              comb12345   [4] = c5 ;
              comb123456  [4] = c5 ;
              comb1234567 [4] = c5 ;
              combination [4] = c5 ;
              
              // Use combination cuts! it is a heart of all game: 
              // here we have the combination and can apply the cut:
              const bool pass_combination12345 = m_acut12345.fun ( comb12345 ) ;
              cnt_c12345 += pass_combination12345 ;
              if ( !pass_combination12345 ) { continue ; } // CONTINUE, next c5
              //
              
              // loop over 6th particle 
              for ( Selected::Range::const_iterator it6 = child6.begin() ;
                    child6.end() != it6 && !problem ; ++it6 ) 
              {
                
                problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
                if ( problem ) { break ; }  // BREAK the loop
                //
                const LHCb::Particle* c6 = *it6 ;
                //        
                // good & unique ? 
                if ( !compare ( c1 ,c2 , c3 , c4 , c5 , c6 ) )
                { continue ; } // CONTINUE, next c6
                //
                comb123456  [5] = c6 ;
                comb1234567 [5] = c6 ;
                combination [5] = c6 ;                
             
                // Use combination cuts! it is a heart of all game: 
                // here we have the combination and can apply the cut:
                const bool pass_combination123456 = m_acut123456.fun ( comb123456 ) ;
                cnt_c123456 += pass_combination123456 ;
                if ( !pass_combination123456 ) { continue ; } // CONTINUE, next c6
                //
                
                // loop over 7th particle 
                for ( Selected::Range::const_iterator it7 = child7.begin() ;
                      child7.end() != it7 && !problem ; ++it7 ) 
                {
                  
                  problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
                  if ( problem ) { break ; }  // BREAK the loop
                  //
                  const LHCb::Particle* c7 = *it7 ;
                  //        
                  // good & unique ? 
                  if ( !compare ( c1 ,c2 , c3 , c4 , c5 , c6 , c7 ) )
                  { continue ; } // CONTINUE, next c6
                  //
                  comb1234567 [6] = c7 ;
                  combination [6] = c7 ;            
                  
                  // Use combination cuts! it is a heart of all game: 
                  // here we have the combination and can apply the cut:
                  const bool pass_combination1234567 = m_acut1234567.fun ( comb1234567 ) ;
                  cnt_c1234567 += pass_combination1234567 ;
                  if ( !pass_combination1234567 ) { continue ; } // CONTINUE, next c7
                  //
                  
                  // loop over 8th particle 
                  for ( Selected::Range::const_iterator it8 = child8.begin() ;
                        child8.end() != it8 && !problem ; ++it8 ) 
                  {
                    
                    problem = problem || tooMuchCandidates ( nGood , *idecay ) ;
                    if ( problem ) { break ; }  // BREAK the loop
                    //
                    const LHCb::Particle* c8 = *it8 ;
                    //        
                    // good & unique ? 
                    if ( !compare ( c1 ,c2 , c3 , c4 , c5 , c6 , c7 , c8 ) )
                    { continue ; } // CONTINUE, next c6
                    //
                    combination [7] = c8 ;            
                    
                    //
                    // check the combinations for overlaps and cuts 
                    // 
                    const bool pass_combination = treat_combination ( combination ) ;
                    cnt_c += pass_combination ;
                    if ( !pass_combination ) { continue ; }
                    
                    // here we can create the mother particle, the vertex, 
                    // check them and save in TES 
                    const bool pass_mother = treat_mother ( idecay->mother().pid() , 
                                                            combination            , 
                                                            combiner               ,
                                                            saved                  ) ;
                    cnt_m += pass_mother ;
                    if ( !pass_mother ) { continue ; }
                    
                    // increment number of good decays
                    ++nGood;
                    // ========================================================
                  }                        // end of the loop over 8th particle 
                  // ==========================================================
                }                          // end of the loop over 7th particle 
                // ============================================================
              }                            // end of the loop over 6th particle 
              // ==============================================================
            }                              // end of the loop over 5th particle 
            // ================================================================
          }                                // end of the loop over 4th particle 
          // ==================================================================
        }                                  // end of the loop over 3rd particle 
        // ====================================================================
      }                                    // end of the loop over 2nd particle 
      // ======================================================================
    }                                      // end of the loop over 1st particle
    // ========================================================================
    
    //
    // some statistics
    //
    if ( problem ) { nGood = 0 ; } // Nothing saved if a problem occurred
    counter ( "#" + idecay->toString() ) += nGood;
    nTotal += nGood;
    
  } // the loop over the decays
  
  //
  // Final!
  // 
  return post_action ( nTotal , problem , saved ) ;
}

// ============================================================================
// The factories (needed for the proper instantiation)
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DaVinci,N3BodyDecays)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DaVinci,N4BodyDecays)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DaVinci,N5BodyDecays)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DaVinci,N6BodyDecays)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DaVinci,N7BodyDecays)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DaVinci,N8BodyDecays)

// ============================================================================
// The END 
// ============================================================================


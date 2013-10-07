// $Id:$
// ============================================================================
// Include files 
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/CompareParticles.h"
// ============================================================================
// local 
// ============================================================================
#include "CombineParticles.h"
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
} //                                               the end of namespace DaVinci
// ============================================================================
namespace 
{
  // ========================================================================== 
  // very crude uniqueness criteria, based on the same protos  
  bool _unique_ ( const LHCb::Particle* p1 , 
                  const LHCb::Particle* p2 ) ;
  // very crude uniqueness criteria, based on the same protos  
  template <class PARTICLE> 
  bool _unique_ ( PARTICLE begin           , 
                  PARTICLE end             , 
                  const LHCb::Particle* p2 ) 
  {
    for ( ; begin != end ; ++begin ) 
    { if ( !_unique_ ( *begin , p2 ) ) { return false ; } }
    return true ;
  }
  // very crude uniqueness criteria, based on the same protos  
  bool _unique_ ( const LHCb::Particle* p1 , 
                  const LHCb::Particle* p2 ) 
  {
    //
    if ( 0 == p1 || 0 == p2 || p1 == p2 ) { return false ; }
    //
    const bool bp1 = p1 -> isBasicParticle() ;
    const bool bp2 = p2 -> isBasicParticle() ;
    //
    if ( bp1 && bp2 ) 
    { 
      //
      const LHCb::ProtoParticle* pp1 = p1->proto() ;
      const LHCb::ProtoParticle* pp2 = p2->proto() ;
      //
      if ( 0 != pp1 && 0 != pp2 && pp1 == pp2 ) { return false ; }
      //
      return true ; // a bit strange logic, but should be OK
    }
    //
    const SmartRefVector<LHCb::Particle>& daughters1 = p1 -> daughters() ;
    const SmartRefVector<LHCb::Particle>& daughters2 = p2 -> daughters() ;
    //
    // one basic and one composite particle 
    if      ( bp1 ) { return _unique_ ( daughters2.begin() , daughters2.end() , p1 ) ; }
    else if ( bp2 ) { return _unique_ ( daughters1.begin() , daughters1.end() , p2 ) ; }
    //
    return 
      daughters1.size() < daughters2.size() ?
      _unique_ ( daughters2.begin () , daughters2.end() , p1 ) :
      _unique_ ( daughters1.begin () , daughters1.end() , p2 ) ;
  }
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
   *  @see LoKi::Particles::PidCompare
   */
  const LoKi::Particles::PidCompare compare = LoKi::Particles::PidCompare () ;
  // ==========================================================================
  bool _ok_ ( const LHCb::Particle* p1 , 
              const LHCb::Particle* p2 ) 
  {
    return 
      0 != p1 && 0 != p2 && p1 != p2 
      &&  compare ( p1 , p2 ) 
      && _unique_ ( p1 , p2 ) ;
  }
  // ==========================================================================
  bool _ok_ ( const LHCb::Particle* p1 , 
              const LHCb::Particle* p2 ,
              const LHCb::Particle* p3 )
  {
    return 
      0  != p3 && p1 != p3 && p2 != p3
      &&  compare ( p1 , p3 ) &&  compare ( p2 , p3 ) 
      && _unique_ ( p1 , p3 ) && _unique_ ( p2 , p3 ) ;
  }
  // ==========================================================================  
  bool _ok_ ( const LHCb::Particle* p1 , 
              const LHCb::Particle* p2 ,
              const LHCb::Particle* p3 ,
              const LHCb::Particle* p4 )
  {
    return 
      0  != p4 && p1 != p4 && p2 != p4 && p3 != p4 
      &&  compare ( p1 , p4 ) &&  compare ( p2 , p4 ) &&  compare ( p3 , p4 ) 
      && _unique_ ( p1 , p4 ) && _unique_ ( p2 , p4 ) && _unique_ ( p3 , p4 ) ;
  }
  // ==========================================================================  
  bool _ok_ ( const LHCb::Particle* p1 , 
              const LHCb::Particle* p2 ,
              const LHCb::Particle* p3 ,
              const LHCb::Particle* p4 ,
              const LHCb::Particle* p5 )
  {
    return 
      0  != p5 && p1 != p5 && p2 != p5 && p3 != p5 && p4 != p5  
      &&  compare ( p1 , p5 ) 
      &&  compare ( p2 , p5 ) 
      &&  compare ( p3 , p5 ) 
      &&  compare ( p4 , p5 ) 
      && _unique_ ( p1 , p5 ) 
      && _unique_ ( p2 , p5 ) 
      && _unique_ ( p3 , p5 ) 
      && _unique_ ( p4 , p5 ) ;
  }
  // ==========================================================================  
  bool _ok_ ( const LHCb::Particle* p1 , 
              const LHCb::Particle* p2 ,
              const LHCb::Particle* p3 ,
              const LHCb::Particle* p4 ,
              const LHCb::Particle* p5 ,
              const LHCb::Particle* p6 )
  {
    return 
      0  != p6 && p1 != p6 && p2 != p6 && p3 != p6 && p4 != p6 && p5 != p6   
      &&  compare ( p1 , p6 ) 
      &&  compare ( p2 , p6 ) 
      &&  compare ( p3 , p6 ) 
      &&  compare ( p4 , p6 ) 
      &&  compare ( p5 , p6 )
      && _unique_ ( p1 , p6 ) 
      && _unique_ ( p2 , p6 ) 
      && _unique_ ( p3 , p6 ) 
      && _unique_ ( p4 , p6 ) 
      && _unique_ ( p5 , p6 ) ;
  }
  // ==========================================================================
  bool _ok_ ( const LHCb::Particle* p1 , 
              const LHCb::Particle* p2 ,
              const LHCb::Particle* p3 ,
              const LHCb::Particle* p4 ,
              const LHCb::Particle* p5 ,
              const LHCb::Particle* p6 ,
              const LHCb::Particle* p7 )
  {
    return 
      0  != p7 && 
      p1 != p7 && p2 != p7 && p3 != p7 && 
      p4 != p7 && p5 != p7 && p6 != p7   
      &&  compare ( p1 , p7 ) 
      &&  compare ( p2 , p7 ) 
      &&  compare ( p3 , p7 ) 
      &&  compare ( p4 , p7 ) 
      &&  compare ( p5 , p7 )
      &&  compare ( p6 , p7 )
      && _unique_ ( p1 , p7 ) 
      && _unique_ ( p2 , p7 ) 
      && _unique_ ( p3 , p7 ) 
      && _unique_ ( p4 , p7 )
      && _unique_ ( p5 , p7 ) 
      && _unique_ ( p6 , p7 ) ;
  }
  // ==========================================================================  
} // end of anonymous namesapce 
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
           << m_acut12 << endreq ;
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
           << m_acut123 << endreq ;
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
           << m_acut1234 << endreq ;
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
           << m_acut12345 << endreq ;
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
           << m_acut123456 << endreq ;
  //
  return DaVinci::N6BodyDecays::decodeRest ( factory ) ;
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
        if ( ! _ok_ ( c1 , c2 ) ) { continue ; }     // CONTINUE, next c2 
        //
        comb12      [1] = c2 ;
        combination [1] = c2 ;
        //
        // Use combination cuts!  
        // it is a heart of all game: 
        // here we have the combination and can apply the cut:
        if ( !m_acut12.fun ( comb12 ) ) { continue ; } // CONTINUE, next c2 
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
          if ( ! _ok_ ( c1 , c2 , c3 ) ) { continue ; }   // CONTINUE, next c3
          //
          combination [2] = c3 ;
          
          //
          // check the combinations for overlaps and cuts 
          //
          if ( !treat_combination ( combination ) ) { continue ; }
          
          // here we can create the mother particle, the vertex, 
          // check them and save in TES 
          if ( !treat_mother 
               ( idecay->mother().pid() , 
                 combination            , 
                 combiner               , 
                 saved                  ) ) { continue ; }
          
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
        if ( !_ok_ ( c1 , c2 ) ) { continue ; } // CONTINUE, next c2 
        //
        comb12      [1] = c2 ;
        comb123     [1] = c2 ;
        combination [1] = c2 ;
        //
        // Use combination cuts! it is a heart of all game: 
        // here we have the combination and can apply the cut:
        if ( !m_acut12.fun ( comb12 ) ) { continue ; } // CONTINUE, next c2 
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
          if ( !_ok_ ( c1 ,c2 , c3 ) ) { continue ; } // CONTINUE, next c3 
          //
          comb123     [2] = c3 ;
          combination [2] = c3 ;
          
          // Use combination cuts! it is a heart of all game: 
          // here we have the combination and can apply the cut:
          if ( !m_acut123.fun ( comb123 ) ) { continue ; } // CONTINUE, next c3
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
            if ( !_ok_ ( c1 ,c2 , c3 , c4 ) ) { continue ; } // CONTINUE, next c4 
            //
            combination [3] = c4 ;
            
            //
            // check the combinations for overlaps and cuts 
            // 
            if ( !treat_combination ( combination ) ) { continue ; }
            
            // here we can create the mother particle, the vertex, 
            // check them and save in TES 
            if ( !treat_mother 
                 ( idecay->mother().pid() , 
                   combination            , 
                   combiner               , 
                   saved                  ) ) { continue ; }
            
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
        if ( !_ok_ ( c1 , c2 ) ) { continue ; } // CONTINUE, next c2 
        //
        comb12      [1] = c2 ;
        comb123     [1] = c2 ;
        comb1234    [1] = c2 ;
        combination [1] = c2 ;
        //
        // Use combination cuts! it is a heart of all game: 
        // here we have the combination and can apply the cut:
        if ( !m_acut12.fun ( comb12 ) ) { continue ; } // CONTINUE, next c2 
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
          if ( !_ok_ ( c1 ,c2 , c3 ) ) { continue ; } // CONTINUE, next c3 
          //
          comb123     [2] = c3 ;
          comb1234    [2] = c3 ;
          combination [2] = c3 ;
          
          // Use combination cuts! it is a heart of all game: 
          // here we have the combination and can apply the cut:
          if ( !m_acut123.fun ( comb123 ) ) { continue ; } // CONTINUE, next c3
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
            if ( !_ok_ ( c1 ,c2 , c3 , c4 ) ) { continue ; } // CONTINUE, next c4 
            //
            comb1234    [3] = c4 ;
            combination [3] = c4 ;
            
            // Use combination cuts! it is a heart of all game: 
            // here we have the combination and can apply the cut:
            if ( !m_acut1234.fun ( comb1234 ) ) { continue ; } // CONTINUE, next c4
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
              if ( !_ok_ ( c1 ,c2 , c3 , c4 , c5 ) ) { continue ; } // CONTINUE, next c5
              //
              combination [4] = c5 ;
              
              //
              // check the combinations for overlaps and cuts 
              // 
              if ( !treat_combination ( combination ) ) { continue ; }
              
              // here we can create the mother particle, the vertex, 
              // check them and save in TES 
              if ( !treat_mother 
                   ( idecay->mother().pid() , 
                     combination            , 
                     combiner               , 
                     saved                  ) ) { continue ; }
              
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
        if ( !_ok_ ( c1 , c2 ) ) { continue ; } // CONTINUE, next c2 
        //
        comb12      [1] = c2 ;
        comb123     [1] = c2 ;
        comb1234    [1] = c2 ;
        comb12345   [1] = c2 ;
        combination [1] = c2 ;
        //
        // Use combination cuts! it is a heart of all game: 
        // here we have the combination and can apply the cut:
        if ( !m_acut12.fun ( comb12 ) ) { continue ; } // CONTINUE, next c2 
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
          if ( !_ok_ ( c1 ,c2 , c3 ) ) { continue ; } // CONTINUE, next c3 
          //
          comb123     [2] = c3 ;
          comb1234    [2] = c3 ;
          comb12345   [2] = c3 ;
          combination [2] = c3 ;
          
          // Use combination cuts! it is a heart of all game: 
          // here we have the combination and can apply the cut:
          if ( !m_acut123.fun ( comb123 ) ) { continue ; } // CONTINUE, next c3
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
            if ( !_ok_ ( c1 ,c2 , c3 , c4 ) ) { continue ; } // CONTINUE, next c4 
            //
            comb1234    [3] = c4 ;
            comb12345   [3] = c4 ;
            combination [3] = c4 ;
            
            // Use combination cuts! it is a heart of all game: 
            // here we have the combination and can apply the cut:
            if ( !m_acut1234.fun ( comb1234 ) ) { continue ; } // CONTINUE, next c4
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
              if ( !_ok_ ( c1 ,c2 , c3 , c4 , c5 ) ) { continue ; } // CONTINUE, next c5
              //
              comb12345   [4] = c5 ;
              combination [4] = c5 ;
              
              // Use combination cuts! it is a heart of all game: 
              // here we have the combination and can apply the cut:
              if ( !m_acut12345.fun ( comb12345 ) ) { continue ; } // CONTINUE, next c5
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
                if ( !_ok_ ( c1 ,c2 , c3 , c4 , c5 , c6 ) )
                { continue ; } // CONTINUE, next c6
                //
                combination [5] = c6 ;               
                
                //
                // check the combinations for overlaps and cuts 
                // 
                if ( !treat_combination ( combination ) ) { continue ; }
                
                // here we can create the mother particle, the vertex, 
                // check them and save in TES 
                if ( !treat_mother 
                     ( idecay->mother().pid() , 
                       combination            , 
                       combiner               , 
                       saved                  ) ) { continue ; }
                
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
        if ( !_ok_ ( c1 , c2 ) ) { continue ; } // CONTINUE, next c2 
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
        if ( !m_acut12.fun ( comb12 ) ) { continue ; } // CONTINUE, next c2 
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
          if ( !_ok_ ( c1 ,c2 , c3 ) ) { continue ; } // CONTINUE, next c3 
          //
          comb123     [2] = c3 ;
          comb1234    [2] = c3 ;
          comb12345   [2] = c3 ;
          comb123456  [2] = c3 ;
          combination [2] = c3 ;
          
          // Use combination cuts! it is a heart of all game: 
          // here we have the combination and can apply the cut:
          if ( !m_acut123.fun ( comb123 ) ) { continue ; } // CONTINUE, next c3
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
            if ( !_ok_ ( c1 ,c2 , c3 , c4 ) ) { continue ; } // CONTINUE, next c4 
            //
            comb1234    [3] = c4 ;
            comb12345   [3] = c4 ;
            comb123456  [3] = c4 ;
            combination [3] = c4 ;
            
            // Use combination cuts! it is a heart of all game: 
            // here we have the combination and can apply the cut:
            if ( !m_acut1234.fun ( comb1234 ) ) { continue ; } // CONTINUE, next c4
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
              if ( !_ok_ ( c1 ,c2 , c3 , c4 , c5 ) ) { continue ; } // CONTINUE, next c5
              //
              comb12345   [4] = c5 ;
              comb123456  [4] = c5 ;
              combination [4] = c5 ;
              
              // Use combination cuts! it is a heart of all game: 
              // here we have the combination and can apply the cut:
              if ( !m_acut12345.fun ( comb12345 ) ) { continue ; } // CONTINUE, next c5
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
                if ( !_ok_ ( c1 ,c2 , c3 , c4 , c5 , c6 ) )
                { continue ; } // CONTINUE, next c6
                //
                comb123456  [5] = c6 ;
                combination [5] = c6 ;               
                
             
                // Use combination cuts! it is a heart of all game: 
                // here we have the combination and can apply the cut:
                if ( !m_acut123456.fun ( comb123456 ) ) { continue ; } // CONTINUE, next c7
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
                  if ( !_ok_ ( c1 ,c2 , c3 , c4 , c5 , c6 , c7 ) )
                  { continue ; } // CONTINUE, next c6
                  //
                  combination [6] = c7 ;            
                  
                  //
                  // check the combinations for overlaps and cuts 
                  // 
                  if ( !treat_combination ( combination ) ) { continue ; }
                  
                  // here we can create the mother particle, the vertex, 
                  // check them and save in TES 
                  if ( !treat_mother 
                       ( idecay->mother().pid() , 
                         combination            , 
                         combiner               , 
                         saved                  ) ) { continue ; }
                  
                  // increment number of good decays
                  ++nGood;
                  
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
// The factories (needed for the proper instantiation)
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DaVinci,N3BodyDecays)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DaVinci,N4BodyDecays)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DaVinci,N5BodyDecays)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DaVinci,N6BodyDecays)
DECLARE_NAMESPACE_ALGORITHM_FACTORY(DaVinci,N7BodyDecays)

// ============================================================================
// The END 
// ============================================================================


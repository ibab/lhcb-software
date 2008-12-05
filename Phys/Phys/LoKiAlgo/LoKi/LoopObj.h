// $Id: LoopObj.h,v 1.13 2008-12-05 10:42:17 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_LOOPOBJ_H 
#define LOKI_LOOPOBJ_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/HashMap.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/ParticleProperty.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IParticleReFitter.h"
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/Kinematics.h"
#include "LoKi/Base.h"
#include "LoKi/Combiner.h"
#include "LoKi/Print.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysRangeTypes.h"
// ============================================================================
/** @file
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
 *  @date 2006-03-14 
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  // forward declaration 
  class Algo ;
  // ==========================================================================
  /** @class LoopObj LoopObj.h LoKi/LoopObj.h
   *  The main horse of LoKi-package
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-15
   */
  class LoopObj : public LoKi::Base 
  {
  public: 
    // ========================================================================
    /// the actual type of selection formula
    typedef std::vector<std::string>                       Formula ;
    /// the actual type of "Combiner" object
    typedef LoKi::Combiner_<LoKi::Types::Range::Container> Combiner ;
    /// the actual type of "current" combination
    typedef Combiner::Select                               Select   ;
    // ========================================================================
  public:
    // ========================================================================
    /// get the effective formula of the loop
    const Formula& formula() const { return m_formula ; }
    /// get the effective dimension of the loop (== number of components)
    size_t dim   () const      { return m_combiner.dim  () ; }
    /// Loop size (including ALL invalid combinations!)
    size_t size  () const      { return m_combiner.size () ; }
    /// get current combination from the cominer 
    const Select& current() const { return m_combiner.current(); }
    /// check the validity of the current combinations
    bool   valid () const      { return m_valid            ; }
    /// get the current status of the loop object 
    StatusCode status() const  { return m_status ; }
    /// advance the loop to the next combination
    LoopObj& operator++()      { return next() ; }
    /// advance the loop to the next combination
    LoopObj& operator++( int ) { return next() ; }
    /// conversion operator to bool (to be used in the loops)
    operator bool                    () const { return m_valid     ; }
    /// conversion operator to LHCb::Particle
    operator const LHCb::Particle*   () const { return particle () ; }
    /// conversion operator to LHCb::Vertex 
    operator const LHCb::Vertex*     () const { return vertex   () ; }
    /** get the daughter particle by index (the index starts from 1)
     *  It is an alias to the previouse method 
     *  @attention <c>index==0</c> corresponds to the particle itself
     *  Indices start from 1 
    *  @param index index of the daughter 
     */ 
    const LHCb::Particle* operator () ( const size_t index ) const 
    { return particle ( index ) ; }
    /// get the access to the combination
    const LHCb::Particle::ConstVector& combination() const 
    { return m_combination ; }
    /** get  particle/daughter particle from the current combination.
     *  It is an alias to the previouse method 
     *  @attention <c>index==0</c> corresponds to the particle itself
     *  Indices start from 1 
     *  @param index index of the daughter 
     */ 
    inline const LHCb::Particle* child ( const size_t index ) const ;
    /** get  particle/daughter particle from the current combination.
     *  It is an alias to the previouse method 
     *  @attention <c>index==0</c> corresponds to the particle itself 
     *  Indices start from 1 
     *  @param index index of the daughter 
     */ 
    inline const LHCb::Particle* daughter ( const size_t index ) const ;
    /** get the sum of LorentzVectors of all daughter paticles 
     *  @attention The result is DIFFERENT from the result of LoKi::LoopObj::momentum(0)
     *  @return lorentz vector of the combination 
     */
    inline LoKi::LorentzVector momentum (                      ) const ;    
    /** get lorentz vector of the particle/daugher particles 
     *  @param  index  index of the first  daugter particle
     *  @attention <c>index==0</c> corresponds to the particle 
     *  @return lorentz vector of the combination 
     */
    inline LoKi::LorentzVector momentum ( const size_t index   ) const ;
    /** get the sum of LorentzVectors of all daughter paticles 
     *  @attention The result is DIFFERENT from the result of LoKi::LoopObj::p(0)
     *  @return lorentz vector of the combination 
     */
    LoKi::LorentzVector p ( ) const { return momentum ( ) ; }
    /** get lorentz vector of the particle/daugher particles 
     *  @param  index  index of the first  daugter particle
     *  @attention <c>index==0</c> corresponds to the particle
     *  It is an alias to the previous method  
     *  @return lorentz vector of the combination 
     */
    LoKi::LorentzVector p        ( const size_t index     ) const 
    { return momentum ( index ) ; }
    /** get lorentz vector of combination of 2 daugher particles 
     *  @param  index1  index of the first  daugter particle
     *  @param  index2  index of the second daughter particle
     *  @attention <c>index==0</c> corresponds to the particle 
     *  @return lorentz vector of the combination 
     */
    inline LoKi::LorentzVector momentum 
    ( const size_t index1    , 
      const size_t index2    ) const ;
    /** get lorentz vector of combination of 2 daugher particles 
     *  @param  index1  index of the first  daugter particle
     *  @param  index2  index of the second daughter particle
     *  @attention <c>index==0</c> corresponds to the particle 
     *  It is an alias to the previous method  
     *  @return lorentz vector of the combination 
     */
    LoKi::LorentzVector p       
    ( const size_t index1    , 
      const size_t index2    ) const 
    { return momentum ( index1 , index2 ) ; }
    /** get lorentz vector of combination of 3 daugher particles 
     *  @param  index1  index of the first  daugter particle
     *  @param  index2  index of the second daughter particle
     *  @param  index3  index of the third  daughter particle
     *  @attention <c>index==0</c> corresponds to the particle 
     *  @return lorentz vector of the combination 
     */
    inline LoKi::LorentzVector momentum 
    ( const size_t index1    , 
      const size_t index2    , 
      const size_t index3    ) const ;
    /** get lorentz vector of combination of 3 daugher particles 
     *  @param  index1  index of the first  daugter particle
     *  @param  index2  index of the second daughter particle
     *  @param  index3  index of the third  daughter particle
     *  @attention <c>index==0</c> corresponds to the particle 
     *  It is an alias to the previous method  
     *  @return lorentz vector of the combination 
     */
    LoKi::LorentzVector p       
    ( const size_t index1    , 
      const size_t index2    , 
      const size_t index3    ) const 
    { return momentum ( index1 , index2 , index3 ); }
    /** get lorentz vector of combination of 4 daugher particles 
     *  @param  index1  index of the first   daugter particle
     *  @param  index2  index of the second  daughter particle
     *  @param  index3  index of the third   daughter particle
     *  @param  index4  index of the fourth  daughter particle
     *  @attention <c>index==0</c> corresponds to the particle 
     *  @return lorentz vector of the combination 
     */
    inline LoKi::LorentzVector momentum 
    ( const size_t index1    , 
      const size_t index2    , 
      const size_t index3    , 
      const size_t index4    ) const ;
    /** get lorentz vector of combination of 4 daughter particles 
     *  @param  index1  index of the first   daugter particle
     *  @param  index2  index of the second  daughter particle
     *  @param  index3  index of the third   daughter particle
     *  @param  index4  index of the fourth  daughter particle
     *  @attention <c>index==0</c> corresponds to the particle 
     *  It is an alias to the previous method  
     *  @return lorentz vector of the combination 
     */
    LoKi::LorentzVector p        
    ( const size_t index1    , 
      const size_t index2    , 
      const size_t index3    , 
      const size_t index4    ) const 
    { return momentum ( index1 , index2 , index3 , index4 ); }
    /** get the kinematical mass (from sum of Lorentz Vectors)
     *  for daughter particle with index  
     *  @attention <c>index==0</c> corresponds to the particle 
     */
    double           mass ( ) const { return momentum ().M () ; }    
    /** get the kinematical mass (from evaluated LorentzVector)
     *  for daughter particle with index  
     *  @attention <c>index==0</c> corresponds to the particle 
     */
    double           mass ( const size_t index ) const 
    { return momentum ( index ).M () ; }    
    /** get the kinematical mass (from evaluated LorentzVector)
     *  for pair of daughter particles with indices  
     *  @attention <c>index==0</c> corresponds to the particle 
     */
    double           mass     
    ( const size_t index1    , 
      const size_t index2    ) const 
    { return momentum ( index1 , index2  ).M() ; }    
    /** get the kinematical mass (from evaluated LorentzVector)
     *  for  daughter particles with indices  
     *  @attention <c>index==0</c> corresponds to the particle 
     */
    double           mass     
    ( const size_t index1    , 
      const size_t index2    , 
      const size_t index3    ) const 
    { return momentum ( index1 , index2 , index3 ).M() ; }
    /** get the kinematical mass (from evaluated LorentzVector)
     *  for  daughter particles with indices  
     *  @attention <c>index==0</c> corresponds to the particle 
     */
    double           mass    
    ( const size_t index1    , 
      const size_t index2    , 
      const size_t index3    , 
      const size_t index4    ) const 
    { return momentum ( index1 , index2 , index3 , index4 ).M() ; }
    /// get the effective particle of the combinations 
    inline LHCb::Particle*   particle ( const IParticleCombiner* c = 0 ) const ;
    /// get the effective vertex of the combiination 
    inline LHCb::Vertex*     vertex   ( const IParticleCombiner* c = 0 ) const ;
    /** get  particle/daughter particle from the current combination 
     *  @attention <c>index==0</c> corresponds to the particle 
     *  Indices start from 1 
     *  @param index index of the daughter 
     */ 
    inline const LHCb::Particle* particle ( const size_t index ) const ;
    // ========================================================================
  public :
    // ========================================================================
    /// get PID for the effective particle of the loop
    const LHCb::ParticleID& pid      () const { return m_pid     ; }
    /// get PID for the effective particle of the loop
    const LHCb::ParticleProperty* pp () const { return m_pp      ; }
    /// get PID for the effective particle of the loop
    const std::string&      pidName  () const { return m_pidname ; }
    // ========================================================================
  public:
    // ========================================================================
    /// set the particle ID for the effectiev particle of the loop 
    LoopObj& setPID ( const LHCb::ParticleID& pid ) ;
    /// set the particle ID for the effectiev particle of the loop 
    LoopObj& setPID ( const std::string&      pid ) ;
    /// set the particle ID for the effectiev particle of the loop 
    LoopObj& setPID ( const LHCb::ParticleProperty* pid ) ;
    // ========================================================================
  public:
    // ========================================================================
    /// set the default IParticleCombiner tool
    LoopObj& setCombiner ( const IParticleCombiner* c ) 
    { m_comb  = c ; return *this ; }
    /// set the default IParticleCombiner tool
    LoopObj& setReFitter ( const IParticleReFitter* c ) 
    { m_reFit = c ; return *this ; }
    // ========================================================================
  public:
    // ========================================================================
    /// get the associated vertex 
    const LHCb::VertexBase* pv () const { return m_pv ; }
    /// set the associated vertex 
    LoopObj& setPV ( const LHCb::VertexBase* v ) { m_pv = v ; return *this ; }    
    // ========================================================================
  public:
    // ========================================================================
    /// add the component to the Loop obejcts
    LoopObj& addComponent 
    ( const std::string&     name  , 
      const Combiner::Range& range ) ;
    // ========================================================================
  public:
    // ========================================================================
    /// make 'effective' particle using IParticleCombiner tool
    StatusCode make  ( const std::string& nick           ) const ;
    /// make 'effective' particle using IParticleCombiner tool
    StatusCode make  ( const IParticleCombiner* comb = 0 ) const ;
    /// refit the particle using IParticleReFitter tool
    StatusCode reFit ( const std::string& nick           ) const ;
    /// refit the particle using IParticleReFitter tool
    StatusCode reFit ( const IParticleReFitter* fit = 0  ) const ;
    /// save the particle into LoKi storage
    StatusCode save  ( const std::string& tag ) const ;
    // ========================================================================
  public:
    // ========================================================================
    /// backup the current state of the loop 
    LoopObj& backup  () ;
    /// restore the loop from the last backup state
    LoopObj& restore () ;
    // ========================================================================
  public:
    // ========================================================================
    /// advance to the next VALID combination 
    LoopObj&   next     () ;    
    // ========================================================================
  protected:    
    // ========================================================================
    /// make 'effective' particle & vertex from the current configuration
    inline StatusCode create ( const IParticleCombiner* c = 0 )  const ;
    /// estimate the validity of current combination
    bool isValid() ;
    /// reset all temporary caches 
    LoopObj& resetCache () ;    
    /// find clone particles 
    void     findClones () ;    
    /// check the correct order of clones  to avoid the multiple count
    bool clonesOrdered () const ;
    /// make an adaptation for new PID
    LoopObj&  adaptToPID() ;
    // ========================================================================
  public:
    // ========================================================================
    /** standard constructor
     *  @param name object name/description
     *  @param reporter error reporter
     *  @param algo the actual horse 
     *  @param combiner the default IParticleCombiner tool 
     *  @param fitter   the default IParticleReFitter tool 
     */
    LoopObj 
    ( const std::string&       name          , 
      const LoKi::IReporter*   reporter      ,
      const LoKi::Algo*        algo          , 
      const IParticleCombiner* combiner  = 0 ,
      const IParticleReFitter* fitter    = 0 ) ;
    // ========================================================================
  protected:
    // ========================================================================
    /// destructor 
    virtual ~LoopObj();
    // ========================================================================
  private:
    // ========================================================================
    /// default constructor is disabled 
    LoopObj();
    /// copy constructor is disabled 
    LoopObj           ( const LoopObj& right ) ;
    /// assignement operator is disabled 
    LoopObj& operator=( const LoopObj& right ) ;    
    // ========================================================================
  protected:
    // ========================================================================
    /// get the algorithm
    const LoKi::Interface<LoKi::Algo>& algo() const { return m_algo ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the algorithm ("owner")
    LoKi::Interface<LoKi::Algo>        m_algo  ; // the algorithm ("owner")
    /// creator of valid combinations
    LoKi::Interface<IParticleCombiner> m_comb  ; // creator of valid combinations
    /// re-fitter 
    LoKi::Interface<IParticleReFitter> m_reFit ; // re-fitter
    /// the Loop "formula"
    Formula  m_formula ; // the Loop "formula"
    /// the "Combiner"
    Combiner                    m_combiner ; // the "Combiner"
    /// current combination 
    LHCb::Particle::ConstVector m_combination ; // current combination 
    /// valid combinaton? 
    bool                        m_valid    ; // valid combinaton? 
    /// current status 
    mutable StatusCode          m_status   ; // current status 
    /// the effective particle 
    mutable LHCb::Particle*     m_particle ; // the effective particle 
    /// the effective vertex  
    mutable LHCb::Vertex*       m_vertex   ; // the effective vertex 
    /// is LoopObject an owner of the particle?
    mutable bool                m_pOwner   ; // owner?
    /// is LoopObject an owner of the vertex?
    mutable bool                m_vOwner   ; // owner? 
    /// PID of effective particle 
    LHCb::ParticleID            m_pid      ; // PID of effective aprticle 
    /// Particle Property for effective particle
    const LHCb::ParticleProperty* m_pp     ; // Particle Property
    /// Particle name for effective particle
    std::string                 m_pidname  ; // Particle name 
    /// Primary Vertex (nedded for some fits/refit)
    const LHCb::VertexBase*     m_pv       ; // Primary Vertex
    // the structure to avoid the multiple counts for 
    // combination of "the same" particle types
    // e.g  (K- (pi+ pi+) pi-) 
    typedef std::vector<unsigned long>                 CloneList ; 
    typedef GaudiUtils::HashMap<std::string,CloneList> Clones    ;
    Clones                                             m_clones  ;
    // ========================================================================
  };
  // ==========================================================================
}  // end of the namespace LoKi
// ============================================================================
#endif // LOKI_LOOPOBJ_H
// ============================================================================
// pick-up the inlined implementation 
// ============================================================================
#include "LoKi/LoopObj.icpp"
// ============================================================================
// The END 
// ============================================================================
  

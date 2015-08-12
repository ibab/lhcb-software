// $Id: $
// ============================================================================
#ifndef DAVINCITOOLS_PI0VETO_H 
#define DAVINCITOOLS_PI0VETO_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================
// DaVinciTools 
// ============================================================================
#include "FilterDesktop.h"
// ============================================================================
/** @namespace Pi0Veto
 *
 *  The useful namespace for  DaVinci components, 
 *  used for pi0-veto
 *
 *  @see Pi0Veto::Filter 
 *  @see Pi0Veto::Tagger
 *  @see Pi0Veto::Tagger2g
 *
 *  All components are based on 
 *  @see LoKi::Photons::pi0Veto 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date   2011-11-20
 *  
 *                    $Revision$
 *  Last Modification $Date$
 *                 by $Author$
 */
namespace Pi0Veto
{
  // ==========================================================================
  /** @class Filter
   *  The simplest component for 'pi0-veto'
   *  It just filters "good" photons:
   *  assuming the input contains all the photons, 
   *  it produces as output the list of single photons 
   *
   *  @code
   *  
   *  from Configurable import Pi0Veto__Filter as Fltr 
   *  
   *  single = Fltr( 
   *        ... , 
   *        MassWindow = 20 * MeV , 
   *        MassChi2   =  4       , 
   *        Inputs = [ 'Phys/StdLooseAllPhotons/Particles' ] , 
   *        ... 
   *        )
   *  
   *  @endcode 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date   2011-11-20
   */
  class Filter :  public FilterDesktop 
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<Pi0Veto::Filter> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the standard initialization 
    virtual StatusCode initialize () ;
    /** the major method for filter input particles 
     *  @param input    (INPUT) the input  container of particles
     *  @param filtered (OUPUT) the output container of particles
     *  @return Status code 
     */
    virtual StatusCode filter 
    ( const LHCb::Particle::ConstVector& input    , 
      LHCb::Particle::ConstVector&       filtered ) ;
    // ========================================================================
  protected:
    // ========================================================================
    /// update handler 
    void updateHandler20 ( Property& p ) ;
    void updateHandler21 ( Property& p ) ;
    void updateHandler22 ( Property& p ) ;
    // ========================================================================
  protected:
    // ========================================================================
    /// standard constructor
    Filter  ( const std::string& name , 
              ISvcLocator*       pSvc ) ;
    /// virtual and protected destructor
    virtual ~Filter () ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Filter  () ;                          // the default consructor is disabled
    /// copy constructor is disabled 
    Filter  ( const Filter& ) ;                 // copy constructor is disabled 
    /// assignement operator is disabled 
    Filter & operator=( const Filter& ) ;                     // no assignement
    // ========================================================================
  protected:
    // ========================================================================
    /// get the half-width of mass-window 
    double massWindow () const { return m_massWindow ; }
    /// get the chi2 of mass-window 
    double massChi2   () const { return m_massChi2   ; }
    /// get the chi2 of mass-window 
    double pi0Mass    () const { return m_pi0Mass    ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the mass-window for pi-veto 
    double          m_massWindow      ; // the mass-window for pi-veto 
    /// the mass-chi2   for pi-veto 
    double          m_massChi2        ; // the mass-chi2   for pi-veto 
    /// the pi0 mass to be used in veto 
    double          m_pi0Mass         ; // the pi0 mass to be used in veto 
    // ========================================================================
  } ;  
  // ==========================================================================
  /** @class Tagger 
   *  Simple class to check/tag the photons in recontructed decay trees 
   *  with resutl of LoKi::Photons::pi0Veto
   *  
   *  
   *  @code
   *
   *  from Configurable import Pi0Veto__Tagger as Tag
   *  
   *  single = Tag ( 
   *        ... , 
   *        ExtraInfo  = 25000     , 
   *        ... , 
   *        MassWindow = 20 * MeV , 
   *        MassChi2   =  4       , 
   *        ... ,
   *        Inputs  = [ 'Phys/MyRecontructedBdecays/Particles' ] , 
   *        Photons = [ 'Phys/StdLooseAllPhotons/Particles'    ]  
   *        ... 
   *        )
   *  
   *  @endcode 
   *
   *  The photons in decay tree will be tagged through their exraInfo field
   *
   *  @attention Please, be careful with redefintion of <c>Code</c>-property 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date   2011-11-20
   */
  class Tagger : public Filter 
  { 
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<Pi0Veto::Tagger> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the standard initialization 
    virtual StatusCode initialize () ;
    /** the major method for filter input particles 
     *  @param input    (INPUT) the input  container of particles
     *  @param filtered (OUPUT) the output container of particles
     *  @return Status code 
     */
    virtual StatusCode filter 
    ( const LHCb::Particle::ConstVector& input    , 
      LHCb::Particle::ConstVector&       filtered ) ;
    /// decode the cuts 
    StatusCode decodeCode () ;
    // ========================================================================
  protected:
    // ========================================================================
    /// standard constructor
    Tagger  ( const std::string& name , 
              ISvcLocator*       pSvc ) ;
    /// virtual and protected destructor
    virtual ~Tagger () ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Tagger  () ;                          // the default consructor is disabled
    /// copy constructor is disabled 
    Tagger  ( const Tagger& ) ;                 // copy constructor is disabled 
    /// assignement operator is disabled 
    Tagger & operator=( const Tagger& ) ;                     // no assignement
    // ========================================================================
  protected:
    // ========================================================================
    /// update handler 
    void updateHandler30 ( Property& p ) ;
    void updateHandler31 ( Property& p ) ;
    // ========================================================================
  protected:
    // ========================================================================
    /// get the cuts for the second photon
    const LoKi::Types::Cuts& photonCut() const { return m_photonCut ; }
    /** get the phtoons 
     *  @attention RootInTES is *NOT* used here (on purpose!)
     *  @param other (output) the photons 
     *  @return number of photons 
     */
    unsigned int getPhotons ( LHCb::Particle::ConstVector& other ) const ;
    /// get the index 
    unsigned int index      () const { return m_index ; }
    // ========================================================================
  private:
    // ========================================================================
    /// Extra-info index to be used 
    unsigned int      m_index ;    // Extra-info index to be used 
    /// the cut for "another" photon
    std::string       m_photonCutCode ; // the cut for "another" photon
    /// the cut for "another" photon
    LoKi::Types::Cut  m_photonCut     ; // the cut for "another" photon
    /// list of TES-locations for "other photons"
    std::vector<std::string> m_photons ;
    // ========================================================================
  } ;  
  // ==========================================================================
  /** @class Tagger2g 
   *  Simple class to check/tag the diphotons in recontructed decay trees 
   *  with result of LoKi::Photons::pi0Veto
   *  
   *  @code
   *
   *  from Configurable import Pi0Veto__Tagger2g as Tag
   *  
   *  single = Tag ( 
   *        ... , 
   *        ExtraInfoIndex  = 25000    , 
   *        ... , 
   *        MassWindow      = 20 * MeV , 
   *        MassChi2        =  4       , 
   *        ... ,
   *        Inputs  = [ 'Phys/MyRecontructedBdecays/Particles' ] , 
   *        Photons = [ 'Phys/StdLooseAllPhotons/Particles'    ]  
   *        ... 
   *        )
   *  
   *  @endcode 
   *
   *  The diphotons in decay tree will be tagged through their exraInfo field
   * 
   *  @attention Please, be careful with redefintion of <c>Code</c>-property 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
   *  @date   2011-11-20
   */
  class Tagger2g : public Tagger
  { 
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<Pi0Veto::Tagger2g> ;
    // ========================================================================
  public:
    // ========================================================================
    /** the major method for filter input particles 
     *  @param input    (INPUT) the input  container of particles
     *  @param filtered (OUPUT) the output container of particles
     *  @return Status code 
     */
    virtual StatusCode filter 
    ( const LHCb::Particle::ConstVector& input    , 
      LHCb::Particle::ConstVector&       filtered ) ;
    // ========================================================================
  protected:
    // ========================================================================
    /// standard constructor
    Tagger2g ( const std::string& name , 
               ISvcLocator*       pSvc ) ;
    /// virtual and protected destructor
    virtual ~Tagger2g () ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Tagger2g  () ;                        // the default consructor is disabled
    /// copy constructor is disabled 
    Tagger2g  ( const Tagger2g& ) ;             // copy constructor is disabled 
    /// assignement operator is disabled 
    Tagger2g & operator=( const Tagger2g& ) ;                 // no assignement
    // ========================================================================
  } ;  
  // ==========================================================================
} //                                                   end of namespace Pi0Veto 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // DAVINCITOOLS_PI0VETO_H
// ============================================================================

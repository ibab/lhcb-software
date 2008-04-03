// $Id: Decay.h,v 1.4 2008-04-03 12:19:20 ibelyaev Exp $
// ============================================================================
#ifndef DAVINCIKERNEL_DECAY_H 
#define DAVINCIKERNEL_DECAY_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <vector>
#include <string>
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/ParticleID.h"
// ============================================================================
/// forward decalrations 
// ============================================================================
class IDecodeSimpleDecayString ;
class IParticlePropertySvc     ;
class  ParticleProperty        ;
// ============================================================================
namespace DaVinci
{
  // ==========================================================================
  /** @class Decay Kernel/Decay.h
   *  The simple representation of "simple 1-step" decay (there are no trees!  
   * 
   *  @author  Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-03-31
   */
  class Decay 
  {
  public: 
    // ========================================================================
    /** @class Item 
     *  the helper representation of the item in the decay chain
     *  @author  Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-03-31
     */
    class Item 
    {
    public:
      // ======================================================================
      /// the constructor from the particle property 
      Item ( const ParticleProperty* pp = 0 ) ;
      /// the constructor from the particle name 
      Item ( const std::string&      name   ) ;
      /// the constructor from the particle PID
      Item ( const LHCb::ParticleID& pid    ) ;
      /// the constructor from the particle PID
      Item ( const int               pid    ) ;
      /// the destructor 
      ~Item() {} // the destructor 
      // ======================================================================
    public:
      // ======================================================================    
      /// get the particle name 
      const std::string&      name () const { return m_name ; }
      /// get the particle PID
      const LHCb::ParticleID& pid  () const { return m_pid  ; }
      /// get the particle property 
      const ParticleProperty* pp   () const { return m_pp   ; }
      // ======================================================================
    public:
      // ======================================================================    
      /// validate the item using the service 
      StatusCode validate (       IParticlePropertySvc* svc  ) const ;
      /// validate the item using the particle property object
      StatusCode validate ( const  ParticleProperty*    prop ) const ;
      // ======================================================================      
    private:
      // ======================================================================    
      /// the particle name 
      mutable std::string              m_name ; // the particle name 
      /// the particle PID 
      mutable LHCb::ParticleID         m_pid  ; // the particle PID 
      /// the source of properties 
      mutable const ParticleProperty*  m_pp   ; // the source of all properties 
      // ======================================================================    
    } ;
    // ========================================================================
    /// the vector of items (the obvious representation of daughter particles)
    typedef std::vector<Item> Items ;
    // ========================================================================
  public:
    // ========================================================================
    /// the default constructor 
    Decay () ;
    // ========================================================================    
    /// the constructor from mother and daughters 
    Decay 
    ( const                   ParticleProperty*   mother    ,   // the mother 
      const std::vector<const ParticleProperty*>& daughters ) ; // the daughtrers 
    /// the constructor from mother and daughters 
    Decay 
    ( const             std::string&              mother    ,   // the mother 
      const std::vector<std::string>&             daughters ) ; // the daughters 
    /// the constructor from mother and daughters 
    Decay 
    ( const             LHCb::ParticleID&         mother    ,   // the mother 
      const std::vector<LHCb::ParticleID>&        daughters ) ; // the daughters 
    /// the constructor from mother and daughters 
    Decay 
    ( const             int                       mother    ,   // the mother 
      const std::vector<int>&                     daughters ) ; // the daughters 
    // ========================================================================
  public:
    // ========================================================================
    /// virtual destructor 
    virtual ~Decay( ); // virtual destructor
    // ========================================================================
  public:
    // ========================================================================
    /// get the mother(head) of the decay 
    const Item&  mother     () const { return m_mother    ; }
    /// get all daughters 
    const Items& daughters  () const { return m_daughters ; }
    /// get all daughters 
    const Items& children   () const { return daughters() ; }
    /// get the number of daughters
    size_t       nDaughters () const { return m_daughters.size() ; }
    /// get the number of daughters
    size_t       nChildren  () const { return m_daughters.size() ; }      
    /** get the component by the number
     *  @attention index 0 corresponds to th emother particle
     *  @param index the index (0 corresponds to the mother particle) 
     *  @return the component 
     */
    const Item&  operator() ( const unsigned int index ) const 
    { 
      if ( 0 == index ) { return m_mother ; }
      return m_daughters [ index - 1 ] ;
    }
    /** get the component by the number
     *  @attention index 0 corresponds to th emother particle
     *  @param index the index (0 corresponds to the mother particle) 
     *  @return the component 
     */
    const Item&  operator[] ( const unsigned int index ) const 
    { return (*this) ( index ) ; }
    // ========================================================================    
  public:    
    // ========================================================================    
    /// set the mother 
    void setMother    ( const Item&             mom ) { m_mother =        mom   ; }
    /// set the mother 
    void setMother    ( const ParticleProperty* mom ) { m_mother = Item ( mom ) ; }
    /// set the mother 
    void setMother    ( const std::string&      mom ) { m_mother = Item ( mom ) ; }
    /// set the mother 
    void setMother    ( const LHCb::ParticleID& mom ) { m_mother = Item ( mom ) ; }
    /// set the mother 
    void setMother    ( const int               mom ) { m_mother = Item ( mom ) ; }
    /// set the daughters 
    void setDaughters ( const Items& daugs )  { m_daughters = daugs ;  }
    /// set the daughters 
    void setDaughters ( const std::vector<const ParticleProperty*>& daugs ) ;
    /// set the daughters 
    void setDaughters ( const std::vector<std::string>&             daugs ) ;
    /// set the daughters
    void setDaughters ( const std::vector<LHCb::ParticleID>&        daugs ) ;
    /// set the daughters 
    void setDaughters ( const std::vector<int>&                     daugs ) ;
    /// set the daughters 
    void setChildren  ( const Items& daugs ) { setDaughters ( daugs ) ; }
    /// set the daughters  
    void setChildren  ( const std::vector<const ParticleProperty*>& daugs ) 
    { setDaughters ( daugs ) ; }
    /// set the daughters  
    void setChildren  ( const std::vector<std::string>&             daugs ) 
    { setDaughters ( daugs ) ; }
    /// set the daughters  
    void setChidlren  ( const std::vector<LHCb::ParticleID>&        daugs ) 
    { setDaughters ( daugs ) ; }
    /// set the daughters  
    void setChildren  ( const std::vector<int>&                     daugs ) 
    { setDaughters ( daugs ) ; }
    // ========================================================================    
  public:    
    // ========================================================================    
    /// validate the decay using the service 
    StatusCode validate ( IParticlePropertySvc* svc ) const ;
    // ========================================================================    
  public:    
    // ========================================================================    
    /// the default printout 
    virtual std::ostream& fillStream ( std::ostream& s ) const ;
    /// the conversion to the string
    std::string   toString   () const ;
    // ========================================================================    
  private:
    // ========================================================================
    /// the head of the decay
    mutable Item  m_mother    ; // the head of the decay
    /// the daughter particles 
    mutable Items m_daughters ; // the daughter particles 
    // ========================================================================
  };
  // ==========================================================================
  /// the actual type for the vector of decays 
  typedef std::vector<DaVinci::Decay>                                  Decays ;
  // ==========================================================================
  namespace Utils
  {
    // ========================================================================
    /** simple function for filtering (by PID) the particles, which 
     *  matched the PID
     *  
     *  @code
     *  
     *   const LHCb::Particle::ConstVector input = ... ;
     *   const DaVinci::Decay& decay = ... ;
     *
     *   LHCb::Particle::ConstVector result ;
     * 
     *   // use the function:
     *
     *   DaVinci::Utils::getParticles 
     *            ( input . begin () , 
     *              input . end   () , 
     *              decay ( 1 )      , // get the first component 
     *              std::back_inserter ( result ) ) ;
     *
     *  @endcode 
     *
     *  It is also applicable to Monte-Carlo data:
     *
     *  @code
     *  
     *   const LHCb::MCParticle::ConstVector input = ... ;
     *   const DaVinci::Decay& decay = ... ;
     *
     *   LHCb::MCParticle::ConstVector result ;
     * 
     *   // use the function:
     *
     *   DaVinci::Utils::getParticles 
     *            ( input . begin () , 
     *              input . end   () , 
     *              decay ( 1 )      , // get the first component 
     *              std::back_inserter ( result ) ) ;
     *
     *  @endcode 
     *
     *  @param begin the begin iterator for the input sequence  
     *  @param end   the end   iterator for the input sequence  
     *  @param item  the decay component 
     *  @param result the output iterator 
     *  @param return the updated output iterator 
     *
     *  @author  Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-03-31
     */
    template <class INPUT , class OUTPUT>
    inline 
    OUTPUT 
    getParticles 
    ( INPUT                       begin  , 
      INPUT                       end    ,
      const DaVinci::Decay::Item& item   , 
      OUTPUT                      result ) 
    {
      for ( ; begin != end ; ++begin ) 
      {
        if ( ! (*begin)        ) { continue ; }   // CONTINUE 
        const LHCb::ParticleID& pid = (*begin)->particleID() ;
        if ( pid != item.pid() ) { continue ; }   // CONTINUE 
        *result = *begin ;
        ++result ;
      }
      return result ;
    }
    // ========================================================================
  } // end of namespace DaVinci::Utils
  // ==========================================================================
  /** create decay object from the descriptor
   *  @attention only "main" decay is created!
   *  @param descriptor the decay descriptor 
   *  @param decode the decoder tool
   *  @return the constructed decay
   */
  DaVinci::Decay decay 
  ( const std::string&        descriptor , 
    IDecodeSimpleDecayString* decoder    ) ;
  // ========================================================================    
  /** create all decay objects from the descriptor
   *  @param descriptor the decay descriptor 
   *  @param decode the decoder tool
   *  @return the constructed decay
   */
  DaVinci::Decays decays
  ( const std::string&        descriptor , 
    IDecodeSimpleDecayString* decoder    ) ;
  /** create all decay objects from the descriptor
   *  @param descriptors the decay descriptors 
   *  @param decode the decoder tool
   *  @return the constructed decay
   */
  DaVinci::Decays decays
  ( const std::vector<std::string>& descriptors , 
    IDecodeSimpleDecayString*       decoder     ) ;
  // ========================================================================    
} // end of namespace DaVinci
// ============================================================================
/// the printout operator to the stream 
inline std::ostream& operator<<
( std::ostream& s , const DaVinci::Decay& decay ) 
{ return decay.fillStream ( s ) ; }
// ============================================================================
// The END
// ============================================================================
#endif // DAVINCIKERNEL_DECAY_H
// ============================================================================

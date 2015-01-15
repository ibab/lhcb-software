// $Id$
// ============================================================================
#ifndef LOKIMC_MCCHILDSELECTOR_H 
#define LOKIMC_MCCHILDSELECTOR_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// Kernel/PartProp
// ============================================================================
#include "Kernel/iNode.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/IMCDecay.h"
#include "LoKi/AuxFunBase.h"
#include "LoKi/MCTypes.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace MCChild 
  {
    // ========================================================================
    /** @class Selector LoKi/MCChildSelector.h
     *  Helper class to select the certain child particle 
     *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
     *  @date   2010-05-29
     */
    class GAUDI_API Selector : public virtual LoKi::AuxFunBase 
    {
    public:
      // ======================================================================
      /// constructor from the index
      Selector ( const unsigned int   i  ) ;
      /// constructor from the indices 
      Selector ( const unsigned int   i1 ,
                 const unsigned int   i2 ) ;
      /// constructor from the indices 
      Selector ( const unsigned int   i1 ,
                 const unsigned int   i2 ,
                 const unsigned int   i3 ) ;
      /// constructor from the indices 
      Selector ( const unsigned int   i1 ,
                 const unsigned int   i2 ,
                 const unsigned int   i3 ,
                 const unsigned int   i4 ) ;
      /// constructor from the indices 
      Selector ( const std::vector<unsigned int>& indices          ) ;
      /// constructor from decay node 
      Selector ( const Decays::iNode&             node             , 
                 const bool                       decayOnly = true ) ;
      /// constructor from decay tree 
      Selector ( const Decays::IMCDecay::iTree&   child            ) ;
      /// constructor from decay tree 
      Selector ( const Decays::IMCDecay::Finder&  child            ) ;
      /// constructor from decay desctriptor 
      Selector ( const std::string&               child            ) ;
      /// constructor from decay desctriptor & factory
      Selector ( const std::string&               child              , 
                 const std::string&               factory          ) ;
      /// constructor from the cut 
      Selector ( const LoKi::MCTypes::MCCuts&     cut              , 
                 const bool                       decayOnly = true ) ;
      /// desctructor  
      virtual ~Selector() ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /** get the child according to the selector 
       *  @param  head (INPUT) the head fo the decay tree 
       *  @return child particle 
       */
      const LHCb::MCParticle* child ( const LHCb::MCParticle* head ) const ;
      /** get the child according to the selector 
       *  @param head      (INPUT) the head fo the decay tree 
       *  @param daughters (UPDATE) 
       *  @return number of particles 
       */
      unsigned int children 
      ( const LHCb::MCParticle*         head      , 
        LHCb::MCParticle::ConstVector&  daughters ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// check the validity the child-selector 
      bool valid () const  ;
      /// try to validate it!
      StatusCode validate ( const LHCb::IParticlePropertySvc* svc  = 0 ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Selector () ;                      // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// build the decay tree form the descriptor 
      StatusCode buildTree ( const std::string& descriptor , 
                             const std::string& factory    ) const ;
      
      // ======================================================================
    private:
      // ======================================================================
      /// the child selector by index 
      std::vector<unsigned int> m_indices   ;    // the child selector by index 
      /// rule to find the child 
      mutable Decays::IMCDecay::Finder  m_finder ;    // rule to find the child 
      std::string               m_decay     ;
      /// cut set 
      bool                      m_setCut    ;    //                     cut set
      /// the cut 
      LoKi::MCTypes::MCCut      m_cut       ;    //                     the cut
      /// decay only ?
      bool                      m_decayOnly ;    // decay only ?
      std::string               m_factory   ;
      // ======================================================================
    };
    // ========================================================================
  } //                                           end of namespace LoKi::MCChild 
  // ==========================================================================
  namespace Child 
  {
    // ========================================================================
    /** @typedef MCChidlSelector
     *  @see LoKi::MCChild::Selector
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2010-05-31
     */
    typedef LoKi::MCChild::Selector                           MCChildSelector ;
    // ========================================================================
  } //                                             end of namespace LoKi::Child 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKIMC_MCCHILDSELECTOR_H
// ============================================================================

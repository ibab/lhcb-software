// $Id$
// ============================================================================
#ifndef LOKI_CHILDSELECTOR_H 
#define LOKI_CHILDSELECTOR_H 1
// ============================================================================
// Include fiels  
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// Kernel/PartProp
// ============================================================================
#include "Kernel/iNode.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/IDecay.h"
#include "LoKi/AuxFunBase.h"
#include "LoKi/PhysTypes.h"
#include "LoKi/Child.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Child
  {
    // =========================================================================
    /** @class Selector LoKi/ChildSelector.h
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
      Selector ( const std::vector<unsigned int>& indices  ) ;
      /// constructor from decay node 
      Selector ( const Decays::iNode&             node     ) ;
      /// constructor from decay tree 
      Selector ( const Decays::IDecay::iTree&     child    ) ;
      /// constructor from decay tree 
      Selector ( const Decays::IDecay::Finder&    child    ) ;
      /// constructor from decay desctriptor 
      Selector ( const std::string&               child    ) ;
      /// constructor from decay desctriptor & factory
      Selector ( const std::string&               child    , 
                 const std::string&               factory  ) ;
      /// constructor from the cut 
      Selector ( const LoKi::Types::Cuts&         cut      ) ;
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
      const LHCb::Particle* child ( const LHCb::Particle* head ) const ;
      /** get the child according to the selector 
       *  @param head      (INPUT) the head fo the decay tree 
       *  @param daughters (UPDATE) 
       *  @return number of particles 
       */
      unsigned int children 
      ( const LHCb::Particle*         head      , 
        LHCb::Particle::ConstVector&  daughters ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// check the validity the child-selector 
      bool valid () const  ;
      /// try to validate it!
      StatusCode validate ( const LHCb::IParticlePropertySvc* svc ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      Selector () ;                      // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// build the decay tree form teh descriptor 
      StatusCode buildTree ( const std::string& descriptor , 
                             const std::string& factory    ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// the child selector by index 
      std::vector<unsigned int> m_indices ;       // the child selector by index 
      /// rule to find the child 
      Decays::IDecay::Finder    m_finder  ;       //      rule to find the child 
      /// cut set 
      bool                      m_setCut  ;       //                     cut set
      /// the cut 
      LoKi::Types::Cut          m_cut     ;       //                     the cut
      // ======================================================================
    };
    // ========================================================================
  } //                                             end of namespace LoKi::Child
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_CHILDSELECTOR_H
// ============================================================================

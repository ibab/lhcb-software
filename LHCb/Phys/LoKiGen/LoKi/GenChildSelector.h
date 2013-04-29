// $Id$
// ============================================================================
#ifndef LOKI_GENCHILDSELECTOR_H 
#define LOKI_GENCHILDSELECTOR_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Kernel/PartProp
// ============================================================================
#include "Kernel/iNode.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/iTree.h"
#include "LoKi/IGenDecay.h"
#include "LoKi/AuxFunBase.h"
#include "LoKi/GenTypes.h"
// ============================================================================
/** @file LoKi/GenChildSelector.h
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
 *  @date 2011-12-11
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace GenChild
  {
    // ========================================================================
    /** @class Selector LoKi/GenChildSelector.h
     *  
     *  "Child-selector" for HepMC-particles
     *
     *  This file is a part of LoKi project - 
     *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
     *
     *  The package has been designed with the kind help from
     *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
     *  contributions and advices from G.Raven, J.van Tilburg, 
     *  A.Golutvin, P.Koppenburg have been used in the design.
     *
     *  @author Vanya Belyaev  Ivan.Belyaev@cern.ch
     *  @date   2011-12-13
     */
    // ========================================================================
    class GAUDI_API Selector : public virtual LoKi::AuxFunBase 
    {
    public:
      // ======================================================================
      /// the vector of selectors 
      typedef std::vector<LoKi::GenChild::Selector>                    Vector ;
      // ======================================================================
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
      Selector ( const Decays::iNode&             node             ) ;
      /// constructor from decay tree 
      Selector ( const Decays::IGenDecay::iTree&  child            ) ;
      /// constructor from decay tree 
      Selector ( const Decays::IGenDecay::Finder& child            ) ;
      /// constructor from decay desctriptor 
      Selector ( const std::string&               child            ) ;
      /// constructor from decay desctriptor & factory
      Selector ( const std::string&               child              , 
                 const std::string&               factory          ) ;
      /// constructor from the cut 
      Selector ( const LoKi::Types::GCuts&        cut              ) ;
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
      const HepMC::GenParticle* child ( const HepMC::GenParticle* head ) const ;
      /** get the child according to the selector 
       *  @param head      (INPUT) the head fo the decay tree 
       *  @param daughters (UPDATE) 
       *  @return number of particles 
       */
      unsigned int children 
      ( const HepMC::GenParticle*     head      , 
        LoKi::GenTypes::ConstVector&  daughters ) const ;
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
      /// build the decay tree form the descriptor 
      StatusCode buildTree ( const std::string& descriptor , 
                             const std::string& factory    ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// the child selector by index 
      std::vector<unsigned int> m_indices   ;    // the child selector by index 
      /// rule to find the child 
      Decays::IGenDecay::Finder m_finder    ;    //      rule to find the child 
      /// cut set 
      bool                      m_setCut    ;    //                     cut set
      /// the cut 
      LoKi::Types::GCut         m_cut       ;    //                     the cut
      // ======================================================================
    };
    // ========================================================================
  } //                                          end of namespace LoKi::GenChild
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_GENCHILDSELECTOR_H
// ============================================================================

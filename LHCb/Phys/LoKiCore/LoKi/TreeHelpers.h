// $Id: TreeHelpers.h,v 1.1 2009-05-22 18:12:36 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_TREEHELPERS_H 
#define LOKI_TREEHELPERS_H 1
// ============================================================================
// Include files
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/iNode.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/DecayDescriptor.h"
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  namespace Parsers 
  {
    // ========================================================================
    /** @class Tree 
     *  simple (type-neutral) representation of the decay tree 
     *  Helper class for implementation of actual trees 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-22
     */
    class Tree 
    {
    public : 
      // ======================================================================
      /// the vector of trees 
      typedef std::vector<Decays::Parsers::Tree>                        Trees ;
      // ======================================================================
    public:
      // ======================================================================
      /// default constructor 
      Tree () ;
      /// constructor from the decay head  
      Tree ( const Decays::iNode&               head       ) ;
      // ======================================================================
    public: // operators 
      // ======================================================================
      /// add to the daughters 
      Tree& operator+= ( const Tree&          tree      ) ;
      Tree& operator+= ( const Decays::iNode& node      ) ;
      Tree& operator+= ( const Trees&         trees     ) ;
      /// add to the optional 
      Tree& operator%= ( const Tree&          tree      ) ;
      Tree& operator%= ( const Decays::iNode& node      ) ;
      Tree& operator%= ( const Trees&         trees     ) ;
      /// OR
      Tree& operator|= ( const Tree&          tree      ) ;
      Tree& operator|= ( const Decays::iNode& node      ) ;
      Tree& operator|= ( const Trees&         trees     ) ;
      /// AND
      Tree& operator&= ( const Tree&          tree      ) ;
      Tree& operator&= ( const Decays::iNode& node      ) ; 
      Tree& operator&= ( const Trees&         trees     ) ;
     // ======================================================================
    public: // configuration 
      // =====================================================================
      Tree& operator += ( const Decays::Trees::Arrow&       arr  ) ;
      Tree& operator += ( const Decays::Trees::Oscillation& osc  ) ;
      // inclusive 
      Tree& operator += ( const bool                        inc  ) ;
      // negated
      Tree& operator *= ( const bool                        neg  ) ;
      // marked
      Tree& operator /= ( const bool                        mark ) ;
      // =====================================================================
    public:
      // =====================================================================
      const Decays::iNode&       head       () const { return m_head.node () ; }
      Decays::Trees::Arrow       arrow      () const { return m_arrow        ; }
      Decays::Trees::Oscillation oscillated () const { return m_oscillated   ; }
      bool                       inclusive  () const { return m_inclusive    ; }
      bool                       negated    () const { return m_negated      ; }
      bool                       marked     () const { return m_marked       ; }
      const Trees&  ored     () const { return m_or       ; }
      const Trees&  anded    () const { return m_and      ; }
      const Trees&  children () const { return m_children ; }      
      const Trees&  optional () const { return m_optional ; }      
      // ======================================================================
    public:
      // ======================================================================
      std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the decay head (single node) 
      Decays::Node  m_head ;
      /// vector of OR-ed  trees 
      Trees m_or            ;
      /// vector of AND-ed trees 
      Trees m_and           ;
      // ======================================================================
    private:
      // ======================================================================
      /// the arrow type 
      Decays::Trees::Arrow       m_arrow      ;                   // decay type 
      /// oscillated?
      Decays::Trees::Oscillation m_oscillated ;                 // oscillated ?
      /// inclusive 
      bool                       m_inclusive  ;                 //    inclusive
      /// negation 
      bool                       m_negated    ;                 //    negation?
      /// mark 
      bool                       m_marked     ;                 //      marked?
      // ======================================================================
    private:
      // ======================================================================
      /// children
      Trees m_children ;                                    //         children
      /// optional nodes 
      Trees m_optional ;                                    //   optional nodes 
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace Decays
  // ==========================================================================
} // end of namespace Parsers 
// ============================================================================
std::ostream& operator<<( std::ostream& s , const Decays::Parsers::Tree& t ) ;
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TREEHELPERS_H
// ============================================================================

// $Id: GenVertices.h,v 1.3 2006-05-02 14:30:27 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.3 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef LOKI_GENVERTICES_H 
#define LOKI_GENVERTICES_H 1
// ============================================================================
// Include files
// ============================================================================
// HepMC
// ============================================================================
#include "HepMC/GenVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/Constants.h"
#include "LoKi/Functions.h"
#include "LoKi/GenTypes.h"
// ============================================================================

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
 *  @date 2006-02-08 
 */
// ============================================================================

namespace LoKi 
{
  /** @namespace GenVertices GenVertices.h LoKi/GenVertices.h
   *  
   *  Collection of LoKi functions and predicates to manipulate with 
   *  generator (HepMC) information 
   *
   *  @author Vanya  BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-02-08
   */
  namespace GenVertices 
  {
    
    /** @struct BarCode 
     *  the most primitive function - it return the "barcode" 
     *  of HepMC::GenVertex object
     *
     *  @see LoKi::Cuts::GVBAR
     *  @see HepMC::GenVertex
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    struct BarCode : 
      public LoKi::Function<const HepMC::GenVertex*> 
    {
    public:
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  BarCode* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~BarCode() ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    } ;
    
    
    /** @struct PositionX
     *
     *  the most primitive function - it return the position 
     *  of HepMC::GenVertex object
     *
     *  @see LoKi::Cuts::GVX 
     *  @see HepMC::GenVertex
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    struct PositionX : 
      public LoKi::Function<const HepMC::GenVertex*> 
    {
    public:
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  PositionX* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~PositionX() ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    } ;

    /** @struct PositionY
     *
     *  the most primitive function - it return the position 
     *  of HepMC::GenVertex object
     *
     *  @see LoKi::Cuts::GVY 
     *  @see HepMC::GenVertex
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    struct PositionY : 
      public LoKi::Function<const HepMC::GenVertex*> 
    {
    public:
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  PositionY* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~PositionY() ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    } ;

    /** @struct PositionZ
     *
     *  the most primitive function - it return the position 
     *  of HepMC::GenVertex object
     *
     *  @see LoKi::Cuts::GVZ
     *  @see HepMC::GenVertex
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    struct PositionZ : 
      public LoKi::Function<const HepMC::GenVertex*> 
    {
    public:
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  PositionZ* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~PositionZ() ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    } ;

    /** @struct PositionT
     *
     *  the most primitive function - it return the time component of 
     *  the position for HepMC::GenVertex object
     *
     *  @see LoKi::Cuts::GVT
     *  @see HepMC::GenVertex
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    struct PositionT : 
      public LoKi::Function<const HepMC::GenVertex*> 
    {
    public:
      /// MANDATORY: clone method ("virtual" constructor")
      virtual  PositionT* clone() const ;
      /// MANDATORY: virtual destructor 
      virtual ~PositionT() ;
      /// MANDATORY: the only one essential method 
      virtual  result_type operator() ( argument p ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    } ;
    
    /** @class CountIF
     *  simple class which counts how many particles 
     *  satisfy the certain criteria
     *
     *  @see LoKi::Cuts::GVCOUNT
     *  @see HepMC::GenParticle
     *  @see HepMC::GenVertex
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    class CountIF
      : public LoKi::Function<const HepMC::GenVertex*> 
    {
    public:
      /** constructor from the criteria and "range"
       *  @param cut the criteria
       *  @param range search region 
       *  @see HepMC::IteratorRange 
       */
      CountIF 
      ( const LoKi::Types::GCuts& cut   , 
        HepMC::IteratorRange      range ) ;
      /// copy constructor 
      CountIF ( const CountIF& right ) ;
      /// virtual destructor
      virtual ~CountIF();
      /// MANDATORY: clone method ("virtual contructor")
      virtual CountIF* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      CountIF () ;
    private:
      LoKi::Types::GCut    m_cut   ;
      HepMC::IteratorRange m_range ;
    } ;
    
    /** @class SumIF
     *  simple class which accumulates the function values 
     *  for particles from the given range which 
     *  satisfy the certain criteria
     *
     *  @see LoKi::Cuts::GVSUM
     *  @see HepMC::GenParticle
     *  @see HepMC::GenVertex
     *
     *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
     *  @date   2005-03-26
     */
    class SumIF
      : public LoKi::Function<const HepMC::GenVertex*> 
    {
    public:
      /** constructor from the criteria and "range"
       *  @param cut the criteria
       *  @param range search region 
       *  @see HepMC::IteratorRange 
       */
      SumIF 
      ( const LoKi::Types::GFunc& fun   ,
        HepMC::IteratorRange      range , 
        const LoKi::Types::GCuts& cut   = 
        LoKi::BooleanConstant<const HepMC::GenParticle*>(true) ) ;
      /// copy constructor 
      SumIF ( const SumIF& right ) ;
      /// virtual destructor
      virtual ~SumIF();
      /// MANDATORY: clone method ("virtual contructor")
      virtual SumIF* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument v ) const ;
      /// "SHORT" representation, @see LoKi::AuxFunBase 
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      SumIF () ;
    private:
      LoKi::Types::GFun    m_fun   ;
      LoKi::Types::GCut    m_cut   ;
      HepMC::IteratorRange m_range ;
    } ;

  } ; // end of namespace LoKi::GenVertices 
  
} ; // end of namespace LoKi 

// ============================================================================
// The END
// ============================================================================
#endif // LOKI_GENVERTICES_H
// ============================================================================

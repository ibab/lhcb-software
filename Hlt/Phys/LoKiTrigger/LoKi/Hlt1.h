// $Id: Hlt1.h,v 1.2 2008-11-13 22:11:03 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_HLT1_H 
#define LOKI_HLT1_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
// TrackEvent/RecEvent
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltSelection.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Interface.h"
#include "LoKi/TrackTypes.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hlt1
  {
    // ========================================================================
    /** @class TrSelection
     *  simple functor which acts as a source for the tracks
     *  Tracks are extracted from IHltDataSvc
     *  @see IHltDataSvc
     *  @see LoKi::IHltUnit
     *  @see LoKi::Cuts::TsSELECTION 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-10
     */
    class TrSelection : public LoKi::BasicFunctors<LHCb::Track*>::Source
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the selection 
      TrSelection ( const Hlt::TSelection<LHCb::Track>* selection ) ;
      /// constructor from the selection 
      TrSelection ( const std::string&  selection ) ;
      /// constructor from the selection 
      TrSelection ( const Hlt::Selection* selection ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TrSelection() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrSelection* clone() const { return new TrSelection(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ()  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the selection 
      const Hlt::TSelection<LHCb::Track>* selection () const 
      { return m_selection ; }
      /// get the selection name 
      const std::string& selName   () const { return m_selName.str() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      TrSelection () ;
      // ======================================================================
    private:
      // ======================================================================
      /// the selection itself 
      const Hlt::TSelection<LHCb::Track>* m_selection ;
      stringKey                           m_selName   ;
      // ======================================================================
    };
    // ========================================================================
    /** @class TrRegister
     *  simple functor which register its input in Hlt Store 
     *  Tracks are copied to IHltDataSvc
     *  @see IHltDataSvc
     *  @see LoKi::IHltUnit
     *  @see LoKi::Cuts::TrREGISTER
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-10
     */
    class TrRegister : public LoKi::BasicFunctors<LHCb::Track*>::Pipe
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the selection 
      TrRegister ( const std::string&  selection ) ;
      /// MANDATORY: virtual destructor 
      virtual ~TrRegister() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  TrRegister* clone() const { return new TrRegister(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a )  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the selection 
      const Hlt::TSelection<LHCb::Track>* selection () const 
      { return m_selection ; }
      /// get the selection name 
      const std::string& selName () const { return m_selName.str() ; }
      const stringKey& output    () const { return m_selName       ; }
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      TrRegister() ;
      // ======================================================================
    private:
      // ======================================================================
      /// the selection itself 
      Hlt::TSelection<LHCb::Track>* m_selection ;
      stringKey                     m_selName   ;
      // ======================================================================
    };
    // ========================================================================
    /** @class RvSelection 
     *  simple functor which acts as a source for the vertices
     *  Vertices are extracted from IHltDataSvc
     *  @see IHltDataSvc
     *  @see LoKi::IHltUnit
     *  @see LoKi::Cuts::RvSELECTION
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-10
     */
    class RvSelection : public LoKi::BasicFunctors<LHCb::RecVertex*>::Source
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the selection 
      RvSelection ( const Hlt::TSelection<LHCb::RecVertex>* selection ) ;
      /// constructor from the selection 
      RvSelection ( const std::string&  selection ) ;
      /// constructor from the selection 
      RvSelection ( const Hlt::Selection* selection ) ;
      /// MANDATORY: virtual destructor 
      virtual ~RvSelection() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RvSelection* clone() const { return new RvSelection(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ()  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the selection 
      const Hlt::TSelection<LHCb::RecVertex>* selection () const 
      { return m_selection ; }
      /// get the selection name 
      const std::string& selName   () const { return m_selName.str() ; }
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      RvSelection () ;
      // ======================================================================
    private:
      // ======================================================================
      /// the selection itself 
      const Hlt::TSelection<LHCb::RecVertex>* m_selection ;
      stringKey                               m_selName   ;
      // ======================================================================
    };
    // ========================================================================
    /** @class RvRegister
     *  simple functor which register its input in Hlt Store 
     *  Vertices are copied to IHltDataSvc
     *  @see IHltDataSvc
     *  @see LoKi::IHltUnit
     *  @see LoKi::Cuts::RvREGISTER
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-11-10
     */
    class RvRegister : public LoKi::BasicFunctors<LHCb::RecVertex*>::Pipe
    {
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the selection 
      RvRegister ( const std::string&  selection ) ;
      /// MANDATORY: virtual destructor 
      virtual ~RvRegister() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  RvRegister* clone() const { return new RvRegister(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a )  const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the selection 
      const Hlt::TSelection<LHCb::RecVertex>* selection () const 
      { return m_selection ; }
      /// get the selection name 
      const std::string& selName   () const { return m_selName.str(); }
      const stringKey&   output    () const { return m_selName      ; }
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled
      RvRegister() ;
      // ======================================================================
    private:
      // ======================================================================
      /// the selection itself 
      Hlt::TSelection<LHCb::RecVertex>* m_selection ;
      stringKey                         m_selName   ;
      // ======================================================================
    };
    // ========================================================================
  } // end of namespace LoKi::Hlt1
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_HLT1_H
// ============================================================================

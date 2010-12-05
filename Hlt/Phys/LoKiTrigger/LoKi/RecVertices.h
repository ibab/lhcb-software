// $Id$
// ============================================================================
#ifndef LOKI_RECVERTICES_H 
#define LOKI_RECVERTICES_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ExtraInfo.h"
#include "LoKi/TriggerTypes.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @namespace LoKi::RecVertices
   *  Collection of  functions specifir for class LHCb::RecVertex
   *  @see LHCb::RecVertex
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2007-08-11
   */
  namespace RecVertices
  {
    // ========================================================================
    /** @class HasInfo
     *  Trivial predicate which evaluates LHCb::RecVertex::hasInfo
     *  function
     *
     *  It relies on the method LoKi::Info::hasInfo
     *
     *  @see LHCb::RecVertex
     *  @see LoKi::Cuts::RVHASINFO 
     *  @see LoKi::ExtraInfo::CheckInfo
     *  @see LoKi::ExtraInfo::hasInfo
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class HasInfo : public LoKi::ExtraInfo::CheckInfo<LHCb::RecVertex>
    {
    public:
      /** constructor from "info"
       *  @param key info index/mark/key
       */
      HasInfo ( const int key ) ;
      /// copy constructor 
      HasInfo ( const HasInfo& right ) ;
      /// destructor 
      virtual ~HasInfo(){} ;
      /// clone method (mandatory!)
      virtual HasInfo* clone() const { return new HasInfo(*this) ; }
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      HasInfo();
    };
    // ========================================================================    
    /** @class Info
     *  Trivial function which evaluates LHCb::RecVertex::info
     *  
     *  It relies on the method LoKi::ExtraInfo::info 
     *
     *  @see LHCb::RecVertex
     *  @see LoKi::Cuts::RVINFO 
     *  @see LoKi::ExtraInfo::GeInfo
     *  @see LoKi::ExtraInfo::info
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class Info : public LoKi::ExtraInfo::GetInfo<LHCb::RecVertex>
    {
    public:
      /** constructor from "info"
       *  @param key info index/mark/key
       *  @param def default value for missing key/invalid object 
       */
      Info ( const int    key , const double def ) ;
      /// copy constructor 
      Info ( const Info& right ) ;
      /// destructor 
      virtual ~Info(){};
      /// clone method (mandatory!)
      virtual  Info* clone() const { return new Info(*this); }
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      Info();
    };
    // ========================================================================
    /** @class SmartInfo
     *  Trivial function which:
     *    - checks the presence of informnation in LHCb::RecVertex::extraInfo
     *    - if the information present, it returns it 
     *    - for missing infomation, use function to evaluate it 
     *    - (optionally) fill th emissing field
     *  
     *  @see LHCb::RecVertex
     *  @see LoKi::Cuts::RVSINFO 
     *  @see LoKi::ExtraInfo::GetSmartInfo
     *  @see LoKi::ExtraInfo::info
     *  @see LoKi::ExtraInfo::hasInfo
     *  @see LoKi::ExtraInfo::setInfo
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class SmartInfo : public LoKi::ExtraInfo::GetSmartInfo<LHCb::RecVertex>
    {
    public:
      /** constructor from fuction, key and update-flag
       *  @param index the key in LHCb::Track::extraInfo table 
       *  @param fun functionto be evaluated for missing keys 
       *  @param update the flag to allow the insert of mnissing information
       */
      SmartInfo
      ( const int                                             index          , 
        const LoKi::BasicFunctors<LHCb::RecVertex>::Function& fun            , 
        const bool                                            update = false ) ;
      /// destructor 
      virtual ~SmartInfo(){};
      /// clone method (mandatory!)
      virtual  SmartInfo* clone() const { return new SmartInfo(*this); }
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      SmartInfo();
    };
    // ========================================================================
    /** @class VertexMinPt
     *  Evaluator of "minPT" for the vertex 
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::minPT
     *  @see LoKi::Cuts::RVMINPT
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-13
     */     
    class VertexMinPt : public LoKi::BasicFunctors<LHCb::RecVertex>::Function
    {
    public:
      /// mandatory: virtual destructor
      virtual ~VertexMinPt() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual VertexMinPt* clone() const { return new VertexMinPt(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "RVMINPT" ; }      
    } ;  
    // ========================================================================
    /** @class VertexMaxPT
     *  Evaluator of "maxPT" for the vertex 
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *  @see HltUtils::maxPT
     *  @see LoKi::Cuts::RVMINPT
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-13
     */     
    class VertexMaxPt : public LoKi::BasicFunctors<LHCb::RecVertex>::Function 
    {
    public:
      /// mandatory: virtual destructor
      virtual ~VertexMaxPt() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual VertexMaxPt* clone() const { return new VertexMaxPt(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "RVMAXPT" ; }      
    } ;  
    // ========================================================================
    /** @class DeltaZ 
     *  evaluator of delta-Z between twho vertices 
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see LoKi::Cuts::RVVDZ 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-13
     */     
    class DeltaZ 
      : public LoKi::BasicFunctors<LoKi::TrackTypes::VertexPair>::Function
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~DeltaZ() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  DeltaZ* clone() const { return new DeltaZ(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "RVVDZ" ; }
    } ;
    // ========================================================================
    /** @class FC 
     *
     *  The actual lines are copied from the package Hlt/HltBase 
     *   written by Jose Angel Hernando Morata
     *
     *  @see HltUtils::FC
     *  @see LoKi::Cuts::RVVFC 
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-08-13
     */     
    class FC 
      :  public LoKi::BasicFunctors<LoKi::TrackTypes::VertexPair>::Function
    {
    public :
      /// MANDATORY: virtual destructor 
      virtual ~FC() {}
      /// MANDATORY: clone method ("virtual constructor")  
      virtual  FC* clone() const { return new FC(*this) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument a ) const ;
      /// OPTIONAL: the nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const 
      { return s << "RVVFC" ; }
    } ; 
    // ========================================================================
  } // end of namespace 
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_RECVERTICES_H
// ============================================================================

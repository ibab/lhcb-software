// $Id$
// ============================================================================
#ifndef LOKI_TRACKIDS_H 
#define LOKI_TRACKIDS_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/OTChannelID.h"
#include "Kernel/STChannelID.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrackTypes.h"
// ============================================================================
/** @file LoKi/TrackIDs.h
 *  Collection of functors that deals with LHCbIDs for Tracks 
 *  (on request from Wouter Hulsbergen)
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-12-17
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Tracks 
  {
    // ========================================================================
    /** @class CountIDs
     *  Simple functor to count LHCbIDs that satisfy certain criteria 
     *  @author  Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-12-17
     */
    class GAUDI_API CountIDs 
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      // the actual type of pointer to member function
      typedef bool (LHCb::LHCbID::*PMF)() const ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from pointer to member function 
      CountIDs ( PMF fun ) ;
      /// constructor from pointer to member function name  
      CountIDs ( const std::string& name ) ;
      /// MANDATORY: virtual destructor
      virtual ~CountIDs() ;
      /// MANDATORY: clone method ("virtual consttructor")
      virtual  CountIDs* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// The default contructor is disabled 
      CountIDs() ;                        // The default contructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the pointer to member function 
      PMF  m_pmf ;                            // the pointer to member function
      /// the actual fuinctor name 
      std::string m_nick ;                          // the actual fuinctor name
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CountOTIDs
     *  Simple functor to count OTChannelID that satisfy certain criteria 
     *  @author  Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-12-17
     */
    class GAUDI_API CountOTIDs 
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// the actual type of pointer to member function
      typedef unsigned int (LHCb::OTChannelID::*PMF)() const ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from pointer to member function 
      CountOTIDs ( PMF fun , const unsigned int i ) ;
      /// constructor from pointer to member function & list  
      CountOTIDs ( PMF fun , const std::vector<unsigned int>& i ) ;
      /// constructor from pointer to member function & range 
      CountOTIDs ( const unsigned int imin , 
                   PMF                fun  , 
                   const unsigned int imax ) ;
      /// constructor from pointer to member function name  
      CountOTIDs ( const std::string& name , const unsigned int i ) ;
      /// constructor from pointer to member function name  & list 
      CountOTIDs ( const std::string& name , const std::vector<unsigned int>& i  ) ;
      /// constructor from pointer to member function name  & range
      CountOTIDs ( const unsigned int imin , 
                   const std::string& nick , 
                   const unsigned int imax ) ;
      /// MANDATORY: virtual destructor
      virtual ~CountOTIDs() ;
      /// MANDATORY: clone method ("virtual consttructor")
      virtual  CountOTIDs* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// The default contructor is disabled 
      CountOTIDs () ;                     // The default contructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// set the nick properly 
      void setNick ( PMF pmf ) ;                       // set the nick properly 
      /// set PMF properly 
      void setPmf  ( const std::string& nick ) ;       //      set PMF properly 
      // ======================================================================
    private:
      // ======================================================================
      /// the pointer to member function 
      PMF  m_pmf ;                            // the pointer to member function
      /// the list of values 
      std::vector<unsigned int>   m_uints ;  //       the list of values 
      /// the low edge 
      unsigned int m_imin ;                           // the low edge 
      /// the high edge 
      unsigned int m_imax ;                          // the high edge      
      /// the actual fuinctor name 
      std::string m_nick ;                          // the actual fuinctor name
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CountSTIDs
     *  Simple functor to count STChannelID that satisfy certain criteria 
     *  @author  Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-12-17
     */
    class GAUDI_API CountSTIDs 
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// the actual type of pointer to member function
      typedef unsigned int (LHCb::STChannelID::*PMF)() const ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from pointer to member function 
      CountSTIDs ( PMF fun , const unsigned int i ) ;
      /// constructor from pointer to member function & list  
      CountSTIDs ( PMF fun , const std::vector<unsigned int>& i ) ;
      /// constructor from pointer to member function & range 
      CountSTIDs ( const unsigned int imin , 
                   PMF                fun  , 
                   const unsigned int imax ) ;
      /// constructor from pointer to member function name  
      CountSTIDs ( const std::string& name , const unsigned int i ) ;
      /// constructor from pointer to member function name  & list 
      CountSTIDs ( const std::string& name , const std::vector<unsigned int>& i  ) ;
      /// constructor from pointer to member function name  & range
      CountSTIDs ( const unsigned int imin , 
                   const std::string& nick , 
                   const unsigned int imax ) ;
      /// MANDATORY: virtual destructor
      virtual ~CountSTIDs() ;
      /// MANDATORY: clone method ("virtual consttructor")
      virtual  CountSTIDs* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// The default contructor is disabled 
      CountSTIDs() ;                      // The default contructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// set the nick properly 
      void setNick ( PMF pmf ) ;                       // set the nick properly 
      /// set PMF properly 
      void setPmf  ( const std::string& nick ) ;       //      set PMF properly 
      // ======================================================================
    private:
      // ======================================================================
      /// the pointer to member function 
      PMF  m_pmf ;                            // the pointer to member function
      /// the list of values 
      std::vector<unsigned int>   m_uints ;  //       the list of values 
      /// the low edge 
      unsigned int m_imin ;                           // the low edge 
      /// the high edge 
      unsigned int m_imax ;                          // the high edge      
      /// the actual fuinctor name 
      std::string m_nick ;                          // the actual fuinctor name
      // ======================================================================
    };  
    // ========================================================================
    /** @class CountVeloIDs
     *  Simple functor to count VeloChannelID that satisfy certain criteria 
     *  @author  Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-12-17
     */
    class GAUDI_API CountVeloIDs 
      : public LoKi::BasicFunctors<const LHCb::Track*>::Function
    {
    public:
      // ======================================================================
      /// the actual type of pointer to member function
      typedef         bool (LHCb::VeloChannelID::*PMF1)() const ;
      typedef unsigned int (LHCb::VeloChannelID::*PMF2)() const ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from pointer to member function 
      CountVeloIDs ( PMF2 fun , const unsigned int i ) ;
      /// constructor from pointer to member function & list  
      CountVeloIDs ( PMF2 fun , const std::vector<unsigned int>& i ) ;
      /// constructor from pointer to member function & range 
      CountVeloIDs ( const unsigned int imin , 
                     PMF2               fun  , 
                     const unsigned int imax ) ;
      /// constructor from pointer to member function name  
      CountVeloIDs ( const std::string& name , const unsigned int i ) ;
      /// constructor from pointer to member function name  & list 
      CountVeloIDs ( const std::string& name , const std::vector<unsigned int>& i  ) ;
      /// constructor from pointer to member function name  & range
      CountVeloIDs ( const unsigned int imin , 
                     const std::string& nick , 
                     const unsigned int imax ) ;
      /// contructor from member-function 
      CountVeloIDs ( PMF1               pmf  ) ;
      /// contructor from member-function 
      CountVeloIDs ( const std::string& nick ) ;      
      /// MANDATORY: virtual destructor
      virtual ~CountVeloIDs() ;
      /// MANDATORY: clone method ("virtual consttructor")
      virtual  CountVeloIDs* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// The default contructor is disabled 
      CountVeloIDs() ;                    // The default contructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// set the nick properly 
      void setNick ( PMF2 pmf ) ;                      // set the nick properly 
      /// set PMF properly 
      void setPmf  ( const std::string& nick ) ;       //      set PMF properly 
      // ======================================================================
    private:
      // ======================================================================
      /// the pointer to member function 
      PMF1 m_pmf1 ;                           // the pointer to member function
      /// the pointer to member function 
      PMF2 m_pmf2 ;                           // the pointer to member function
      /// the list of values 
      std::vector<unsigned int>   m_uints ;          //      the list of values 
      /// the low edge 
      unsigned int m_imin ;                          //            the low edge 
      /// the high edge 
      unsigned int m_imax ;                          //           the high edge      
      /// the actual fuinctor name 
      std::string m_nick ;                           // the actual functor name
      // ======================================================================
    };  
    // ========================================================================
  } //                                            end of namespace LoKi::Tracks
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Cuts
  {
    // ========================================================================
    /** @typedef TrIDC
     *  simple functor to count LHCbIDs 
     *
     *  @code 
     *
     *   const LHCb::Track* track = ... ;
     *
     *    TrIDC fun1 = TrIDC ( &LHCb::LHCbID::isOT ) ; 
     *    TrIDC fun2 = TrIDC ( "isVelo"            ) ;
     *
     *    const bool good = fun1 ( track ) > 10 && fun2 ( track ) > 6 ; 
     *
     *  @endcode 
     *
     *  @see LoKi::Tracks::CountIDs 
     *  @see LHCb::LHCbIDs 
     *  @see LHCb::Track
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru  
     *  @date 2010-01-01
     */
    typedef LoKi::Tracks::CountIDs                                      TrIDC ;
    // ========================================================================
    /** @typedef TrOTIDC
     *  simple functor to count OTChannelID 
     *
     *  @code 
     *
     *   const LHCb::Track* track = ... ;
     *
     *    TrOTIDC fun1 = TrOTIDC (     &LHCb::OTChannelID::layer  , 1   ) ; 
     *
     *    TrOTIDC fun2 = TrOTIDC ( 3 , &LHCb::OTChannelID::module , 10  ) ;
     *
     *    const std::vector<unsigned int>& lst = ... ;
     *    TrOTIDC fun3 = TrOTIDC ( &LHCb::OTChannelID::station , lst  ) ; 
     *
     *    TrOTIDC fun4 = TrOTIDC (     "layer"  , 1   ) ; 
     *
     *    TrOTIDC fun5 = TrOTIDC ( 3 , "module" , 10  ) ;
     *
     *    const std::vector<unsigned int>& lst = ... ;
     *    TrOTIDC fun6 = TrOTIDC ( "station , lst  ) ; 
     *
     *    const bool good = 
     *               fun1 ( track ) > 1 && 
     *               fun2 ( track ) > 2 &&  
     *               fun3 ( track ) > 3 && 
     *               fun4 ( track ) > 4 &&  
     *               fun5 ( track ) > 5 && 
     *               fun6 ( track ) > 6   ; 
     *
     *  @endcode 
     *
     *  @see LoKi::Tracks::CountOTIDs 
     *  @see LHCb::OTChannelID 
     *  @see LHCb::Track
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru  
     *  @date 2010-01-01
     */
    typedef LoKi::Tracks::CountOTIDs                                  TrOTIDC ;
    // ========================================================================
    /** @typedef TrSTIDC
     *  simple functor to count STChannelID 
     *
     *  @code 
     *
     *   const LHCb::Track* track = ... ;
     *
     *    TrSTIDC fun1 = TrSTIDC (     &LHCb::STChannelID::sector  , 1   ) ; 
     *
     *    TrSTIDC fun2 = TrSTIDC ( 3 , &LHCb::STChannelID::layer , 10  ) ;
     *
     *    const std::vector<unsigned int>& lst = ... ;
     *    TrSTIDC fun3 = TrSTIDC ( &LHCb::STChannelID::strip , lst  ) ; 
     *
     *    TrSTIDC fun4 = TrSTIDC (     "sector"  , 1   ) ; 
     *
     *    TrSTIDC fun5 = TrSTIDC ( 3 , "layer" , 10  ) ;
     *
     *    const std::vector<unsigned int>& lst = ... ;
     *    TrSTIDC fun6 = TrSTIDC ( "strip" , lst  ) ; 
     *
     *    const bool good = 
     *               fun1 ( track ) > 1 && 
     *               fun2 ( track ) > 2 &&  
     *               fun3 ( track ) > 3 && 
     *               fun4 ( track ) > 4 &&  
     *               fun5 ( track ) > 5 && 
     *               fun6 ( track ) > 6   ; 
     *
     *  @endcode 
     *
     *  @see LoKi::Tracks::CountSTIDs 
     *  @see LHCb::STChannelID 
     *  @see LHCb::Track
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru  
     *  @date 2010-01-01
     */
    typedef LoKi::Tracks::CountSTIDs                                  TrSTIDC ;
    // ========================================================================
    /** @typedef TrVELOIDC
     *  simple functor to count VeloChannelID 
     *
     *  @code 
     *
     *   const LHCb::Track* track = ... ;
     *
     *    TrVELOIDC fun1 = TrVELOIDC (     &LHCb::VeloChannelID::sensor  , 1   ) ; 
     *
     *    TrVELOIDC fun2 = TrVELOIDC ( 3 , &LHCb::VeloChannelID::strip , 10  ) ;
     *
     *    const std::vector<unsigned int>& lst = ... ;
     *    TrVELOIDC fun3 = TrVELOIDC ( &LHCb::VeloChannelID::channelID , lst  ) ; 
     *
     *    TrVELOIDC fun4 = TrVELOIDC (     "sensor"  , 1   ) ; 
     *
     *    TrVELOIDC fun5 = TrVELOIDC ( 3 , "strip" , 10  ) ;
     *
     *    const std::vector<unsigned int>& lst = ... ;
     *    TrVELOIDC fun6 = TrVELOIDC ( "channelID" , lst  ) ; 
     *
     *    const bool good = 
     *               fun1 ( track ) > 1 && 
     *               fun2 ( track ) > 2 &&  
     *               fun3 ( track ) > 3 && 
     *               fun4 ( track ) > 4 &&  
     *               fun5 ( track ) > 5 && 
     *               fun6 ( track ) > 6   ; 
     *
     *  @endcode 
     *
     *  @see LoKi::Tracks::CountVeloIDs 
     *  @see LHCb::VeloChannelID 
     *  @see LHCb::Track
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru  
     *  @date 2010-01-01
     */
    typedef LoKi::Tracks::CountVeloIDs                              TrVELOIDC ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TRACKIDS_H
// ============================================================================

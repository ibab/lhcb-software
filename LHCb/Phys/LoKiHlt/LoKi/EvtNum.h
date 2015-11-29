// $Id: $
// ============================================================================
#ifndef LOKI_EVTNUM_H 
#define LOKI_EVTNUM_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
/** @file 
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2012-02-13
 * 
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */ 
 namespace LoKi 
{
  // ============================================================================
  namespace Numbers 
  {
    // ==========================================================================
    class EvtNum     ;
    class EvtNumList ;
    // ==========================================================================
    /** @class EvtNum LoKi/EvtNum.h
     *
     *  Simple wrapper over the numerical event type 
     *
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date   2012-02-13
     */
    class GAUDI_API EvtNum 
    {
    public:
      // =======================================================================
      /// the actual event type 
      typedef unsigned long long                                      evt_type ;
      // =======================================================================
    public:
      // =======================================================================
      EvtNum ( evt_type e  = 0 ) ;
      // =======================================================================
      bool operator<  ( const EvtNum& right ) const
      { return m_evtnum <  right.m_evtnum ; }
      bool operator<= ( const EvtNum& right ) const
      { return m_evtnum <= right.m_evtnum ; }
      bool operator>  ( const EvtNum& right ) const
      { return m_evtnum >  right.m_evtnum ; }
      bool operator>= ( const EvtNum& right ) const
      { return m_evtnum >= right.m_evtnum ; }
      bool operator== ( const EvtNum& right ) const
      { return m_evtnum == right.m_evtnum ; }
      bool operator!= ( const EvtNum& right ) const
      { return m_evtnum != right.m_evtnum ; }
      // =======================================================================
      EvtNum& operator+= ( const unsigned long e )
      { m_evtnum += e  ; return *this ; }
      // =======================================================================
      std::size_t   hash       () const ;
      // =======================================================================
      std::ostream& fillStream ( std::ostream& s ) const ;
      std::string   toString   ()                  const ;
      std::string   toCpp      ()                  const ;
      // =======================================================================
      evt_type event   () const { return   evtnum () ; }
      evt_type evt     () const { return   evtnum () ; }
      evt_type evtnum  () const { return m_evtnum    ; }
      // =======================================================================
      operator evt_type() const { return   evtnum () ; }
      // =======================================================================
    private :
      // =======================================================================
      // the actual storage 
      evt_type m_evtnum ;
      // =======================================================================
    } ;
    // =========================================================================
    /** @class EvtNumList 
     *  simple wrapper for event list 
     *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
     *  @date 2012-02-13
     */
    class GAUDI_API EvtNumList 
    {
    public:
      // =======================================================================
      /// the actual type for event list 
      typedef std::vector<EvtNum>                                     evt_list ;
      // =======================================================================
    private:
      // =======================================================================
      typedef evt_list::const_iterator                                iterator ;
      // =======================================================================
    public:
      // =======================================================================
      EvtNumList ( const evt_list& evts = evt_list()  ) ;
      EvtNumList ( const EvtNum&   evt                ) ;
      // =======================================================================
      std::size_t    size  () const { return m_list.size  () ; }
      bool           empty () const { return m_list.empty () ; }
      // =======================================================================
      iterator       begin () const { return m_list.begin () ; }
      iterator       end   () const { return m_list.end   () ; }
      const EvtNum&  at( std::size_t index ) const { return m_list.at ( index ) ; }
      // =======================================================================
      std::size_t    hash       () const ;
      std::ostream&  fillStream ( std::ostream& s ) const ;
      std::string    toString   ()                  const ;
      std::string    toCpp      ()                  const ;
      // =======================================================================
      bool contains ( const EvtNum& e ) const ;
      // =======================================================================
    public:
      // =======================================================================
      // swap it! 
      void swap     ( EvtNumList&   o ) { std::swap ( m_list , o.m_list ) ; }
      // =======================================================================
    public:
      // =======================================================================
      EvtNumList operator+( const EvtNum&     right ) const ;
      EvtNumList operator+( const EvtNumList& right ) const ;
      // =======================================================================
    private:
      // =======================================================================
      /// the actual event list 
      std::vector<EvtNum> m_list ;                      // the actual event list 
      // =======================================================================
    } ;
    // =========================================================================
    /** @class RunEvt 
     *  simple wrapper for run-event pair
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-02-13
     */
    class GAUDI_API RunEvt 
    {
    public:
      // ======================================================================
      typedef unsigned int run_type ;
      typedef EvtNum       evt_type ;
      // ======================================================================
      typedef run_type   first_type ;
      typedef evt_type  second_type ;
      // ======================================================================
    public:
      // ======================================================================
      RunEvt ( const run_type r = 0 , const evt_type         e = evt_type() ) ;
      RunEvt ( const run_type r     , const EvtNum::evt_type e              ) ;
      // ======================================================================      
    public:
      // ======================================================================      
      bool operator<  ( const RunEvt& right ) const 
      { return m_pair < right.m_pair ; }
      bool operator>  ( const RunEvt& right ) const 
      { return right  < *this ; }
      bool operator== ( const RunEvt& right ) const
      { return m_pair == right.m_pair ; }
      bool operator!= ( const RunEvt& right ) const 
      { return !( *this == right ) ; }
      bool operator<= ( const RunEvt& right ) const 
      { return !( *this >  right ) ; }
      bool operator>= ( const RunEvt& right ) const 
      { return !( *this <  right ) ; }
      // ======================================================================
    public:
      // ======================================================================
      std::size_t   hash       () const ;
      std::ostream& fillStream ( std::ostream& s ) const ;
      std::string   toString   ()                  const ;
      std::string   toCpp      ()                  const ;
      // =======================================================================
    public:
      // =======================================================================
      run_type run   () const { return m_pair.first  ; }
      evt_type evt   () const { return m_pair.second ; }
      evt_type event () const { return evt () ; }
      // =======================================================================
    public:
      // ======================================================================
      std::pair<run_type,evt_type> m_pair ;
      // ======================================================================
    } ;
    // =========================================================================
    /** @class RunEvtList 
     *  simple wrapper for run-event list
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-02-13
     */
    class GAUDI_API RunEvtList 
    {
    public:
      // =======================================================================
      /// the actual type of the list 
      typedef  std::vector<RunEvt>                                runevt_list ;
      // =======================================================================
    private:
      // =======================================================================
      typedef runevt_list::const_iterator                            iterator ;
      // =======================================================================
    public:
      // =======================================================================
      RunEvtList ( const runevt_list& lst = runevt_list() ) ;
      RunEvtList ( const RunEvt&      evt                 ) ;
      // =======================================================================
      std::size_t size  () const { return m_list.size  () ; }
      bool        empty () const { return m_list.empty () ; }
      // =======================================================================
      iterator    begin () const { return m_list.begin () ; }
      iterator    end   () const { return m_list.end   () ; }
      const RunEvt& at ( std::size_t index ) const { return m_list.at ( index ) ; }
      // =======================================================================
      std::size_t   hash       () const ;
      std::ostream& fillStream ( std::ostream& s ) const ;
      std::string   toString   ()                  const ;
      std::string   toCpp      ()                  const ;
      // =======================================================================
      bool contains ( const RunEvt& e ) const ;
      // =======================================================================
    public:
      // =======================================================================
      // swap it! 
      void swap     ( RunEvtList&   o ) { std::swap ( m_list , o.m_list ) ; }
      // =======================================================================
    public:
      // =======================================================================
      RunEvtList operator+( const RunEvt&     right ) const ;
      RunEvtList operator+( const RunEvtList& right ) const ;
      // =======================================================================
    private:
      // =======================================================================
      /// the actual run/event-list 
      runevt_list m_list ; // the actual run/event-list 
      // =======================================================================
    };
    // =========================================================================
    // useful functions 
    // =========================================================================
    /** hash-function: heeded for boost::hash
     *  @return the actual hash value 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-02-13
     */
    inline std::size_t hash_value ( const LoKi::Numbers::EvtNum& evt )
    { return evt.hash () ; }
    // =========================================================================
    /** hash-function: heeded for boost::hash
     *  @return the actual hash value 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-02-13
     */
    inline std::size_t hash_value ( const LoKi::Numbers::EvtNumList& evt )
    { return evt.hash () ; }
    // ========================================================================
    /** hash-function: heeded for boost::hash
     *  @return the actual hash value 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-02-13
     */
    inline std::size_t hash_value ( const LoKi::Numbers::RunEvt& evt )
    { return evt.hash () ; }
    // =========================================================================
    /** hash-function: heeded for boost::hash
     *  @return the actual hash value 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-02-13
     */
    inline std::size_t hash_value ( const LoKi::Numbers::RunEvtList& evt )
    { return evt.hash () ; }
    // =========================================================================
    inline std::ostream& 
    operator<<( std::ostream& s , const LoKi::Numbers::EvtNum&     e ) 
    { return e.fillStream ( s ) ; }
    // =========================================================================
    inline std::ostream& 
    operator<<( std::ostream& s , const LoKi::Numbers::EvtNumList& e ) 
    { return e.fillStream ( s ) ; }
    // =========================================================================
    inline std::ostream& 
    operator<<( std::ostream& s , const LoKi::Numbers::RunEvt&     e ) 
    { return e.fillStream ( s ) ; }
    // =========================================================================
    inline std::ostream& 
    operator<<( std::ostream& s , const LoKi::Numbers::RunEvtList&     e ) 
    { return e.fillStream ( s ) ; }
    // =========================================================================
    inline 
    EvtNumList operator+( const EvtNum&     e1 , const EvtNumList&  e2 )
    { return e2 + e1 ; }
    inline 
    RunEvtList operator+( const RunEvt&     e1 , const RunEvtList&  e2 )
    { return e2 + e1 ; }
    // =========================================================================
    inline 
    RunEvtList operator+( const RunEvt&     e1 , const RunEvt&      e2 )
    { return RunEvtList ( e1 ) + e2  ; }
    // =========================================================================
    // for python 
    // =========================================================================
    GAUDI_API 
    EvtNumList add1 ( const EvtNum&     e1 , const EvtNumList& e2 ) ;
    GAUDI_API 
    EvtNumList add1 ( const EvtNum&     e1 , const EvtNum&     e2 ) ;
    GAUDI_API 
    EvtNumList add1 ( const EvtNumList& e1 , const EvtNum&     e2 ) ;
    GAUDI_API 
    EvtNumList add1 ( const EvtNumList& e1 , const EvtNumList& e2 ) ;
    GAUDI_API 
    EvtNumList add1 ( const EvtNumList& e1 , EvtNum::evt_type  e2 ) ;
    GAUDI_API 
    RunEvtList add2 ( const RunEvt&     e1 , const RunEvt&     e2 ) ;
    GAUDI_API 
    RunEvtList add2 ( const RunEvt&     e1 , const RunEvtList& e2 ) ;
    GAUDI_API 
    RunEvtList add2 ( const RunEvtList& e1 , const RunEvt&     e2 ) ;
    GAUDI_API 
    RunEvtList add2 ( const RunEvtList& e1 , const RunEvtList& e2 ) ;
    // =========================================================================
    // swap them! 
    // =========================================================================
    inline void swap ( EvtNumList& l1 , EvtNumList& l2 ) { l1.swap ( l2 ) ; }
    inline void swap ( RunEvtList& l1 , RunEvtList& l2 ) { l1.swap ( l2 ) ; }
    // =========================================================================
  } //                                        the end of namespace LoKi::Numbers 
  // ===========================================================================
 } //                                                  the end of namespace LoKi 
// =============================================================================
namespace Gaudi
{
  // ===========================================================================
  namespace Utils 
  {
    // =========================================================================
    /// the streamer 
    GAUDI_API 
    std::ostream& 
    toStream ( const LoKi::Numbers::EvtNum& evt , 
               std::ostream&                s   ) ;
    // =========================================================================
    /// the streamer 
    GAUDI_API 
    std::string 
    toString ( const LoKi::Numbers::EvtNum& evt ) ;
    // =========================================================================
    /// the streamer 
    GAUDI_API 
    std::ostream& 
    toStream ( const LoKi::Numbers::EvtNumList& evt , 
               std::ostream&                s   ) ;
    // =========================================================================
    /// the streamer 
    GAUDI_API 
    std::string 
    toString ( const LoKi::Numbers::EvtNumList& evt ) ;
    // =========================================================================
    /// the streamer 
    GAUDI_API 
    std::ostream& 
    toStream ( const LoKi::Numbers::RunEvt& evt , 
               std::ostream&                s   ) ;
    // =========================================================================
    /// the streamer 
    GAUDI_API 
    std::string 
    toString ( const LoKi::Numbers::RunEvt& evt ) ;
    // =========================================================================
    inline std::string toCpp ( const LoKi::Numbers::EvtNum&     e )
    { return e.toCpp() ; }
    // =========================================================================
    inline std::string toCpp ( const LoKi::Numbers::EvtNumList& e )
    { return e.toCpp() ; }
    // =========================================================================
    inline std::string toCpp ( const LoKi::Numbers::RunEvt&     e )
    { return e.toCpp() ; }
    // =========================================================================
    inline std::string toCpp ( const LoKi::Numbers::RunEvtList& e )
    { return e.toCpp() ; }
    // =========================================================================
  } //                                                   end of namespace Utils 
  // ===========================================================================
  namespace Parsers 
  {
    // =========================================================================
    /// parser 
    GAUDI_API 
    StatusCode 
    parse( LoKi::Numbers::EvtNum& result, const std::string& input);
    // =========================================================================
    /// parser 
    GAUDI_API 
    StatusCode 
    parse( LoKi::Numbers::EvtNumList& result, const std::string& input);
    // =========================================================================
    /// parser 
    GAUDI_API 
    StatusCode 
    parse( LoKi::Numbers::RunEvt& result, const std::string& input);
    // =========================================================================
    /// parser 
    GAUDI_API 
    StatusCode 
    parse( LoKi::Numbers::RunEvtList& result, const std::string& input);
    // =========================================================================
  } //                                           end of namespace Gaudi::Parsers 
  // ===========================================================================
} //                                                      end of namespace Gaudi
// =============================================================================
//                                                                       The END 
// =============================================================================
#endif // LOKI_EVTNUM_H
// =============================================================================

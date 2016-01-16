// $Id$ 
// ============================================================================
#ifndef LOKI_MONITOR_H 
#define LOKI_MONITOR_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/HistoDef.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiHistoID.h"
// ============================================================================
class IHistogramSvc  ;
class IAlgContextSvc ;
class IStatSvc       ;
class ICounterSvc    ;
class GaudiTool      ;
class GaudiAlgorithm ;
class StatEntity     ;
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  class Histo      ;
  class CounterDef ;
  // ==========================================================================
  namespace Monitoring
  {
    // ========================================================================
    /** helper enumerator to indicate the mode for creation of counters:
     *
     *   - ContextSvc : the counter is retrieved from the corresponding 
     *                  GauidiAlgorithm using Context Service      
     *   - StatSvc    : the counter is retrieved from Stat Service 
     *   - CounterSvc : the counter is retrieved from Counter Service 
     *
     *  @see StatEntity
     *  @see Stat
     *  @see IAlgContextSvc 
     *  @see GaudiAlgorithm
     *  @see GaudiCommon
     *  @see IStatSvc 
     *  @see ICounterSvc 
     */
    enum Flag 
      {
        ContextSvc = 0 , //  local counter through IAlgContext -> GaudiAlgorithm  ,
        StatSvc        , // global counter through IStatSvc  ,
        CounterSvc       // global counter through ICounterSvc  
      };
    // ========================================================================
    /** get the (global) counter by name using IStatSvc 
     *  @param IStatSvc 
     *  @param ssvc service of statistics 
     *  @param name the counter name 
     *  @return the counter 
     */
    GAUDI_API 
    StatEntity* getCounter 
    ( IStatSvc*          csvc , 
      const std::string& name ) ;
    /** get the (local) counter by name using GaudiAlgorithm
     *  @param alg the algorithm 
     *  @param name the counter name 
     *  @return the counter 
     */
    GAUDI_API 
    StatEntity* getCounter 
    ( GaudiAlgorithm*    alg  , 
      const std::string& name ) ; 
    /** get the (local) counter by name using GaudiTool
     *  @param tool the tool 
     *  @param name the counter name 
     *  @return the counter 
     */
    GAUDI_API 
    StatEntity* getCounter 
    ( GaudiTool*         tool , 
      const std::string& name ) ; 
    /** get the counter by name using ICounterSvc 
     *  @see ICounterSvc 
     *  @param csvc the counter service 
     *  @param group the counter group
     *  @param name the counter name 
     *  @return the counter 
     */
    GAUDI_API 
    StatEntity* getCounter 
    ( ICounterSvc*       csvc     ,
      const std::string& group    ,
      const std::string& name     ) ;
    /** get the counter by name using IAlgContextSvc 
     *  @param name the counter name 
     *  @param csvc context service 
     *  @return the counter 
     */
    GAUDI_API 
    StatEntity* getCounter 
    ( const IAlgContextSvc* csvc , 
      const std::string&    name ) ;
    /** get the counter by name using IStatSvc/ICounter or IAlgContextSvc
     *  @param ICounterSvc      
     *  @param IStatSvc      
     *  @param IAlgContextSvc 
     *  @param flag  local/global flag 
     *  @param group the counter grop
     *  @param name  the counter name
     *  @return the counter 
     */
    GAUDI_API 
    StatEntity* getCounter 
    ( const Flag         flag  , 
      const std::string& group , 
      const std::string& name  ) ;
    /** get the counter by name using IStatSvc/ICounter or IAlgContextSvc
     *  @param ICounterSvc      
     *  @param IStatSvc      
     *  @param IAlgContextSvc 
     *  @param flag  local/global flag 
     *  @param name  the counter name
     *  @return the counter 
     */
    GAUDI_API 
    StatEntity* getCounter 
    ( const Flag         flag  , 
      const std::string& name  ) ;
    /** get the counter using helper class 
     *  @return the counter 
     */
    GAUDI_API 
    StatEntity* getCounter 
    ( const LoKi::CounterDef& cnt ) ;
    // ========================================================================
  } //                                        end of namespace LoKi::Monitoring
  // ==========================================================================
  /** @class Histo LoKi/Monitor.h
   *  Special holder to postpone  the histogram booking 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2015-01-14
   */
  class GAUDI_API Histo
  {
  public: 
    // ========================================================================
    /// constructors for Histogram service 
    Histo ( const std::string&       dir     , 
            const std::string&       id      , 
            const Gaudi::Histo1DDef& hdef    , 
            IHistogramSvc*           svc = 0 ) ;
    Histo ( const std::string&       dir     , 
            const int                id      , 
            const Gaudi::Histo1DDef& hdef    , 
            IHistogramSvc*           svc = 0 ) ;
    Histo ( const std::string&       dir     , 
            const GaudiAlg::ID&      id      , 
            const Gaudi::Histo1DDef& hdef    , 
            IHistogramSvc*           svc = 0 ) ;
    Histo ( const std::string&       path    , 
            const Gaudi::Histo1DDef& hdef    , 
            IHistogramSvc*           svc = 0 ) ;
    /// constructors for Context service  
    Histo ( const Gaudi::Histo1DDef& hdef    , 
            const std::string&       id      ,
            IAlgContextSvc*          svc = 0 ) ;
    Histo ( const Gaudi::Histo1DDef& hdef    , 
            const int                id      ,
            IAlgContextSvc*          svc = 0 ) ;
    Histo ( const Gaudi::Histo1DDef& hdef    , 
            const GaudiAlg::ID&      id      , 
            IAlgContextSvc*          svc = 0 ) ;
    /// default constructor 
    Histo () ;
    // ========================================================================
  public:
    // ========================================================================
    const std::string&       path  () const { return m_path  ; }
    const GaudiAlg::ID&      id    () const { return m_id    ; }
    const Gaudi::Histo1DDef& hdef  () const { return m_hdef  ; }
    bool                     hcase () const { return m_case  ; }
    bool                     valid () const { return m_valid ; }
    // ========================================================================
    IHistogramSvc*  histoSvc   () const { return m_hsvc ; }
    IAlgContextSvc* contextSvc () const { return m_cntx ; }
    // ========================================================================
  private:
    // 
    std::string       m_path  ;
    GaudiAlg::ID      m_id    ;
    Gaudi::Histo1DDef m_hdef  ;
    bool              m_case  ;
    bool              m_valid ;
    IHistogramSvc*    m_hsvc  ;
    IAlgContextSvc*   m_cntx  ;
    // ========================================================================
  };
  // ==========================================================================
  /** @class CounterDef 
   *  helper class to keep defintino of counter 
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2015-01-14
   */
  class GAUDI_API CounterDef 
  {
  public:
    // ========================================================================
    CounterDef ( const std::string&           group ,
                 const std::string&           name  , 
                 const LoKi::Monitoring::Flag flag  ) ;
    CounterDef ( const std::string&           name = "" , 
                 const LoKi::Monitoring::Flag f    = LoKi::Monitoring::ContextSvc ) ;
    // ========================================================================
  public:
    // ========================================================================
    const std::string&     group () const { return m_group ; }
    const std::string&     name  () const { return m_name  ; }
    LoKi::Monitoring::Flag flag  () const { return m_flag  ; }
    // ========================================================================
    bool valid() const { return !m_name.empty() ; }
    // ========================================================================
  private:
    // ========================================================================
    std::string            m_group ;
    std::string            m_name  ;
    LoKi::Monitoring::Flag m_flag  ;
    // ========================================================================    
  };
  // ==========================================================================
} // end of namespace LoKi 
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    GAUDI_API std::string toCpp ( const LoKi::Histo&       o ) ;
    GAUDI_API std::string toCpp ( const LoKi::CounterDef&  o ) ;
    GAUDI_API std::string toCpp ( const GaudiAlg::ID&      o ) ;
    GAUDI_API std::string toCpp ( const Gaudi::Histo1DDef& o ) ;
    // ========================================================================
  }
  // ==========================================================================
}
// ============================================================================
#endif // LOKI_MONITOR_H
// ============================================================================
// The END 
// ============================================================================

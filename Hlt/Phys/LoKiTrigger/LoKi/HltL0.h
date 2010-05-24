// $Id: HltL0.h,v 1.4 2010-05-24 09:11:20 graven Exp $
// ============================================================================
#ifndef LOKITRIGGER_HLTL0_H 
#define LOKITRIGGER_HLTL0_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <iosfwd>
#include <map>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/VectorMap.h"
// ============================================================================
// L0Event 
// ============================================================================
#include "Event/L0DUBase.h"
#include "Event/L0DUReport.h"
#include "Event/L0CaloCandidate.h"
#include "Event/L0MuonCandidate.h"
// ============================================================================
namespace Hlt 
{
  // ==========================================================================
  /** @namespace Hlt:L0Utils  HltL0.h
   *  Collection of simple functions to deal with L0 candidates
   *
   *  The actual lines are stollen from 
   *    Gerhard "The Great" Raven & Jose Angel Hernando Morata
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2009-03-23
   */
  namespace L0Utils 
  {
    // ========================================================================
    /** the error codes 
     *  @see Hlt::L0Utils::getElementaryL0Data 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */     
    enum Codes 
      {
        L0Report_Invalid = 850 ,
        L0Config_Invalid       ,
        L0Channel_Unknown      ,
        L0Channel_Invalid      ,
        L0Condition_Invalid    ,
        L0Data_Invalid         ,
        L0Channel_InvalidMap     
      };
    // ========================================================================
    /** @class L0CaloCut 
     *  simple "cut" for L0Calocandidates 
     *  The actual lines(and the concept) are stollen 
     *                      from Gerhard "The Great "Raven 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */
    class L0CaloCut
    {
    public:
      // ======================================================================
      /// constructor 
      L0CaloCut
      ( const L0DUBase::CaloType::Type type ) 
        : m_type         ( type  )
        , m_hasThreshold ( false )
        , m_threshold    ( -1    ) 
      {}
      /// constructor 
      L0CaloCut 
      ( const L0DUBase::CaloType::Type type      , 
        const int                      threshold ) 
        : m_type         ( type      )
        , m_hasThreshold ( true      )
        , m_threshold    ( threshold ) 
      {}
      // the only essential method 
      inline bool operator()( const LHCb::L0CaloCandidate* calo) const 
      {
        return 
          ( 0      != calo         ) && 
          ( m_type == calo->type() ) && 
          ( !m_hasThreshold || m_threshold <= calo->etCode() ) ;
      }
      // ======================================================================
    public:
      // ======================================================================
      // standard printout 
      std::ostream& fillStream ( std::ostream& s ) const ;
      /// conversion to string 
      std::string   toString   ()                  const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the candidate type 
      L0DUBase::CaloType::Type m_type   ;                 // the candidate type 
      /// the flag for threshold 
      bool                     m_hasThreshold ;       // the flag for threshold 
      /// the threshold
      int                      m_threshold    ;                // the threshold
      // ======================================================================
    };
    // ========================================================================
    inline std::ostream& operator<< (std::ostream& s , const L0CaloCut& cut )
    { return cut.fillStream ( s ) ; }
    // ========================================================================
    /// the actual tyle of cuts list:
    typedef std::vector<L0CaloCut>                                  L0CaloCuts;
    // ========================================================================
    /** @class L0MuonCut 
     *  simple "cut" for L0MuonCandidates 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */
    class L0MuonCut 
    {
    public:
      // ======================================================================
      /// constructor 
      L0MuonCut
      ( const std::string& type  ) 
        : m_type         ( type  )
        , m_hasThreshold ( false )
        , m_threshold    ( -1    ) 
      {}
      /// constructor 
      L0MuonCut 
      ( const std::string& type      , 
        const int          threshold ) 
        : m_type         ( type      )
        , m_hasThreshold ( true      )
        , m_threshold    ( threshold ) 
      {}
      /// default constructor 
      L0MuonCut () 
        : m_type         (       )
        , m_hasThreshold ( false )
        , m_threshold    ( -1    ) 
      {}
      // the only essential method 
      inline bool operator()( const LHCb::L0MuonCandidate* muon ) const 
      {
        return 
          ( 0      != muon         ) && 
          ( !m_hasThreshold || m_threshold < (muon->encodedPt()&0x7F) ) ;
      }
      // ======================================================================
    public:
      // ======================================================================
      // standard printout 
      std::ostream& fillStream ( std::ostream& s ) const ;
      /// conversion to string 
      std::string   toString   ()                  const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the candidate type 
      std::string              m_type         ;           // the candidate type 
      /// the flag for threshold 
      bool                     m_hasThreshold ;       // the flag for threshold 
      /// the threshold
      int                      m_threshold    ;                // the threshold
      // ======================================================================
    };
    // ========================================================================
    inline std::ostream& operator<< (std::ostream& s , const L0MuonCut& cut )
    { return cut.fillStream ( s ) ; }
    // ========================================================================
    /// the actual tyle of cuts list:
    typedef std::vector<L0MuonCut>                                  L0MuonCuts;
    // ========================================================================
    /// the actual type for vector of names 
    typedef std::vector<std::string>                                    Names ;
    /// the actual type for vector of elementary data 
    typedef std::map<std::string,const LHCb::L0DUElementaryCondition*>   Data ;
    /// the map : channel name -> channel type  
    typedef GaudiUtils::VectorMap<std::string,L0DUBase::CaloType::Type> CaloTypes ;
    // ========================================================================
    /** get the elementary data for the given channel by names 
     *  @param config  (INPUT)  L0DUConfig object
     *  @param channel (INPUT)  the channel under the interest 
     *  @param names   (INPUT)  the list of names 
     *  @param data    (OUTPUT) the output map of data 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */
    StatusCode getElementaryL0Data 
    ( const LHCb::L0DUConfig*  config  , 
      const std::string&       channel ,
      const Names&             names   , 
      Data&                    data    ) ;
    // =====================================================================
    /** get the elementary data for the given channel by names 
     *  @param channel (INPUT)  the channel under the interest
     *  @param names   (INPUT)  the list of names 
     *  @param data    (OUTPUT) the output map of data 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */
    StatusCode getElementaryL0Data 
    ( const LHCb::L0DUChannel* channel ,
      const Names&             names   , 
      Data&                    data    ) ;
    // ========================================================================
    /** get L0Muon cuts form the L0 config 
     *  @param config  (INPUT)  L0 configuration 
     *  @param channel (INPUT)  the channel under the interest 
     *  @param names   (INPUT)  the list of names 
     *  @param cuts    (OUTPUT) the output list of cuts 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */        
    StatusCode getL0Cuts 
    ( const LHCb::L0DUConfig* config  ,
      const std::string&      channel ,
      const Names&            names   , 
      L0MuonCuts&             cuts    ) ;
    // ======================================================================
    /** get L0Muon cuts from the L0 channel
     *  @param channel (INPUT)  the channel under the interest 
     *  @param names   (INPUT)  the list of names 
     *  @param cuts    (OUTPUT) the output list of cuts 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */        
    StatusCode getL0Cuts 
    ( const LHCb::L0DUChannel* channel ,
      const Names&             names   , 
      L0MuonCuts&              cuts    ) ;
    // ========================================================================
    /** get the cuts for L0 Calo channels 
     *  @param config the L0 configuration object 
     *  @param channel the channel name 
     *  @param types the map  { name : type } 
     *  @param cuts  (OUTPUT) the list of LCalo cuts 
     *  @see Hlt::L0Utils::L0CaloCut
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */
    StatusCode getL0Cuts 
    ( const LHCb::L0DUConfig*  config  , 
      const std::string&       channel ,
      const CaloTypes&         types   , 
      L0CaloCuts&              cuts    ) ;
    // ========================================================================
    /** get the cuts for L0 Calo channels 
     *  @param config the L0 configuration object 
     *  @param channel the channel name 
     *  @param types the map  { name : type } 
     *  @param cuts  (OUTPUT) the list of LCalo cuts 
     *  @see Hlt::L0Utils::L0CaloCut
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */
    StatusCode getL0Cuts 
    ( const LHCb::L0DUChannel* channel ,
      const CaloTypes&         types   ,
      L0CaloCuts&              cuts    ) ;
    // ========================================================================
    /** get the cuts for L0 Calo channels 
     *  @param types the map  { name : type } 
     *  @param cuts  (OUTPUT) the list of LCalo cuts 
     *  @see Hlt::L0Utils::L0CaloCut
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */
    StatusCode getL0Cuts 
    ( const std::string&       channel ,
      const CaloTypes&         types   , 
      L0CaloCuts&              cuts    ) ;
    // ========================================================================
    /** get the elementary data for the given channel by names 
     *  @param report  (INPUT)  L0DUReport object
     *  @param channel (INPUT)  the channel under the interest 
     *  @param names   (INPUT)  the list of names 
     *  @param data    (OUTPUT) the output map of data 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */
    inline StatusCode getElementaryL0Data 
    ( const LHCb::L0DUReport*  report  , 
      const std::string&       channel , 
      const Names&             names   , 
      Data&                    data    ) 
    {
      data.clear() ;
      if ( 0 == report ) { return L0Report_Invalid ; }                // RETURN 
      return getElementaryL0Data 
        ( report->configuration () ,  channel , names , data ) ;      // RETURN
    }
    // ========================================================================
    /** get L0Muon cuts form the L0-trigge report 
     *  @param report  (INPUT)  L0DUReport object
     *  @param channel (INPUT)  the channel under the interest 
     *  @param names   (INPUT)  the list of names 
     *  @param cuts    (OUTPUT) the output list of cuts 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */        
    inline StatusCode getL0Cuts 
    ( const LHCb::L0DUReport*  report  , 
      const std::string&       channel ,
      const Names&             names   , 
      L0MuonCuts&              cuts    ) 
    {
      cuts.clear() ;
      if ( 0 == report ) { return L0Report_Invalid ; }                // RETURN 
      return getL0Cuts
        ( report->configuration () ,  channel , names , cuts ) ;      // RETUEN
    }
    // ========================================================================
    /** get the cuts for L0 Calo channels 
     *  @param report  the L0 report 
     *  @param channel the channel name 
     *  @param types the map  { name : type } 
     *  @param cuts  (OUTPUT) the list of LCalo cuts 
     *  @see Hlt::L0Utils::L0CaloCut
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */
    inline StatusCode getL0Cuts 
    ( const LHCb::L0DUReport*  report  , 
      const std::string&       channel ,
      const CaloTypes&         types   , 
      L0CaloCuts&              cuts    ) 
    {
      cuts.clear() ;
      if ( 0 == report ) { return L0Report_Invalid ; }                // RETURN 
      return getL0Cuts
        ( report->configuration () ,  channel , types , cuts ) ;      // RETURN
    }
    // ========================================================================
  } // end of namespace Hlt::L0Utils 
  // ==========================================================================
} // end of namespace Hlt 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKITRIGGER_HLTL0_H
// ============================================================================

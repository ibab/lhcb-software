// $Id$
// ============================================================================
#ifndef LOKITRIGGER_HLTL0_H 
#define LOKITRIGGER_HLTL0_H 1
// ============================================================================
// $URL$
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
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
// ============================================================================
/** @file  LoKi/HltL0.h
 *  Implementation for L0-related functions 
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *
 *  $Revision$
 *  Last Modification $Date$ by $Author$ 
 */
namespace LoKi
{
  // ==========================================================================
  namespace L0
  {
    // ========================================================================
    /** the error codes 
     *  @see Hlt::L0::getElementaryL0Data 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */     
    enum Codes {
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
      : public LoKi::BasicFunctors<const LHCb::L0CaloCandidate*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor 
      L0CaloCut ( const L0DUBase::CaloType::Type type ) ;
      /// constructor 
      L0CaloCut 
      ( const L0DUBase::CaloType::Type type      , 
        const int                      threshold ) ;
      /// constructor 
      L0CaloCut () ;
      /// MANDATORY: virtual desctructor 
      virtual ~L0CaloCut() ;
      /// MANDATORY: clone method ("virtual construtor")
      virtual  L0CaloCut* clone() const ;
      /// the only essential method 
      result_type  operator()( argument calo) const ;
      /// OPTIONAL:  standard printout 
      std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      L0CaloCut& operator=( const L0CaloCut& ) ;
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
    /// the actual tyle of cuts list:
    typedef std::vector<L0CaloCut>                                  L0CaloCuts;
    // ========================================================================
    /** @class L0MuonCut 
     *  simple "cut" for L0MuonCandidates 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-03-23
     */
    class L0MuonCut 
      : public LoKi::BasicFunctors<const LHCb::L0MuonCandidate*>::Predicate
    {
    public:
      // ======================================================================
      /// constructor 
      L0MuonCut ( std::string type = "" ) ;
      /// constructor 
      L0MuonCut ( std::string type      , 
                  int         threshold ) ;
      /// MANDATORY: virtual desctructor 
      virtual ~L0MuonCut() ;
      /// MANDATORY: clone method ("virtual construtor")
      virtual  L0MuonCut* clone() const ;
      /// the only essential method 
      virtual result_type operator()( argument muon ) const ;
      // standard printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      L0MuonCut& operator=( const L0MuonCut& ) ;
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
    /// the actual type of cuts list:
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
    /** get L0Muon cuts from the L0-trigger report 
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
  } //                                           end of namespace Hlt::L0Utils 
  // ==========================================================================
} //                                                      end of namespace Hlt 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKITRIGGER_HLTL0_H
// ============================================================================

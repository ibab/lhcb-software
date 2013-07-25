// $Id: HltDiffHltDecReports.cpp,v 1.1.1.1 2009-06-24 15:38:52 tskwarni Exp $
// Include files 
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/HltDecReports.h"
#include <iterator>

class HltDiffHltDecReports : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltDiffHltDecReports( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltDiffHltDecReports() {}; ///< Destructor
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  std::string m_lhs;  
  std::string m_rhs;  
};

//-----------------------------------------------------------------------------
// Implementation file for class : HltDiffHltDecReports
//
// 2009-05-26 : Gerhard Raven
//-----------------------------------------------------------------------------

// explicit instantiation of class, and the corresponding
// declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltDiffHltDecReports )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDiffHltDecReports::HltDiffHltDecReports( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
    declareProperty("HltDecReportsLHS",m_lhs);
    declareProperty("HltDecReportsRHS",m_rhs);
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode 
HltDiffHltDecReports::execute() {
// grab the two reports...
    LHCb::HltDecReports *lhs = get<LHCb::HltDecReports>( m_lhs );
    LHCb::HltDecReports *rhs = get<LHCb::HltDecReports>( m_rhs );

    if (lhs->configuredTCK() != rhs->configuredTCK() ) {
        always() << " configuredTCK: " 
                 << m_lhs << " : " << lhs->configuredTCK() 
                 << m_rhs << " : " << rhs->configuredTCK() 
                 << endmsg;
    }

    LHCb::HltDecReports::Container::const_iterator l = lhs->begin(), r = rhs->begin();
    while ( l != lhs->end() || r != rhs->end()) {
        bool rend = ( r == rhs->end() );
        bool lend = ( l == lhs->end() );

        if ( !lend && ( rend || l->first < r->first )) { 
            always() << " only in " << m_lhs << " : " << l->first << " : " << l->second << endmsg;
            ++l;
        } else if ( !rend && ( lend || r->first < l->first ) ) { 
            always() << " only in " << m_rhs << " : " << r->first << " : " << r->second << endmsg;
            ++r;
       } else { 
            if ( l->second.decReport() != r->second.decReport()) {
                always() << " dif: " << l->first << " : " << l->second << "  <-> " << r->second << endmsg;
            } else {
                // always() << " match: " << l->first << " : " << l->second << endmsg;
            }
            ++l;
            ++r;
       }
    }

    return StatusCode::SUCCESS;
}

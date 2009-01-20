// $Id: PatKShortChecker.h,v 1.2 2009-01-20 15:49:30 cattanem Exp $
#ifndef PATKSHORTCHECKER_H 
#define PATKSHORTCHECKER_H 1

// Include files
// from STL
#include <string>


// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Linker/LinkerTool.h"

/** @class PatKShortChecker PatKShortChecker.h
 *  Monitor the KShort in an event
 *
 *  @author Olivier Callot
 *  @date   2002-11-23
 *  @adapt to A-Team framework 2007-08-20 SHM
 */

  class PatKShortChecker : public GaudiAlgorithm {
  public:
    /// Standard constructor
    PatKShortChecker( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~PatKShortChecker( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
    typedef LinkerTool<LHCb::Track,LHCb::MCParticle> TrAsct  ;
  protected:
    
    std::string trackType( const LHCb::Track* tr );
    
    bool isKChild( const LHCb::MCParticle* part );
    
  private:
    std::string  m_inputLocation;
    
    TrAsct*               m_trToMCP;          ///< Truth associator
    TrAsct*               m_seedToMCP;        ///< Truth associator
    TrAsct*               m_downToMCP;        ///< Truth associator
    
    std::vector<int>  m_counter;

    std::vector<double> m_cntSeed;
    std::vector<double> m_cntDown;
  };

#endif // PATKSHORTCHECKER_H

// $Id: PatKShortChecker.h,v 1.1.1.1 2007-08-22 15:38:27 smenzeme Exp $
#ifndef PATKSHORTCHECKER_H 
#define PATKSHORTCHECKER_H 1

// Include files
// from STL
#include <string>


// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

#include "Linker/LinkerTool.h"

#include "PatMCInfo.h"

/** @class PatKShortChecker PatKShortChecker.h
 *  Monitor the KShort in an event
 *
 *  @author Olivier Callot
 *  @date   2002-11-23
 *  @adapt to A-Team framework 2007-08-20 SHM
 */

namespace Tf{

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
    
    IParticlePropertySvc* m_ppSvc;
    PatMCInfo*            m_mcInfo;
    TrAsct*               m_trToMCP;          ///< Truth associator
    TrAsct*               m_seedToMCP;        ///< Truth associator
    TrAsct*               m_downToMCP;        ///< Truth associator
    
    std::vector<int>  m_counter;

    std::vector<double> m_cntSeed;
    std::vector<double> m_cntDown;
  };
}
#endif // PATKSHORTCHECKER_H

// $Id: VeloPixChecker.h,v 1.1.1.1 2007/10/09 18:41:19 smenzeme Exp $
#ifndef VELOPIXCHECKER_H 
#define VELOPIXCHECKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"


/** @class VeloPixChecker VeloPixChecker.h
 *  
 *   Algorithm to produce an NTuple to check Pat tracks.
 *  @author Olivier Callot
 *  @date   2006-05-11
 *  @update for A-Team framework 2007-08-20
 */


  class VeloPixChecker : public GaudiTupleAlg {
  public: 
    /// Standard constructor
    VeloPixChecker( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloPixChecker( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    
  private:
    std::string m_tupleName;
    std::string m_container;
    
  };

#endif // VELOPIXCHECKER_H

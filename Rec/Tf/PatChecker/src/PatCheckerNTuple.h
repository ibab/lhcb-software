// $Id: PatCheckerNTuple.h,v 1.1.1.1 2007-08-22 15:38:27 smenzeme Exp $
#ifndef PATCHECKERNTUPLE_H 
#define PATCHECKERNTUPLE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"


/** @class PatCheckerNTuple PatCheckerNTuple.h
 *  
 *   Algorithm to produce an NTuple to check Pat tracks.
 *  @author Olivier Callot
 *  @date   2006-05-11
 *  @update for A-Team framework 2007-08-20
 */

namespace Tf
{
  class PatCheckerNTuple : public GaudiTupleAlg {
  public: 
    /// Standard constructor
    PatCheckerNTuple( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~PatCheckerNTuple( ); ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    
  private:
    std::string m_tupleName;
    std::string m_container;
    
  };
}
#endif // PATCHECKERNTUPLE_H

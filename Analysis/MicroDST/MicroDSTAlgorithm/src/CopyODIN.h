// $Id: CopyODIN.h,v 1.3 2007-12-11 17:37:12 jpalac Exp $
#ifndef COPYODIN_H 
#define COPYODIN_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"


/** @class CopyODIN CopyODIN.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-15
 */
class CopyODIN : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyODIN( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyODIN( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  typedef MicroDST::BasicCopy<LHCb::ODIN> ODINCopy;
};
#endif // COPYODIN_H

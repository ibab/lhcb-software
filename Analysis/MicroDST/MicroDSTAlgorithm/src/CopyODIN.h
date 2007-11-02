// $Id: CopyODIN.h,v 1.2 2007-11-02 15:19:07 jpalac Exp $
#ifndef COPYODIN_H 
#define COPYODIN_H 1

// Include files
// from Gaudi
#include "MicroDST/CopyAndStoreData.h"


/** @class CopyODIN CopyODIN.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-15
 */
class CopyODIN : public CopyAndStoreData {
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

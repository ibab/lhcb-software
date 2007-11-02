// $Id: CopyRecHeader.h,v 1.2 2007-11-02 15:19:07 jpalac Exp $
#ifndef COPYRECHEADER_H 
#define COPYRECHEADER_H 1

// Include files
// from Gaudi
#include "MicroDST/CopyAndStoreData.h"


/** @class CopyRecHeader CopyRecHeader.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-15
 */
class CopyRecHeader : public CopyAndStoreData {
public: 
  /// Standard constructor
  CopyRecHeader( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyRecHeader( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  typedef MicroDST::BasicCopy<LHCb::RecHeader> RecHeaderCopy;
};
#endif // COPYRECHEADER_H

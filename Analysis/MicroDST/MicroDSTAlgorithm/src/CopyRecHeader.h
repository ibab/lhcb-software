// $Id: CopyRecHeader.h,v 1.1 2007-10-15 16:24:57 jpalac Exp $
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

};
#endif // COPYRECHEADER_H

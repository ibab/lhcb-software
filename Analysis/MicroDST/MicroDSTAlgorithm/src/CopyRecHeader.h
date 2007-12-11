// $Id: CopyRecHeader.h,v 1.3 2007-12-11 17:37:12 jpalac Exp $
#ifndef COPYRECHEADER_H 
#define COPYRECHEADER_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"


/** @class CopyRecHeader CopyRecHeader.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-15
 */
class CopyRecHeader : public MicroDSTAlgorithm {
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

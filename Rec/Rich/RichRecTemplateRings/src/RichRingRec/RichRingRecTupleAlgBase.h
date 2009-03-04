// $Id: RichRingRecTupleAlgBase.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef RICHRINGREC_RICHRINGRECTUPLEALGBASE_H 
#define RICHRINGREC_RICHRINGRECTUPLEALGBASE_H 1

// Include files
// from Gaudi
#include "RichRecBase/RichRecTupleAlgBase.h"
//Interfaces
#include "RichRingRec/IRichRingRecToolBase.h"
// local
#include "RichRingRec/RichDataArrayDefinitions.h"

using namespace LHCb;

using namespace Rich::Rec;



/** @class RichRingRecTupleAlgBase RichRingRecTupleAlgBase.h RichRingRec/RichRingRecTupleAlgBase.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-29
 */
class RichRingRecTupleAlgBase : public RichRecTupleAlgBase {
public: 
  /// Standard constructor
  RichRingRecTupleAlgBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichRingRecTupleAlgBase( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  IRichRingRecToolBase* rt(){  return  m_ringtool; }

protected:

private:
       IRichRingRecToolBase* m_ringtool;

};
#endif // RICHRINGREC_RICHRINGRECTUPLEALGBASE_H

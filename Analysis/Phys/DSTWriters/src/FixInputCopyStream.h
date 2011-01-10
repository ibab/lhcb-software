// $Id: $
#ifndef FIXINPUTCOPYSTREAM_H 
#define FIXINPUTCOPYSTREAM_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IDataStoreLeaves;

/** @class FixInputCopyStream FixInputCopyStream.h
 *  
 *  Algorithm to fix interference problems between OutputStream and
 *  InputCopyStream. To guarantee correct InputCopyStream behaviour, place 
 *  an instance of this algorithm in a sequence such that it runs before
 *  each <b>OutputStream</b>.
 *
 *  @author Juan Palacios
 *  @date   2011-01-10
 */
class FixInputCopyStream : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FixInputCopyStream( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FixInputCopyStream( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  /// Pointer to the (public) tool used to retrieve the objects in a file.
  IDataStoreLeaves *m_leavesTool;

};
#endif // FIXINPUTCOPYSTREAM_H

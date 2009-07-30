// $Id: MicroDSTAlgorithm.h,v 1.3 2009-07-30 06:02:37 jpalac Exp $
#ifndef MICRODSTALGORITHM_H 
#define MICRODSTALGORITHM_H 1

// Include files
// from Gaudi
#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiKernel/IRegistry.h>
// Local
#include <MicroDST/MicroDSTCommon.h>
#include <MicroDST/Functors.hpp>


class ObjectContainerBase;

/** @class MicroDSTAlgorithm MicroDSTAlgorithm.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-11
 */
class GAUDI_API MicroDSTAlgorithm : public MicroDSTCommon<GaudiAlgorithm>
{
public: 
  /// Standard constructor
  MicroDSTAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MicroDSTAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

};
#endif // COPYANDSTOREDATA_H

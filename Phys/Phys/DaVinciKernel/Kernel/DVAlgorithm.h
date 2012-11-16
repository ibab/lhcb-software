
#ifndef DAVINCIKERNEL_DVALGORITHM_H
#define DAVINCIKERNEL_DVALGORITHM_H 1

#include "Kernel/DaVinciTupleAlgorithm.h"

#ifndef DAVINCIKERNEL_DVALGORITHM_NOOBSOLETEWARNING
#warning Including obsolete header DVAlgorithm.h. Use DaVinciAlgorithm, DaVinciHistoAlgorithm or DaVinciTupleAlgorithm instead.
#endif

/** @class DVAlgorithm Kernel/DVAlgorithm.h
 *
 *  OBSOLETE : Please use DaVinciAlgorithm, DaVinciHistoAlgorithm
 *             or DaVinciTupleAlgorithm instead
 *
 *  @author Chris Jones
 *  @date   2012-06-22
 */
class DVAlgorithm : public DaVinciTupleAlgorithm
{

public:

  DVAlgorithm( const std::string& name,
               ISvcLocator* pSvcLocator )
    : DaVinciTupleAlgorithm ( name, pSvcLocator ) { }

  virtual StatusCode initialize();

};

#endif // DAVINCIKERNEL_DVALGORITHM_H


#ifndef DAVINCIKERNEL_DVALGORITHM_H
#define DAVINCIKERNEL_DVALGORITHM_H 1

#include "Kernel/DaVinciTupleAlgorithm.h"

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
  
  virtual StatusCode initialize ()
  {
    const StatusCode sc = DaVinciTupleAlgorithm::initialize();
    warning() << "'" << System::typeinfoName(typeid(*this)) << "'"
              << " is using the depreciated DVAlgorithm base class. "
              << "Please move to DaVinciAlgorithm, DaVinciHistoAlgorithm or DaVinciTupleAlgorithm."
              << endmsg;
    return sc;
  }

};

#endif // DAVINCIKERNEL_DVALGORITHM_H

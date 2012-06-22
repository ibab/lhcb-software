// $Id: DVAlgorithm.h,v 1.59 2010-06-18 09:42:06 jpalac Exp $ 
// ============================================================================
#ifndef DAVINCIKERNEL_DVALGORITHM_H
#define DAVINCIKERNEL_DVALGORITHM_H 1

// Retain for backwards compatibility

#include "Kernel/DaVinciTupleAlgorithm.h"

class DVAlgorithm : public DaVinciTupleAlgorithm
{
public:
  DVAlgorithm( const std::string& name,
               ISvcLocator* pSvcLocator )
    : DaVinciTupleAlgorithm ( name, pSvcLocator ) { }
};

//typedef DaVinciTupleAlgorithm DVAlgorithm;

#endif // DAVINCIKERNEL_DVALGORITHM_H

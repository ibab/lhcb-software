//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetCond/src/component/DetCond_dll.cpp,v 1.1.1.1 2001-09-14 15:07:21 andreav Exp $
#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_FACTORY_ENTRIES( DetCond ) {

  /// Declare services 
  DECLARE_SERVICE  ( ConditionsDBCnvSvc );
  
}

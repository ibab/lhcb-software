 // $Id: CondDBTestAlgorithm.h,v 1.1 2001-12-17 20:13:25 andreav Exp $
#ifndef DETCONDEXAMPLE_CONDDBTESTALGORITHM_H
#define DETCONDEXAMPLE_CONDDBTESTALGORITHM_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class DataObject;

///---------------------------------------------------------------------------
/** @class CondDBTestAlgorithm CondDBTestAlgorithm.h Ex/DetCondExample/CondDBTestAlgorithm.h

    Example of an algorithm retrieving condition data stored in the CondDB.

    Pointers to relevant DataObjects are retrieved at every new event.
    The retrieved objects are explicitly updated to ensure they are valid.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class CondDBTestAlgorithm : public Algorithm {

 public:

  /// Constructor.
  CondDBTestAlgorithm( const std::string& name, 
		       ISvcLocator* pSvcLocator ); 

  // Algorithm standard methods.
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  /// Dump interesting information about a DataObject
  StatusCode i_analyse( DataObject* pObj );

};

#endif    // DETCONDEXAMPLE_CONDDBTESTALGORITHM_H

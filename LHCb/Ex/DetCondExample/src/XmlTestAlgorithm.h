// $Id: XmlTestAlgorithm.h,v 1.1 2001-12-17 20:13:25 andreav Exp $
#ifndef DETCONDEXAMPLE_XMLTESTALGORITHM_H
#define DETCONDEXAMPLE_XMLTESTALGORITHM_H 1

// Base class
#include "GaudiKernel/Algorithm.h"

///---------------------------------------------------------------------------
/** @class XmlTestAlgorithm XmlTestAlgorithm.h DetCondExample/XmlTestAlgorithm.h

    Example of an algorithm retrieving condition data stored in XML files.

    Pointers to relevant DataObjects are retrieved at every new event.
    The retrieved objects are explicitly updated to ensure they are valid.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class XmlTestAlgorithm : public Algorithm {

 public:

  /// Constructor.
  XmlTestAlgorithm( const std::string& name, 
		    ISvcLocator* pSvcLocator ); 

  // Algorithm standard methods.
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

};

#endif    // DETCONDEXAMPLE_XMLTESTALGORITHM_H

// $Id: XmlTestAlgorithm.h,v 1.2 2005-07-14 15:14:17 marcocle Exp $
#ifndef DETCONDEXAMPLE_XMLTESTALGORITHM_H
#define DETCONDEXAMPLE_XMLTESTALGORITHM_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

///---------------------------------------------------------------------------
/** @class XmlTestAlgorithm XmlTestAlgorithm.h DetCondExample/XmlTestAlgorithm.h

    Example of an algorithm retrieving condition data stored in XML files.

    Pointers to relevant DataObjects are retrieved at every new event.
    The retrieved objects are explicitly updated to ensure they are valid.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class XmlTestAlgorithm : public GaudiAlgorithm {

 public:

  /// Constructor.
  XmlTestAlgorithm( const std::string& name, ISvcLocator* pSvcLocator ); 

  // Algorithm standard methods.
  StatusCode execute();

};

#endif    // DETCONDEXAMPLE_XMLTESTALGORITHM_H

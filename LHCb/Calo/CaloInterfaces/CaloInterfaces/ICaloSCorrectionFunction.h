#ifndef CALOSSHAPE_ICALOSCORRECTIONFUNCTION_H
#define CALOSSHAPE_ICALOSCORRECTIONFUNCTION_H 1

// Include files
#include "GaudiKernel/AlgTool.h"

static const InterfaceID IID_ICaloSCorrectionFunction("ICaloSCorrectionFunction",1,0); 

class ICaloSCorrectionFunction : virtual public IAlgTool {
public:
  static const InterfaceID& interfaceID() {return IID_ICaloSCorrectionFunction;}
  virtual StatusCode calculate( double min, 
                                double middle,
                                double max,
                                double e,
                                double x,
                                double y,
                                double& result) = 0;
  virtual StatusCode calculateprime( double min, 
                                     double middle,
                                     double max,
                                     double e,
                                     double x,
                                     double y,
                                     double& result) = 0;
};

#endif // CALOSSHAPE_ICALOSCORRECTIONFUNCTION_H

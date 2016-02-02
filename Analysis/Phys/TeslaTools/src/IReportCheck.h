#ifndef IREPORTCHECK_H 
#define IREPORTCHECK_H 1

#include <string>
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IReportCheck("IReportCheck", 1 , 0);
class IReportCheck : virtual public IAlgTool { 
  
  public:
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IReportCheck; } // Actual operator function
    virtual int VersionTopLevel( std::string, std::string ) = 0;
    virtual int checkBankVersion() = 0;
    virtual unsigned int getTCK() = 0;
};

#endif

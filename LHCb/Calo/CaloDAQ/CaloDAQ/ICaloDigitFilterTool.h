// $Id: $
#ifndef ICALODIGITFILTERTOOL_H 
#define ICALODIGITFILTERTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "Event/CaloDigit.h"
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ICaloDigitFilterTool ( "ICaloDigitFilterTool", 2, 0 );

/** @class ICaloDigitFilterTool ICaloDigitFilterTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2010-12-13
 */
class ICaloDigitFilterTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloDigitFilterTool; }

  virtual int getScale()=0;
  virtual bool cleanDigits(std::string det, bool substr=true, bool mask = true,bool spd=false)=0;
  virtual int method(std::string det)=0;
  virtual double offset(LHCb::CaloCellID id,bool spd=false)=0;    
  virtual double offsetRMS(LHCb::CaloCellID id,bool spd=false)=0;    


protected:

private:



};
#endif // ICALODIGITFILTERTOOL_H

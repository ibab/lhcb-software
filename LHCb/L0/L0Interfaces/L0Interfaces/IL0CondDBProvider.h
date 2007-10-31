// $Id: IL0CondDBProvider.h,v 1.2 2007-10-31 16:42:21 odescham Exp $
#ifndef IL0CONDDBPROVIDER_H 
#define IL0CONDDBPROVIDER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IL0CondDBProvider ( "IL0CondDBProvider", 1, 0 );

/** @class IL0CondDBProvider IL0CondDBProvider.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-10-13
 */
class IL0CondDBProvider : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IL0CondDBProvider; }


  virtual double scale(unsigned int base)=0;
  virtual double caloEtScale()=0;
  virtual double muonPtScale()=0;


protected:

private:

};
#endif // IL0CONDDBPROVIDER_H

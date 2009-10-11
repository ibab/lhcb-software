// $Id: IFunctionFactory.h,v 1.2 2009-10-11 20:41:02 graven Exp $
#ifndef HLTBASE_IFUNCTIONFACTORY_H 
#define HLTBASE_IFUNCTIONFACTORY_H 1

// Include files
// from STL
#include <string>
#include "HltBase/EFunctions.h"

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IFunctionFactory ( "IFunctionFactory", 1, 0 );

/** @class IFunctionFactory IFunctionFactory.h HltBase/IFunctionFactory.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-11-26
 */
template <class T>
class IFunctionFactory : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IFunctionFactory; }
  //FIXME: the above will return the same ID regardless of the template argument
  //       T, resulting in different interfaces returning the same ID...


  virtual bool command(const std::string& command, 
                       const std::string& value="") = 0;

  virtual zen::function<T>* function(const std::string& functioname) = 0;
  virtual zen::filter<T>* filter(const std::string& filtername) = 0;

};

#endif // HLTBASE_IFUNCTIONSFACTORY_H

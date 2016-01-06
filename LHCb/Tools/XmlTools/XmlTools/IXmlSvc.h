// $Id: IXmlSvc.h,v 1.5 2007-02-05 18:51:19 marcocle Exp $

#ifndef DETDESCCNV_IXMLSVC_H
#define DETDESCCNV_IXMLSVC_H

// Include files
#include "GaudiKernel/IInterface.h"
#include "xercesc/dom/DOMDocument.hpp"
#include "XmlTools/IXmlParserSvc.h"



/** @class IXmlSvc IXmlSvc.h DetDescCnv/IXmlSvc.h

    An interface to provide supporting features for XML manipulation. This
    includes among others the interface to the expression evaluation, the
    parameter handling and the xml parsing

    @author Radovan Chytracek
    @author Pere Mato
    @author Sebastien Ponce

*/
class IXmlSvc : public extend_interfaces<IInterface, IXmlParserSvc> {

public:

  /// Declaration of the interface ID (interface id, major version, minor version)
  DeclareInterfaceID( IXmlSvc, 4 , 0);

  /**
   * Tells whether generic conversion of user defined detector elements should
   * be used in case the corresponding user defined converter is not available
   * @return true if generic conversion is allowed
   */
  virtual bool allowGenericCnv() = 0;

  /** 
   * Evaluates a numerical expresion
   * @param expr expresion to evaluate. It may include units and parameters
   * @param check boolean to control if the value needs to be check for being a
   * dimentioned magnitude (with units)
   * @return return double value
   */
  virtual double eval (const char* expr,
                       bool check = true)  = 0;

  /** 
   * Evaluates a numerical expresion
   * @param expr expresion to evaluate. It may include units and parameters
   * @param check boolean to control if the value needs to be check for being a
   * dimentioned magnitude (with units)
   * @return return double value
   */
  virtual double eval (const std::string& expr,
                       bool check = true) = 0;

  /**
   * Adds a parameter in the list of known parameters. The value can also be an
   * expression with units and other parameters.
   * @param name parameter name
   * @param value string which defines the value of the parameter.
   * @return true if success 
   */
  virtual bool addParameter (const std::string& name,
                             const std::string& value) = 0;

  /**
   * Adds a parameter in the list of known parameters. The value can also be an
   * expression with units and other parameters.
   * @param name parameter name
   * @param value string which defines the value of the parameter.
   * @return true if success 
   */
  virtual bool addParameter (const char* name,
                             const char* value) = 0;

  /**
   * Adds a parameter in the list of known parameters. The value can also be an
   * expression with units and other parameters.
   * @param name parameter name
   * @param value string which defines the value of the parameter.
   * @return true if success 
   */
  virtual bool addParameter (const char* name,
                             double value) = 0;

  /**
   * Removes a parameter from the list of known parameters
   * @param name parameter name
   * @return true if success 
   */
  virtual bool removeParameter (const std::string& name) = 0;

  /**
   * Removes a parameter from the list of known parameters
   * @param name parameter name
   * @return true if success 
   */
  virtual bool removeParameter (const char* name) = 0;

};

#endif // DETDESCCNV_IXMLSVC_H

// $Id: IXmlSvc.h,v 1.4 2005-03-18 14:52:34 cattanem Exp $

#ifndef DETDESCCNV_IXMLSVC_H
#define DETDESCCNV_IXMLSVC_H

// Include files
#include "GaudiKernel/IInterface.h"
#include "xercesc/dom/DOMDocument.hpp"
#include "XmlTools/IXmlParserSvc.h"

/// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IXmlSvc(130, 3 , 0);


/** @class IXmlSvc IXmlSvc.h DetDescCnv/IXmlSvc.h

    An interface to provide supporting features for XML manipulation. This
    includes among others the interface to the expression evaluation, the
    parameter handling and the xml parsing

    @author Radovan Chytracek
    @author Pere Mato
    @author Sebastien Ponce

*/
class IXmlSvc : virtual public IInterface,
                virtual public IXmlParserSvc {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IXmlSvc; }

  /**
   * Parses an XmlFile and produces the corresponding DOMTree
   * @param fileName the name of the file to be parsed
   * @return the corresponding DOMTree or 0 if the parsing was not successfull
   */
  virtual xercesc::DOMDocument* parse (const char* fileName) = 0;

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

// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/IXmlSvc.h,v 1.1 2001-01-25 12:12:29 mato Exp $
#ifndef DETDESC_IXMLSVC_H
#define DETDESC_IXMLSVC_H

// Include files
#include "GaudiKernel/IInterface.h"

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IXmlSvc(130, 2 , 0); 


/** @class IXmlSvc IXmlSvc.h DetDesc/IXmlSvc.h

    An interface to provide supporting features for XML manipulation. This includes 
    among others the interface to the expression evaluation and the parameter handling.

    @author Radovan Chytracek
    @author Pere Mato
    @version 2.0 (23/01/01)
*/
class IXmlSvc : virtual public IInterface
{

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IXmlSvc; }

  /** Evaluate a numerical expresion
      @param expr expresion to evaluate. It may include units and parameters
      @param check boolean to control if the value needs to be check for being a 
      dimentioned magnitude (with units)
      @return return double value
  */
  virtual double eval( const char* expr, bool check = true ) const  = 0;
  virtual double eval( const std::string& expr, bool check = true ) const = 0;

  /** Add a parameter in the list of known parameters. The value can also be an expression with
      units and other parameters.
      @param name parameter name
      @param value string which defines the value of the parameter.
      @return true if success 
  */
  virtual bool addParameter( const std::string& name, const std::string& value ) = 0;
  virtual bool addParameter( const char*, const char* value ) = 0;
  virtual bool addParameter( const char* name, double value ) = 0;

  /** Remove a parameter from the list of known parameters
      @param name parameter name
      @return true if success 
  */
  virtual bool removeParameter( const std::string& name ) = 0;
  virtual bool removeParameter( const char* name ) = 0;
};

#endif // DETDESC_IXMLSVC_H

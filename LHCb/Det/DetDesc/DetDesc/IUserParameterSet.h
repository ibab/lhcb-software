//$Id: IUserParameterSet.h,v 1.3 2002-04-24 12:41:26 mato Exp $
#ifndef  DETDESC_IUSERPARAMETERSET_H 
#define  DETDESC_IUSERPARAMETERSET_H 1

#include "GaudiKernel/Kernel.h"
#include <string>
#include <vector>

///---------------------------------------------------------------------------
/** @class IUserParameterSet IUserParameterSet.h Det/DetDesc/IUserParameterSet.h

    Abstract interface to a container for a list of user parameters.
    Its methods are copied from some of those supported by IDetectorElement,
    which eventually could inherit from this interface.

    Two sets of methods are supported in parallel, named after 
    "userParameter" (old style) and "param" (new style).

    @author Andrea Valassi (IUserParameterSet)
    @author Vanya Belyaev (IDetectorElement)
    @date December 2001
*///--------------------------------------------------------------------------

class IUserParameterSet
{

 public:

  /// This defines the type of a parameter.
  enum Kind { DOUBLE, INT, OTHER };

 public:

  // Old style "userParameter" methods. Kept for backward compatibility.

  /**
   * This gets the type of a parameter.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its type
   */
  virtual std::string userParameterType (std::string name) = 0;
  
  /**
   * This gets the comment of a parameter.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its comment
   */
  virtual std::string userParameterComment (std::string name) = 0;
  
  /**
   * This gets the value of a parameter, as a string.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual std::string userParameterAsString (std::string name) = 0;
  
  /**
   * This gets the value of a parameter, as an int.
   * If the value is not an int, raises a UserParameterException.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual int userParameterAsInt (std::string name) = 0;
  
  /**
   * This gets the value of a parameter, as a double.
   * If the value is not a number, raises a UserParameterException.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double userParameterAsDouble (std::string name) = 0;
  
  /**
   * This gets the value of the parameter as a double. 
   * It is equivalent to userParameterAsDouble.
   * If the value is not a double, raises a UserParameterException.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double userParameter (std::string name) = 0;
  
  /**
   * This gets the type of a parameter vector.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its type
   */
  virtual std::string userParameterVectorType (std::string name) = 0;
  
  /**
   * This gets the comment of a parameter vector.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its comment
   */
  virtual std::string userParameterVectorComment (std::string name) = 0;
  
  /**
   * This gets the value of a parameter vector, as a vector of string.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<std::string> 
    userParameterVectorAsString (std::string name) = 0;
  
  /**
   * This gets the value of a parameter vector, as a vector of int.
   * If this is not a vector of int, raises a UserParameterException.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<int>
    userParameterVectorAsInt (std::string name) = 0;
  
  /**
   * This gets the value of a parameter as a vector of double.
   * If this is not a vector of double, raises a UserParameterException.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double>
    userParameterVectorAsDouble (std::string name) = 0;
  
  /**
   * This gets the value of a parameter as a vector of double. 
   * It is equivalent to userParameterVectorAsDouble.
   * If this is not a vector of double, raises a UserParameterException.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double> userParameterVector (std::string name) = 0;

  /**
   * This gets the list of existing parameters as a vector of names.
   * @return the list of parameter names
   */
  virtual std::vector<std::string> userParameters() = 0;
  
  /**
   * This gets the list of existing parameter vectors as a vector of names.
   * @return the list of parameter vector names
   */
  virtual std::vector<std::string> userParameterVectors() = 0;  

 public:

  // New style "param" methods.

  /**
   * This gets the kind of a parameter.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its kind
   */
  virtual IUserParameterSet::Kind paramKind (std::string name) = 0;
  
  /**
   * This gets the type of a parameter.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its type
   */
  virtual std::string paramType (std::string name) = 0;
  
  /**
   * This gets the comment of a parameter.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its comment
   */
  virtual std::string paramComment (std::string name) = 0;
  
  /**
   * This gets the value of a parameter, as a string.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual std::string paramAsString (std::string name) = 0;
  
  /**
   * This gets the value of a parameter, as an int.
   * If the value is not an int, raises a UserParameterException.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual int paramAsInt (std::string name) = 0;
  
  /**
   * This gets the value of a parameter, as a double.
   * If the value is not a number, raises a UserParameterException.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double paramAsDouble (std::string name) = 0;
  
  /**
   * This gets the value of the parameter as a double. 
   * It is equivalent to paramAsDouble.
   * If the value is not a double, raises a UserParameterException.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double param (std::string name) = 0;
  
  /**
   * This gets the kind of a parameter vector.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its kind
   */
  virtual IUserParameterSet::Kind paramVectorKind (std::string name) = 0;
  
  /**
   * This gets the type of a parameter vector.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its type
   */
  virtual std::string paramVectorType (std::string name) = 0;
  
  /**
   * This gets the comment of a parameter vector.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its comment
   */
  virtual std::string paramVectorComment (std::string name) = 0;
  
  /**
   * This gets the value of a parameter vector, as a vector of string.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<std::string> paramVectorAsString (std::string name) = 0;
  
  /**
   * This gets the value of a parameter vector, as a vector of int.
   * If this is not a vector of int, raises a UserParameterException.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<int> paramVectorAsInt (std::string name) = 0;
  
  /**
   * This gets the value of a parameter as a vector of double.
   * If this is not a vector of double, raises a UserParameterException.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double> paramVectorAsDouble (std::string name) = 0;
  
  /**
   * This gets the value of a parameter as a vector of double. 
   * It is equivalent to paramVectorAsDouble.
   * If this is not a vector of double, raises a UserParameterException.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double> paramVector (std::string name) = 0;

  /**
   * This gets the list of existing params as a vector of names.
   * @return the list of param names
   */
  virtual std::vector<std::string> params() = 0;
  
  /**
   * This gets the list of existing paramVectors as a vector of names.
   * @return the list of param names
   */
  virtual std::vector<std::string> paramVectors() = 0;  

 public:

  /// Print the user parameters on a string
  virtual std::string printParams() = 0;

  /// Print the user parameter vectors on a string
  virtual std::string printParamVectors() = 0;

};

#endif // DETDESC_IUSERPARAMETERSET_H


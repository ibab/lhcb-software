//$Id: IParamSet.h,v 1.1 2003-04-23 10:06:42 sponce Exp $
#ifndef  DETDESC_IPARAMSET_H 
#define  DETDESC_IPARAMSET_H 1

#include "GaudiKernel/Kernel.h"
#include <string>
#include <vector>

/** @class IParamSet IParamSet.h Det/DetDesc/IParamSet.h
 *
 *  Abstract interface to a container for a list of user parameters.
 *
 *  Two sets of methods are supported in parallel, named after 
 *  "userParameter" (old style) and "param" (new style). The old style is
 *  deprecated and should be removed in future releases.
 *
 *  @author Sebastien Ponce
 *  @author Andrea Valassi
 *  @author Vanya Belyaev
 */
class IParamSet
{

 public:

  /// This defines the type of a parameter.
  enum Kind { DOUBLE, INT, OTHER };

 public:

  // Old style "userParameter" methods. Kept for backward compatibility.

  /**
   * \deprecated See paramType
   * This gets the type of a parameter.
   * If this parameter does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its type
   */
  virtual std::string userParameterType (std::string name) = 0;
  
  /**
   * \deprecated See paramComment
   * This gets the comment of a parameter.
   * If this parameter does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its comment
   */
  virtual std::string userParameterComment (std::string name) = 0;
  
  /**
   * \deprecated See paramAsString
   * This gets the value of a parameter, as a string.
   * If this parameter does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual std::string userParameterAsString (std::string name) = 0;
  
  /**
   * \deprecated See paramAsInt
   * This gets the value of a parameter, as an int.
   * If the value is not an int, raises a ParamException.
   * If this parameter does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual int userParameterAsInt (std::string name) = 0;
  
  /**
   * \deprecated See paramAsDouble
   * This gets the value of a parameter, as a double.
   * If the value is not a number, raises a ParamException.
   * If this parameter does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double userParameterAsDouble (std::string name) = 0;
  
  /**
   * \deprecated See param
   * This gets the value of the parameter as a double. 
   * It is equivalent to userParameterAsDouble.
   * If the value is not a double, raises a ParamException.
   * If this parameter does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double userParameter (std::string name) = 0;
  
  /**
   * \deprecated See paramVectorType
   * This gets the type of a parameter vector.
   * If this parameter vector does not exist, raises a ParamException.
   * @param name the name of the parameter vector
   * @return its type
   */
  virtual std::string userParameterVectorType (std::string name) = 0;
  
  /**
   * \deprecated See paramVectorComment
   * This gets the comment of a parameter vector.
   * If this parameter vector does not exist, raises a ParamException.
   * @param name the name of the parameter vector
   * @return its comment
   */
  virtual std::string userParameterVectorComment (std::string name) = 0;
  
  /**
   * \deprecated See paramVectorAsString
   * This gets the value of a parameter vector, as a vector of string.
   * If this parameter vector does not exist, raises a ParamException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<std::string> 
    userParameterVectorAsString (std::string name) = 0;
  
  /**
   * \deprecated See paramVectorAsInt
   * This gets the value of a parameter vector, as a vector of int.
   * If this is not a vector of int, raises a ParamException.
   * If this parameter vector does not exist, raises a ParamException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<int>
    userParameterVectorAsInt (std::string name) = 0;
  
  /**
   * \deprecated See paramVectorAsDouble
   * This gets the value of a parameter as a vector of double.
   * If this is not a vector of double, raises a ParamException.
   * If this parameter vector does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double>
    userParameterVectorAsDouble (std::string name) = 0;
  
  /**
   * \deprecated See paramVector
   * This gets the value of a parameter as a vector of double. 
   * It is equivalent to userParameterVectorAsDouble.
   * If this is not a vector of double, raises a ParamException.
   * If this parameter vector does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double> userParameterVector (std::string name) = 0;

  /**
   * \deprecated See params
   * This gets the list of existing parameters as a vector of names.
   * @return the list of parameter names
   */
  virtual std::vector<std::string> userParameters() = 0;
  
  /**
   * \deprecated See paramVectors
   * This gets the list of existing parameter vectors as a vector of names.
   * @return the list of parameter vector names
   */
  virtual std::vector<std::string> userParameterVectors() = 0;  

 public:

  // New style "param" methods.

  /**
   * This gets the kind of a parameter.
   * If this parameter does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its kind
   */
  virtual IParamSet::Kind paramKind (std::string name) = 0;
  
  /**
   * This gets the type of a parameter.
   * If this parameter does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its type
   */
  virtual std::string paramType (std::string name) = 0;
  
  /**
   * This gets the comment of a parameter.
   * If this parameter does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its comment
   */
  virtual std::string paramComment (std::string name) = 0;
  
  /**
   * This gets the value of a parameter, as a string.
   * If this parameter does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual std::string paramAsString (std::string name) = 0;
  
  /**
   * This gets the value of a parameter, as an int.
   * If the value is not an int, raises a ParamException.
   * If this parameter does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual int paramAsInt (std::string name) = 0;
  
  /**
   * This gets the value of a parameter, as a double.
   * If the value is not a number, raises a ParamException.
   * If this parameter does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double paramAsDouble (std::string name) = 0;
  
  /**
   * This gets the value of the parameter as a double. 
   * It is equivalent to paramAsDouble.
   * If the value is not a double, raises a ParamException.
   * If this parameter does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double param (std::string name) = 0;
  
  /**
   * This gets the kind of a parameter vector.
   * If this parameter vector does not exist, raises a ParamException.
   * @param name the name of the parameter vector
   * @return its kind
   */
  virtual IParamSet::Kind paramVectorKind (std::string name) = 0;
  
  /**
   * This gets the type of a parameter vector.
   * If this parameter vector does not exist, raises a ParamException.
   * @param name the name of the parameter vector
   * @return its type
   */
  virtual std::string paramVectorType (std::string name) = 0;
  
  /**
   * This gets the comment of a parameter vector.
   * If this parameter vector does not exist, raises a ParamException.
   * @param name the name of the parameter vector
   * @return its comment
   */
  virtual std::string paramVectorComment (std::string name) = 0;
  
  /**
   * This gets the value of a parameter vector, as a vector of string.
   * If this parameter vector does not exist, raises a ParamException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<std::string> paramVectorAsString (std::string name) = 0;
  
  /**
   * This gets the value of a parameter vector, as a vector of int.
   * If this is not a vector of int, raises a ParamException.
   * If this parameter vector does not exist, raises a ParamException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<int> paramVectorAsInt (std::string name) = 0;
  
  /**
   * This gets the value of a parameter as a vector of double.
   * If this is not a vector of double, raises a ParamException.
   * If this parameter vector does not exist, raises a ParamException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double> paramVectorAsDouble (std::string name) = 0;
  
  /**
   * This gets the value of a parameter as a vector of double. 
   * It is equivalent to paramVectorAsDouble.
   * If this is not a vector of double, raises a ParamException.
   * If this parameter vector does not exist, raises a ParamException.
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

#endif // DETDESC_IPARAMSET_H


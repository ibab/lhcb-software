//$Id: UserParameterSet.h,v 1.1 2001-12-11 10:02:20 sponce Exp $
#ifndef DETDESC_USERPARAMETERSET_H 
#define DETDESC_USERPARAMETERSET_H 1

#include <map>

// Base class
#include "DetDesc/IUserParameterSet.h"

///---------------------------------------------------------------------------
/** @class UserParameterSet UserParameterSet.h DetDesc/UserParameterSet.h

    Simple container for a list of user parameters.
    Now a separate class, it was previously a part of DetectorElement.
    Two sets of methods are supported in parallel, named after "userParameter"
    (old style) and "param" (new style).

    @author Andrea Valassi (UserParameterSet)
    @author Vanya Belyaev (DetectorElement)
    @date December 2001
*///--------------------------------------------------------------------------

class UserParameterSet: virtual public IUserParameterSet
{

 public:

  /// Default constructor
  UserParameterSet();

  /// Copy constructor from an IUserParameterSet
  UserParameterSet( IUserParameterSet& obj );

  /// Assignment operator from an IUserParameterSet
  UserParameterSet& operator= ( IUserParameterSet& obj );

  /// Destructor
  virtual ~UserParameterSet();

 public:

  // Implementation of IUserParameterSet interface.
  // Old style "userParameter" methods. Kept for backward compatibility.

  /**
   * This gets the type of a parameter.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its type
   */
  virtual std::string userParameterType (std::string name);
  
  /**
   * This gets the comment of a parameter.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its comment
   */
  virtual std::string userParameterComment (std::string name);
  
  /**
   * This gets the value of a parameter, as a string.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual std::string userParameterAsString (std::string name);
  
  /**
   * This gets the value of a parameter, as an int.
   * If the value is not an int, raises a UserParameterException.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual int userParameterAsInt (std::string name);
  
  /**
   * This gets the value of a parameter, as a double.
   * If the value is not a number, raises a UserParameterException.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double userParameterAsDouble (std::string name);
  
  /**
   * This gets the value of the parameter as a double. 
   * It is equivalent to userParameterAsDouble.
   * If the value is not a double, raises a UserParameterException.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double userParameter (std::string name);
  
  /**
   * This gets the type of a parameter vector.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its type
   */
  virtual std::string userParameterVectorType (std::string name);
  
  /**
   * This gets the comment of a parameter vector.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its comment
   */
  virtual std::string userParameterVectorComment (std::string name);
  
  /**
   * This gets the value of a parameter vector, as a vector of string.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<std::string> 
    userParameterVectorAsString (std::string name);
  
  /**
   * This gets the value of a parameter vector, as a vector of int.
   * If this is not a vector of int, raises a UserParameterException.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<int>
    userParameterVectorAsInt (std::string name);
  
  /**
   * This gets the value of a parameter as a vector of double.
   * If this is not a vector of double, raises a UserParameterException.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double>
    userParameterVectorAsDouble (std::string name);
  
  /**
   * This gets the value of a parameter as a vector of double. 
   * It is equivalent to userParameterVectorAsDouble.
   * If this is not a vector of double, raises a UserParameterException.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double> userParameterVector (std::string name);

  /**
   * This gets the list of existing parameters as a vector of names.
   * @return the list of parameter names
   */
  virtual std::vector<std::string> userParameters();
  
  /**
   * This gets the list of existing parameter vectors as a vector of names.
   * @return the list of parameter names
   */
  virtual std::vector<std::string> userParameterVectors();  

 public:

  // Implementation of IUserParameter interface.
  // New style "param" methods.

  /**
   * This gets the kind of a parameter.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its kind
   */
  virtual IUserParameterSet::Kind paramKind (std::string name);

  /**
   * This gets the type of a parameter.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its type
   */
  virtual std::string paramType (std::string name);
  
  /**
   * This gets the comment of a parameter.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its comment
   */
  virtual std::string paramComment (std::string name);
  
  /**
   * This gets the value of a parameter, as a string.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual std::string paramAsString (std::string name);
  
  /**
   * This gets the value of a parameter, as an int.
   * If the value is not an int, raises a UserParameterException.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual int paramAsInt (std::string name);
  
  /**
   * This gets the value of a parameter, as a double.
   * If the value is not a number, raises a UserParameterException.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double paramAsDouble (std::string name);
  
  /**
   * This gets the value of the parameter as a double. 
   * It is equivalent to paramAsDouble.
   * If the value is not a double, raises a UserParameterException.
   * If this parameter does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double param (std::string name);
  
  /**
   * This gets the kind of a parameter vector.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its kind
   */
  virtual IUserParameterSet::Kind paramVectorKind (std::string name);
  
  /**
   * This gets the type of a parameter vector.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its type
   */
  virtual std::string paramVectorType (std::string name);
  
  /**
   * This gets the comment of a parameter vector.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its comment
   */
  virtual std::string paramVectorComment (std::string name);
  
  /**
   * This gets the value of a parameter vector, as a vector of string.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<std::string> paramVectorAsString (std::string name);
  
  /**
   * This gets the value of a parameter vector, as a vector of int.
   * If this is not a vector of int, raises a UserParameterException.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<int> paramVectorAsInt (std::string name);
  
  /**
   * This gets the value of a parameter as a vector of double.
   * If this is not a vector of double, raises a UserParameterException.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double> paramVectorAsDouble (std::string name);
  
  /**
   * This gets the value of a parameter as a vector of double. 
   * It is equivalent to paramVectorAsDouble.
   * If this is not a vector of double, raises a UserParameterException.
   * If this parameter vector does not exist, raises a UserParameterException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double> paramVector (std::string name);

  /**
   * This gets the list of existing params as a vector of names.
   * @return the list of param names
   */
  virtual std::vector<std::string> params();
  
  /**
   * This gets the list of existing paramVectors as a vector of names.
   * @return the list of param names
   */
  virtual std::vector<std::string> paramVectors();  

 public:

  // Define new user parameters.
  // Old style "userParameter" methods. Kept for backward compatibility.

  /**
   * This adds a new double parameter.
   * This parameter has a name, a type, a comment and a value, given both
   * as a std::string and as a double.
   * If this parameter already existed, it is replaced by the new one.
   * @param name the name of the parameter
   * @param type the type of the parameter
   * (this is only a clue for the user, it is not used by UserParameter)
   * @param comment a comment on this parameter use
   * @param value the value of the parameter, as a string
   * @param d_value the value of the parameter, as a double
   */
  virtual void addUserParameter (std::string name,
				 std::string type,
				 std::string comment,
				 std::string value,
				 double d_value);

  /**
   * This adds a new integer parameter.
   * This parameter has a name, a type, a comment and a value, given
   * as a std::string, as a double and as an int.
   * If this parameter already existed, it is replaced by the new one.
   * @param name the name of the parameter
   * @param type the type of the parameter
   * (this is only a clue for the user, it is not used by UserParameter)
   * @param comment a comment on this parameter use
   * @param value the value of the parameter, as a string
   * @param d_value the value of the parameter, as a double
   * @param i_value the value of the parameter, as an int
   */
  virtual void addUserParameter (std::string name,
				 std::string type,
				 std::string comment,
				 std::string value,
				 double d_value,
				 int i_value);
  
  /**
   * This adds a new non-numerical parameter.
   * This parameter has a name, a type, a comment and a value, given 
   * as a std::string.
   * If this parameter already existed, it is replaced by the new one.
   * @param name the name of the parameter
   * @param type the type of the parameter
   * (this is only a clue for the user, it is not used by UserParameter)
   * @param comment a comment on this parameter use
   * @param value the value of the parameter, as a string
   */
  virtual void addUserParameter (std::string name,
				 std::string type,
				 std::string comment,
				 std::string value);
  
  /**
   * This adds a new double parameter vector.
   * This parameter vector has a name, a type, a comment and a value,
   * given both as a std::vector<std::string> and as a std::vector<double>.
   * If this parameter vector already existed, it is replaced by the new one.
   * @param name the name of the parameter vector
   * @param type the type of the parameter vector
   * (this is only a clue for the user, it is not used by UserParameter)
   * @param comment a comment on this parameter vector use
   * @param value the value of the parameter vector, as a vector of string
   * @param d_value the value of the parameter vector, as a vector of double
   */
  virtual void addUserParameterVector (std::string name,
				       std::string type,
				       std::string comment,
				       std::vector<std::string> value,
				       std::vector<double> d_value);
  
  /**
   * This adds a new integer parameter vector.
   * This parameter vector has a name, a type, a comment and a value,
   * given as a std::vector<std::string>, as a std::vector<double> and 
   * as a std::vector<int>.
   * If this parameter vector already existed, it is replaced by the new one.
   * @param name the name of the parameter vector
   * @param type the type of the parameter vector
   * (this is only a clue for the user, it is not used by UserParameter)
   * @param comment a comment on this parameter vector use
   * @param value the value of the parameter vector, as a vector of string
   * @param d_value the value of the parameter vector, as a vector of double
   * @param i_value the value of the parameter vector, as a vector of int
   */
  virtual void addUserParameterVector (std::string name,
				       std::string type,
				       std::string comment,
				       std::vector<std::string> value,
				       std::vector<double> d_value,
				       std::vector<int> i_value);
  
  /**
   * This adds a new non-numerical parameter vector.
   * This parameter vector has a name, a type, a comment and a value, 
   * given as a std::vector<std::string>.
   * If this parameter vector already existed, it is replaced by the new one.
   * @param name the name of the parameter vector
   * @param type the type of the parameter vector
   * (this is only a clue for the user, it is not used by UserParameter)
   * @param comment a comment on this parameter vector use
   * @param value the value of the parameter vector, as a vector of string
   */
  virtual void addUserParameterVector (std::string name,
				       std::string type,
				       std::string comment,
				       std::vector<std::string> value);

 public:

  // Define new user parameters.
  // New style "param" methods.

  /**
   * This adds a new double parameter.
   * This parameter has a name, a type, a comment and a value, given both
   * as a std::string and as a double.
   * If this parameter already existed, it is replaced by the new one.
   * @param name the name of the parameter
   * @param type the type of the parameter
   * (this is only a clue for the user, it is not used by UserParameter)
   * @param comment a comment on this parameter use
   * @param value the value of the parameter, as a string
   * @param d_value the value of the parameter, as a double
   */
  virtual void addParam (std::string name,
			 std::string type,
			 std::string comment,
			 std::string value,
			 double d_value);

  /**
   * This adds a new integer parameter.
   * This parameter has a name, a type, a comment and a value, given
   * as a std::string, as a double and as an int.
   * If this parameter already existed, it is replaced by the new one.
   * @param name the name of the parameter
   * @param type the type of the parameter
   * (this is only a clue for the user, it is not used by UserParameter)
   * @param comment a comment on this parameter use
   * @param value the value of the parameter, as a string
   * @param d_value the value of the parameter, as a double
   * @param i_value the value of the parameter, as an int
   */
  virtual void addParam (std::string name,
			 std::string type,
			 std::string comment,
			 std::string value,
			 double d_value,
			 int i_value);
  
  /**
   * This adds a new non-numerical parameter.
   * This parameter has a name, a type, a comment and a value, given 
   * as a std::string.
   * If this parameter already existed, it is replaced by the new one.
   * @param name the name of the parameter
   * @param type the type of the parameter
   * (this is only a clue for the user, it is not used by UserParameter)
   * @param comment a comment on this parameter use
   * @param value the value of the parameter, as a string
   */
  virtual void addParam (std::string name,
			 std::string type,
			 std::string comment,
			 std::string value);
  
  /**
   * This adds a new double parameter vector.
   * This parameter vector has a name, a type, a comment and a value,
   * given both as a std::vector<std::string> and as a std::vector<double>.
   * If this parameter vector already existed, it is replaced by the new one.
   * @param name the name of the parameter vector
   * @param type the type of the parameter vector
   * (this is only a clue for the user, it is not used by UserParameter)
   * @param comment a comment on this parameter vector use
   * @param value the value of the parameter vector, as a vector of string
   * @param d_value the value of the parameter vector, as a vector of double
   */
  virtual void addParamVector (std::string name,
			       std::string type,
			       std::string comment,
			       std::vector<std::string> value,
			       std::vector<double> d_value);
  
  /**
   * This adds a new integer parameter vector.
   * This parameter vector has a name, a type, a comment and a value,
   * given as a std::vector<std::string>, as a std::vector<double> and 
   * as a std::vector<int>.
   * If this parameter vector already existed, it is replaced by the new one.
   * @param name the name of the parameter vector
   * @param type the type of the parameter vector
   * (this is only a clue for the user, it is not used by UserParameter)
   * @param comment a comment on this parameter vector use
   * @param value the value of the parameter vector, as a vector of string
   * @param d_value the value of the parameter vector, as a vector of double
   * @param i_value the value of the parameter vector, as a vector of int
   */
  virtual void addParamVector (std::string name,
			       std::string type,
			       std::string comment,
			       std::vector<std::string> value,
			       std::vector<double> d_value,
			       std::vector<int> i_value);
  
  /**
   * This adds a new non-numerical parameter vector.
   * This parameter vector has a name, a type, a comment and a value, 
   * given as a std::vector<std::string>.
   * If this parameter vector already existed, it is replaced by the new one.
   * @param name the name of the parameter vector
   * @param type the type of the parameter vector
   * (this is only a clue for the user, it is not used by UserParameter)
   * @param comment a comment on this parameter vector use
   * @param value the value of the parameter vector, as a vector of string
   */
  virtual void addParamVector (std::string name,
			       std::string type,
			       std::string comment,
			       std::vector<std::string> value);
  
 private:

  // Private type definitions and data members.

  /// This defines a parameter.
  typedef struct _param {
    std::string type;
    std::string comment;
    std::string value;
    double d_value;
    int i_value;
    IUserParameterSet::Kind kind;
  } Param;

  /// This defines a map of parameters.
  typedef std::map<std::string, Param> ParamMap;

  /// This defines a parameter vector.
  typedef struct _paramVector {
    std::string type;
    std::string comment;
    std::vector<std::string> value;
    std::vector<double> d_value;
    std::vector<int> i_value;
    IUserParameterSet::Kind kind;
  } ParamVector;

  /// This defines a map of parameter vectors.
  typedef std::map<std::string, ParamVector> ParamVectorMap;

  /// This is the list of parameters.
  ParamMap m_paramMap;

  /// This is the list of parameter vectors.
  ParamVectorMap m_paramVectorMap;

};

#endif // DETDESC_USERPARAMETERSET_H


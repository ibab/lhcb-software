//$Id: Condition.h,v 1.2 2001-12-13 18:58:55 andreav Exp $
#ifndef DETDESC_CONDITION_H
#define DETDESC_CONDITION_H 1

#include <string>

// Base classes
#include "DetDesc/IUserParameterSet.h"
#include "DetDesc/ValidDataObject.h"

// Unique ID of the class
#include "DetDesc/CLIDCondition.h"

// Forward declaration
class UserParameterSet;

///---------------------------------------------------------------------------
/** @class Condition Condition.h Det/DetDesc/Condition.h

    A DataObject implementing the IUserParameterSet and IValidity interfaces.
    This is the basic type of condition data in the transient data store
    (alignment, slow control, etc.).

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class Condition : public ValidDataObject, 
                  virtual public IUserParameterSet
{

 public: 
  
  /// Default constructor
  Condition();

  /// Constructor 
  Condition( const ITime& since, const ITime& till );
  
  /// Copy constructor 
  Condition( Condition& obj );
  
  /// Overloaded copy operator
  Condition& operator= ( Condition& obj );

  /// Destructor 
  virtual ~Condition();
  
 public:

  /**
   * This method initializes the condition. It should be overridden
   * and used for computation purposes. This is a kind of hook for adding
   * user code easily in the initialization of a condition.
   */
  virtual StatusCode initialize();

 public:

  // Re-implemented from DataObject

  /// Class ID of this instance
  inline virtual const CLID& clID() const { return classID(); } 

  /// Class ID of this class
  inline static  const CLID& classID() { return CLID_Condition; };

 public:
  
  // Implementation of IUserParameter
  // Old style "userParameter" methods. Kept for backward compatibility.

  /// Get the type of a parameter.
  virtual std::string userParameterType( std::string name );
  
  /// Get the comment of a parameter.
  virtual std::string userParameterComment( std::string name );
  
  /// Get the value of a parameter, as a string.
  virtual std::string userParameterAsString( std::string name );
  
  /// Get the value of a parameter, as an int.
  virtual int userParameterAsInt( std::string name );
  
  /// Get the value of a parameter, as a double.
  virtual double userParameterAsDouble( std::string name );
  
  /// Get the value of a parameter, as a double.
  virtual double userParameter( std::string name );
  
  /// Get the type of a parameter vector.
  virtual std::string userParameterVectorType( std::string name );
  
  /// Get the comment of a parameter vector.
  virtual std::string userParameterVectorComment( std::string name );
  
  /// Get the value of a parameter vector, as a string.
  virtual std::vector<std::string>
    userParameterVectorAsString( std::string name );

  /// Get the value of a parameter vector, as an int.
  virtual std::vector<int> userParameterVectorAsInt( std::string name );

  /// Get the value of a parameter vector, as a double.
  virtual std::vector<double> userParameterVectorAsDouble( std::string name );

  /// Get the value of a parameter vector, as a double.
  virtual std::vector<double> userParameterVector( std::string name );

  /// Returns list of existing parameters as a vector of their names
  virtual std::vector<std::string> userParameters();
  
  /// Returns list of existing parameter vectors as a vector of their names
  virtual std::vector<std::string> userParameterVectors();

 public:

  // Implementation of IUserParameter
  // New style "param" methods.

  /// Get the kind of a parameter.
  virtual IUserParameterSet::Kind paramKind( std::string name );

  /// Get the type of a parameter.
  virtual std::string paramType( std::string name );
  
  /// Get the comment of a parameter.
  virtual std::string paramComment( std::string name );
  
  /// Get the value of a parameter, as a string.
  virtual std::string paramAsString( std::string name );
  
  /// Get the value of a parameter, as an int.
  virtual int paramAsInt( std::string name );
  
  /// Get the value of a parameter, as a double.
  virtual double paramAsDouble( std::string name );
  
  /// Get the value of a parameter, as a double.
  virtual double param( std::string name );
  
  /// Get the kind of a parameter vector.
  virtual IUserParameterSet::Kind paramVectorKind( std::string name );

  /// Get the type of a parameter vector.
  virtual std::string paramVectorType( std::string name );
  
  /// Get the comment of a parameter vector.
  virtual std::string paramVectorComment( std::string name );
  
  /// Get the value of a parameter vector, as a string.
  virtual std::vector<std::string> paramVectorAsString( std::string name );

  /// Get the value of a parameter vector, as an int.
  virtual std::vector<int> paramVectorAsInt( std::string name );

  /// Get the value of a parameter vector, as a double.
  virtual std::vector<double> paramVectorAsDouble( std::string name );

  /// Get the value of a parameter vector, as a double.
  virtual std::vector<double> paramVector( std::string name );

  /// Returns list of existing parameter vectors as a vector of their names
  virtual std::vector<std::string> params();
  
  /// Returns list of existing parameter vectors as a vector of their names
  virtual std::vector<std::string> paramVectors();

 public:

  // Implementation of IUserParameter

  /// Print the user parameters on a string
  virtual std::string printParams();

  /// Print the user parameter vectors on a string
  virtual std::string printParamVectors();

 public:

  // Define new user parameters.
  // Old style "userParameter" methods. Kept for backward compatibility.

  /// Add a new double parameter. 
  virtual void addUserParameter( std::string name,
				 std::string type,
				 std::string comment,
				 std::string value,
				 double d_value );

  /// Add a new integer parameter. 
  virtual void addUserParameter( std::string name,
				 std::string type,
				 std::string comment,
				 std::string value,
				 double d_value,
				 int i_value );
  
  /// Add a new non-numerical parameter. 
  virtual void addUserParameter( std::string name,
				 std::string type,
				 std::string comment,
				 std::string value );
  
  /// Add a new double parameter vector. 
  virtual void addUserParameterVector( std::string name,
				       std::string type,
				       std::string comment,
				       std::vector<std::string> value,
				       std::vector<double> d_value );
  
  /// Add a new integer parameter vector. 
  virtual void addUserParameterVector( std::string name,
				       std::string type,
				       std::string comment,
				       std::vector<std::string> value,
				       std::vector<double> d_value,
				       std::vector<int> i_value );
  
  /// Add a new non-numerical parameter vector. 
  virtual void addUserParameterVector( std::string name,
				       std::string type,
				       std::string comment,
				       std::vector<std::string> value );
  
 public:

  // Define new user parameters.
  // New style "param" methods.

  /// Add a new double parameter. 
  virtual void addParam( std::string name,
			 std::string type,
			 std::string comment,
			 std::string value,
			 double d_value );
  
  /// Add a new integer parameter. 
  virtual void addParam( std::string name,
			 std::string type,
			 std::string comment,
			 std::string value,
			 double d_value,
			 int i_value );
  
  /// Add a new non-numerical parameter. 
  virtual void addParam( std::string name,
			 std::string type,
			 std::string comment,
			 std::string value );
  
  /// Add a new double parameter vector. 
  virtual void addParamVector( std::string name,
			       std::string type,
			       std::string comment,
			       std::vector<std::string> value,
			       std::vector<double> d_value );
  
  /// Add a new integer parameter vector. 
  virtual void addParamVector( std::string name,
			       std::string type,
			       std::string comment,
			       std::vector<std::string> value,
			       std::vector<double> d_value,
			       std::vector<int> i_value );
  
  /// Add a new non-numerical parameter vector. 
  virtual void addParamVector( std::string name,
			       std::string type,
			       std::string comment,
			       std::vector<std::string> value );
  
 private:

  /// Delegation of the IUserParameterSet interface and parameter definition
  UserParameterSet* m_parameterSet;

};

#endif // DETDESC_CONDITION_H

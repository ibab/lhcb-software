//$Id: ParamValidDataObject.h,v 1.1 2003-04-23 10:06:42 sponce Exp $
#ifndef DETDESC_PARAMVALIDDATAOBJECT_H
#define DETDESC_PARAMVALIDDATAOBJECT_H 1

// Base classes
#include "DetDesc/ValidDataObject.h"

// Unique ID of the class
#include "DetDesc/IParamSetEditor.h"
#include "DetDesc/CLIDParamValidDataObject.h"

// Forward declaration
class IParamSet;
class ParamSet;

/** @class ParamValidDataObject ParamValidDataObject.h DetDesc/ParamValidDataObject.h
 *
 *  A DataObject implementing the IParamSetEditor and IValidity
 *  interfaces. This is the basic type of condition data and detector
 *  elements in the transient data store
 *
 *  @author Sebastien Ponce 
 *  @author Andrea Valassi 
*/
class ParamValidDataObject : public ValidDataObject,
                             virtual public IParamSetEditor {

 public: 
  
  /// Default constructor
  ParamValidDataObject();

  /// Constructor 
  ParamValidDataObject (const ITime& since, const ITime& till);
  
  /// Copy constructor 
  ParamValidDataObject (ParamValidDataObject& obj);
  
  /// Destructor 
  virtual ~ParamValidDataObject();
  
 public:

  /**
   * reset to the initial state
   */
  void reset();

 public:

  // Re-implemented from ValidDataObject

  /// Class ID of this instance
  inline virtual const CLID& clID() const { return classID(); } 

  /// Class ID of this class
  inline static  const CLID& classID() { return CLID_ParamValidDataObject; };

 public:
  
  // Implementation of IParam
  // Old style "userParameter" methods. Kept for backward compatibility.

  /// \deprecated See paramType
  virtual std::string userParameterType (std::string name);
  
  /// \deprecated See paramComment
  virtual std::string userParameterComment (std::string name);
  
  /// \deprecated See paramAsString
  virtual std::string userParameterAsString (std::string name);
  
  /// \deprecated See paramAsInt
  virtual int userParameterAsInt (std::string name);
  
  /// \deprecated See paramAsDouble
  virtual double userParameterAsDouble (std::string name);
  
  /// \deprecated See param
  virtual double userParameter (std::string name);
  
  /// \deprecated See paramVectorType
  virtual std::string userParameterVectorType (std::string name);
  
  /// \deprecated See paramVectorComment
  virtual std::string userParameterVectorComment (std::string name);
  
  /// \deprecated See paramVectorAsString
  virtual std::vector<std::string>
    userParameterVectorAsString (std::string name);

  /// \deprecated See paramVectorAsInt
  virtual std::vector<int> userParameterVectorAsInt (std::string name);

  /// \deprecated See paramVectorAsDouble
  virtual std::vector<double> userParameterVectorAsDouble (std::string name);

  /// \deprecated See paramVector
  virtual std::vector<double> userParameterVector (std::string name);

  /// \deprecated See params
  virtual std::vector<std::string> userParameters();
  
  /// \deprecated See paramVectors
  virtual std::vector<std::string> userParameterVectors();

 public:

  // Implementation of IParam
  // New style "param" methods.

  /// Get the kind of a parameter.
  virtual IParamSet::Kind paramKind (std::string name);

  /// Get the type of a parameter.
  virtual std::string paramType (std::string name);
  
  /// Get the comment of a parameter.
  virtual std::string paramComment (std::string name);
  
  /// Get the value of a parameter, as a string.
  virtual std::string paramAsString (std::string name);
  
  /// Get the value of a parameter, as an int.
  virtual int paramAsInt (std::string name);
  
  /// Get the value of a parameter, as a double.
  virtual double paramAsDouble (std::string name);
  
  /// Get the value of a parameter, as a double.
  virtual double param (std::string name);
  
  /// Get the kind of a parameter vector.
  virtual IParamSet::Kind paramVectorKind (std::string name);

  /// Get the type of a parameter vector.
  virtual std::string paramVectorType (std::string name);
  
  /// Get the comment of a parameter vector.
  virtual std::string paramVectorComment (std::string name);
  
  /// Get the value of a parameter vector, as a string.
  virtual std::vector<std::string> paramVectorAsString (std::string name);

  /// Get the value of a parameter vector, as an int.
  virtual std::vector<int> paramVectorAsInt (std::string name);

  /// Get the value of a parameter vector, as a double.
  virtual std::vector<double> paramVectorAsDouble (std::string name);

  /// Get the value of a parameter vector, as a double.
  virtual std::vector<double> paramVector (std::string name);

  /// Returns list of existing parameter vectors as a vector of their names
  virtual std::vector<std::string> params();
  
  /// Returns list of existing parameter vectors as a vector of their names
  virtual std::vector<std::string> paramVectors();

 public:

  // Implementation of IParamSet

  /// Print the parameters on a string
  virtual std::string printParams();

  /// Print the parameter vectors on a string
  virtual std::string printParamVectors();

 public:
  
  // Implementation of IParamEditor
  // Old style "userParameter" methods. Kept for backward compatibility.

  /// \deprecated See addParam
  virtual void addUserParameter (std::string name,
                                 std::string type,
                                 std::string comment,
                                 std::string value,
                                 double d_value);

  /// \deprecated See addParam
  virtual void addUserParameter (std::string name,
                                 std::string type,
                                 std::string comment,
                                 std::string value,
                                 double d_value,
                                 int i_value);
  
  /// \deprecated See addParam
  virtual void addUserParameter (std::string name,
                                 std::string type,
                                 std::string comment,
                                 std::string value);
  
  /// \deprecated See addParamVector
  virtual void addUserParameterVector (std::string name,
                                       std::string type,
                                       std::string comment,
                                       std::vector<std::string> value,
                                       std::vector<double> d_value);
  
  /// \deprecated See addParamVector
  virtual void addUserParameterVector (std::string name,
                                       std::string type,
                                       std::string comment,
                                       std::vector<std::string> value,
                                       std::vector<double> d_value,
                                       std::vector<int> i_value);
  
  /// \deprecated See addParamVector
  virtual void addUserParameterVector (std::string name,
                                       std::string type,
                                       std::string comment,
                                       std::vector<std::string> value);

 public:

  // Implementation of IParamEditor
  // New style "param" methods.

  /**
   * This adds a new double param to the detectorElement.
   * This parameter has a name, a type, a comment and a value, given both
   * as a std::string and as a double.
   * If this parameter was already existing, it is replaced by the new one.
   * @param name the name of the parameter
   * @param type the type of the parameter. This is only a clue for the user, it
   * is not used by the detector element itself
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
   * This adds a new integer param to the detectorElement.
   * This parameter has a name, a type, a comment and a value, given both
   * as a std::string and as an int.
   * If this parameter was already existing, it is replaced by the new one.
   * @param name the name of the parameter
   * @param type the type of the parameter. This is only a clue for the user, it
   * is not used by the detector element itself
   * @param comment a comment on this parameter use
   * @param value the value of the parameter, as a string
   * @param i_value the value of the parameter, as an int
   */
  virtual void addParam (std::string name,
                         std::string type,
                         std::string comment,
                         std::string value,
                         double d_value,
                         int i_value);
  
  /**
   * This adds a new non-numerical param to the detectorElement.
   * This parameter has a name, a type, a comment and a value,
   * given as a std::string.
   * If this parameter was already existing, it is replaced by the new one.
   * @param name the name of the parameter
   * @param type the type of the parameter. This is only a clue for the user, it
   * is not used by the detector element itself
   * @param comment a comment on this parameter use
   * @param value the value of the parameter, as a string
   * @param d_value the value of the parameter, as a double
   */
  virtual void addParam (std::string name,
                         std::string type,
                         std::string comment,
                         std::string value);
  
  /**
   * This adds a new double paramVector to the detectorElement.
   * This parameter has a name, a type, a comment and a value, given both as a
   * std::vector<std::string> and as a std::vector<double>
   * If this parameter vector was already existing, it is replaced by the new
   * one.
   * @param name the name of the parameter vector
   * @param type the type of the parameter vector. This is only a clue for
   * the user, it is not used by the detector element itself
   * @param comment a comment on this parameter vector use
   * @param value the value of the parameter vector, as a vector of strings
   * @param d_value the value of the parameter vector, as a vector of doubles
   */
  virtual void addParamVector (std::string name,
                               std::string type,
                               std::string comment,
                               std::vector<std::string> value,
                               std::vector<double> d_value);
  
  /**
   * This adds a new integer paramVector to the detectorElement.
   * This parameter has a name, a type, a comment and a value, given both as a
   * std::vector<std::string> and as a std::vector<int>
   * If this parameter vector was already existing, it is replaced by the new
   * one.
   * @param name the name of the parameter vector
   * @param type the type of the parameter vector. This is only a clue for
   * the user, it is not used by the detector element itself
   * @param comment a comment on this parameter vector use
   * @param value the value of the parameter vector, as a vector of strings
   * @param d_value the value of the parameter vector, as a vector of ints
   */
  virtual void addParamVector (std::string name,
                               std::string type,
                               std::string comment,
                               std::vector<std::string> value,
                               std::vector<double> d_value,
                               std::vector<int> i_value);
  
  /**
   * This adds a new non-numerical paramVector to the detectorElement.
   * This parameter has a name, a type, a comment and a value, given as a
   * std::vector<std::string>.
   * If this parameter vector was already existing, it is replaced by the new
   * one.
   * @param name the name of the parameter vector
   * @param type the type of the parameter vector. This is only a clue for
   * the user, it is not used by the detector element itself
   * @param comment a comment on this parameter vector use
   * @param value the value of the parameter vector, as a vector of strings
   * @param d_value the value of the parameter vector, as a vector of doubles
   * strings by default
   */
  virtual void addParamVector (std::string name,
                               std::string type,
                               std::string comment,
                               std::vector<std::string> value);
  
private:
  
  /// Delegation of the IParamSet interface and parameter definition
  ParamSet* m_paramSet;

};

#endif // DETDESC_PARAMVALIDDATAOBJECT_H

//$Id: IParamSetEditor.h,v 1.1 2003-04-23 10:06:42 sponce Exp $
#ifndef  DETDESC_IPARAMSETEDITOR_H 
#define  DETDESC_IPARAMSETEDITOR_H 1

#include "DetDesc/IParamSet.h"

/** @class IParamSetEditor IParamSetEditor.h
 *
 *  Abstract interface to a container for a list of user parameters that
 *  can be edited. This extends the IParamSet interface.
 *
 *  Two sets of methods are supported in parallel, named after 
 *  "userParameter" (old style) and "param" (new style). The old style is
 *  deprecated and should be removed in future releases.
 *
 *  @author Sebastien Ponce
 *  @author Andrea Valassi
 *  @author Vanya Belyaev
 */
class IParamSetEditor : virtual public IParamSet
{

public:

  // Old style "userParameter" methods. Kept for backward compatibility.

  /**
   * \deprecated See addParam
   * This adds a new double userParameter to the detectorElement.
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
  virtual void addUserParameter (std::string name,
                                 std::string type,
                                 std::string comment,
                                 std::string value,
                                 double d_value) = 0;

  /**
   * \deprecated See addParam
   * This adds a new integer userParameter to the detectorElement.
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
  virtual void addUserParameter (std::string name,
                                 std::string type,
                                 std::string comment,
                                 std::string value,
                                 double d_value,
                                 int i_value) = 0;
  
  /**
   * \deprecated See addParam
   * This adds a new non-numerical userParameter to the detectorElement.
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
  virtual void addUserParameter (std::string name,
                                 std::string type,
                                 std::string comment,
                                 std::string value) = 0;
  
  /**
   * \deprecated See addParamVector
   * This adds a new double userParameterVector to the detectorElement.
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
  virtual void addUserParameterVector (std::string name,
                                       std::string type,
                                       std::string comment,
                                       std::vector<std::string> value,
                                       std::vector<double> d_value) = 0;
  
  /**
   * \deprecated See addParamVector
   * This adds a new integer userParameterVector to the detectorElement.
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
  virtual void addUserParameterVector (std::string name,
                                       std::string type,
                                       std::string comment,
                                       std::vector<std::string> value,
                                       std::vector<double> d_value,
                                       std::vector<int> i_value) = 0;
  
  /**
   * \deprecated See addParamVector
   * This adds a new non-numerical userParameterVector to the detectorElement.
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
  virtual void addUserParameterVector (std::string name,
                                       std::string type,
                                       std::string comment,
                                       std::vector<std::string> value) = 0;

public:

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
                         double d_value) = 0;

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
                         int i_value) = 0;
  
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
                         std::string value) = 0;
  
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
                               std::vector<double> d_value) = 0;
  
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
                               std::vector<int> i_value) = 0;
  
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
                               std::vector<std::string> value) = 0;

};

#endif // DETDESC_IPARAMSETEDITOR_H


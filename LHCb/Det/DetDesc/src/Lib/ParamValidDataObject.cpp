//$Id: ParamValidDataObject.cpp,v 1.2 2003-06-16 13:42:36 sponce Exp $
#include <string> 

#include "DetDesc/ParamValidDataObject.h"
#include "DetDesc/ParamSet.h"

//---------------------------------------------------------------------------

/// Default constructor
ParamValidDataObject::ParamValidDataObject()
  : IParamSetEditor(), ValidDataObject()
{
  m_paramSet = new ParamSet();
}; 

//---------------------------------------------------------------------------

/// Constructor
ParamValidDataObject::ParamValidDataObject (const ITime& since, 
                                            const ITime& till)
  : IParamSetEditor(), ValidDataObject( since, till )
{
  m_paramSet = new ParamSet();
}; 

//---------------------------------------------------------------------------

/// Copy constructor
ParamValidDataObject::ParamValidDataObject (ParamValidDataObject& obj)
  : IParamSetEditor ((IParamSetEditor&)obj),
    ValidDataObject ((ValidDataObject&)obj)
{
  m_paramSet = new ParamSet ((IParamSet&)obj);
}; 

//---------------------------------------------------------------------------

/// Destructor
ParamValidDataObject::~ParamValidDataObject() 
{
  delete m_paramSet;
};

//----------------------------------------------------------------------------

void ParamValidDataObject::reset() 
{
  /// reset geometry
  if (0 != m_paramSet) { m_paramSet->reset() ;} 
}


//----------------------------------------------------------------------------

/// Get the type of a parameter.
std::string ParamValidDataObject::userParameterType (std::string name) 
{
  // redirection to the new method
  return paramType (name);
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter.
std::string ParamValidDataObject::userParameterComment (std::string name) 
{
  // redirection to the new method
  return paramComment (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a string.
std::string ParamValidDataObject::userParameterAsString (std::string name) 
{
  // redirection to the new method
  return paramAsString (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a int.
int ParamValidDataObject::userParameterAsInt (std::string name) 
{
  // redirection to the new method
  return paramAsInt (name);
};

//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double ParamValidDataObject::userParameterAsDouble (std::string name) 
{
  // redirection to the new method
  return paramAsDouble (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double ParamValidDataObject::userParameter (std::string name) 
{
  // redirection to the new method
  return param (name);
};  
  
//----------------------------------------------------------------------------

/// Get the type of a parameter vector.
std::string ParamValidDataObject::userParameterVectorType (std::string name) 
{
  // redirection to the new method
  return paramVectorType (name);
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter vector.
std::string ParamValidDataObject::userParameterVectorComment (std::string name) 
{
  // redirection to the new method
  return paramVectorComment (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a string vector.
std::vector<std::string> 
ParamValidDataObject::userParameterVectorAsString (std::string name) 
{
  // redirection to the new method
  return paramVectorAsString (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a int vector.
std::vector<int>
ParamValidDataObject::userParameterVectorAsInt (std::string name) 
{
  // redirection to the new method
  return paramVectorAsInt (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double>
ParamValidDataObject::userParameterVectorAsDouble (std::string name) 
{
  // redirection to the new method
  return paramVectorAsDouble (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double>
ParamValidDataObject::userParameterVector (std::string name) 
{
  // redirection to the new method
  return paramVector (name);
};

//----------------------------------------------------------------------------

/// Get the list of existing parameters.
std::vector<std::string> ParamValidDataObject::userParameters() 
{
  // redirection to the new method
  return params();
};
   
//----------------------------------------------------------------------------

/// Get the list of existing parameter vectors.
std::vector<std::string> ParamValidDataObject::userParameterVectors() 
{
  // redirection to the new method
  return paramVectors();
};

//----------------------------------------------------------------------------

/// Get the type of a parameter.
IParamSet::Kind ParamValidDataObject::paramKind (std::string name) 
{
  return m_paramSet->paramKind (name);
};

//----------------------------------------------------------------------------

/// Get the type of a parameter.
std::string ParamValidDataObject::paramType (std::string name) 
{
  return m_paramSet->paramType (name);
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter.
std::string ParamValidDataObject::paramComment (std::string name) 
{
  return m_paramSet->paramComment (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a string.
std::string ParamValidDataObject::paramAsString (std::string name) 
{
  return m_paramSet->paramAsString (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a int.
int ParamValidDataObject::paramAsInt (std::string name) 
{
  return m_paramSet->paramAsInt (name);
};

//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double ParamValidDataObject::paramAsDouble (std::string name) 
{
  return m_paramSet->paramAsDouble (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double ParamValidDataObject::param (std::string name) 
{
  return m_paramSet->param (name);
};  
  
//----------------------------------------------------------------------------

/// Get the kind of a parameter vector.
IParamSet::Kind
ParamValidDataObject::paramVectorKind (std::string name) 
{
  return m_paramSet->paramVectorKind (name);
};
  
//----------------------------------------------------------------------------

/// Get the type of a parameter vector.
std::string ParamValidDataObject::paramVectorType (std::string name) 
{
  return m_paramSet->paramVectorType (name);
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter vector.
std::string ParamValidDataObject::paramVectorComment (std::string name) 
{
  return m_paramSet->paramVectorComment (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a string vector.
std::vector<std::string> 
ParamValidDataObject::paramVectorAsString (std::string name) 
{
  return m_paramSet->paramVectorAsString (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a int vector.
std::vector<int>
ParamValidDataObject::paramVectorAsInt (std::string name) 
{
  return m_paramSet->paramVectorAsInt (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double>
ParamValidDataObject::paramVectorAsDouble (std::string name) 
{
  return m_paramSet->paramVectorAsDouble (name);
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double> ParamValidDataObject::paramVector (std::string name) 
{
  return m_paramSet->paramVector (name);
};

//----------------------------------------------------------------------------

/// Get the list of existing parameters.
std::vector<std::string> ParamValidDataObject::params() 
{
  return m_paramSet->params();
};
   
//----------------------------------------------------------------------------

/// Get the list of existing parameter vectors.
std::vector<std::string> ParamValidDataObject::paramVectors() 
{
  return m_paramSet->paramVectors();
};

//----------------------------------------------------------------------------

/// addUserParameter
void ParamValidDataObject::addUserParameter (std::string name,
                                             std::string type,
                                             std::string comment,
                                             std::string value,
                                             double d_value) {
  // redirection to the new method
  addParam (name, type, comment, value, d_value);
}
 
//----------------------------------------------------------------------------
  
/// addUserParameter
void ParamValidDataObject::addUserParameter (std::string name,
                                             std::string type,
                                             std::string comment,
                                             std::string value,
                                             double d_value,
                                             int i_value) {
  // redirection to the new method
  addParam (name, type, comment, value, d_value, i_value);
}
 
//----------------------------------------------------------------------------
  
/// addUserParameter
void ParamValidDataObject::addUserParameter (std::string name,
                                             std::string type,
                                             std::string comment,
                                             std::string value) {
  // redirection to the new method
  addParam (name, type, comment, value);
}
 
//----------------------------------------------------------------------------
  
/// addUserParameterVector
void
ParamValidDataObject::addUserParameterVector (std::string name,
                                              std::string type,
                                              std::string comment,
                                              std::vector<std::string> value,
                                              std::vector<double> d_value) {
  // redirection to the new method
  addParamVector (name, type, comment, value, d_value);
}
 
//----------------------------------------------------------------------------

/// addUserParameterVector
void
ParamValidDataObject::addUserParameterVector (std::string name,
                                              std::string type,
                                              std::string comment,
                                              std::vector<std::string> value,
                                              std::vector<double> d_value,
                                              std::vector<int> i_value) {
  // redirection to the new method
  addParamVector (name, type, comment, value, d_value, i_value);
}
 
//----------------------------------------------------------------------------

/// addUserParameterVector
void
ParamValidDataObject::addUserParameterVector (std::string name,
                                              std::string type,
                                              std::string comment,
                                              std::vector<std::string> value) {
  // redirection to the new method
  addParamVector (name, type, comment, value);
}
 
//----------------------------------------------------------------------------

/// Add a new double parameter.
void ParamValidDataObject::addParam (std::string name,
                                     std::string type,
                                     std::string comment,
                                     std::string value,
                                     double d_value) {
  m_paramSet->addParam (name, type, comment, value, d_value);
  return;
};
 
//----------------------------------------------------------------------------
 
/// Add a new integer parameter.
void ParamValidDataObject::addParam (std::string name,
                                     std::string type,
                                     std::string comment,
                                     std::string value,
                                     double d_value,
                                     int i_value) {
  m_paramSet->addParam (name, type, comment, value, d_value, i_value);
  return;
};

//----------------------------------------------------------------------------

/// Add a new non-numerical parameter.
void ParamValidDataObject::addParam (std::string name,
                                     std::string type,
                                     std::string comment,
                                     std::string value) {
  m_paramSet->addParam (name, type, comment, value);
  return;
};
  
//----------------------------------------------------------------------------

/// Add a new double parameter vector.
void ParamValidDataObject::addParamVector (std::string name,
                                           std::string type,
                                           std::string comment,
                                           std::vector<std::string> value,
                                           std::vector<double> d_value) {
  m_paramSet->addParamVector (name, type, comment, value, d_value);
  return;
};

//----------------------------------------------------------------------------

/// Add a new integer parameter vector.
void ParamValidDataObject::addParamVector (std::string name,
                                           std::string type,
                                           std::string comment,
                                           std::vector<std::string> value,
                                           std::vector<double> d_value,
                                           std::vector<int> i_value) {
  m_paramSet->addParamVector (name, type, comment, value, d_value, i_value);
  return;
};

//----------------------------------------------------------------------------

/// Add a new non-numerical parameter vector.
void ParamValidDataObject::addParamVector (std::string name,
                                           std::string type,
                                           std::string comment,
                                           std::vector<std::string> value) {
  m_paramSet->addParamVector (name, type, comment, value);
  return;
};

//----------------------------------------------------------------------------

/// Print the user parameters on a string
std::string ParamValidDataObject::printParams() {
  return m_paramSet->printParams();
};

//----------------------------------------------------------------------------

/// Print the user parameter vectors on a string
std::string ParamValidDataObject::printParamVectors() {
  return m_paramSet->printParamVectors();
};

//----------------------------------------------------------------------------

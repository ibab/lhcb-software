//$Id: UserParameterSet.cpp,v 1.1 2001-12-11 10:02:28 sponce Exp $

// Include files
#include "DetDesc/UserParameterSet.h"
#include "DetDesc/UserParameterException.h"

//----------------------------------------------------------------------------

/// Constructor
UserParameterSet::UserParameterSet() 
{
};

//----------------------------------------------------------------------------

/// Copy constructor from an IUserParameterSet
UserParameterSet::UserParameterSet( IUserParameterSet& obj ) {
  std::vector<std::string>::iterator it;
  std::vector<std::string> params = obj.params();
  for ( it = params.begin(); params.end() != it; ++it ) {
    IUserParameterSet::Kind kind = obj.paramKind( *it );
    switch( kind )
      {
      case IUserParameterSet::INT : 
	addParam( *it, 
		  obj.paramType(*it), 
		  obj.paramComment(*it), 
		  obj.paramAsString(*it), 
		  obj.paramAsDouble(*it),
		  obj.paramAsInt(*it) );
	break;
      case IUserParameterSet::DOUBLE : 
	addParam( *it, 
		  obj.paramType(*it), 
		  obj.paramComment(*it), 
		  obj.paramAsString(*it), 
		  obj.paramAsDouble(*it) );
	break;
      case IUserParameterSet::OTHER : 
	addParam( *it, 
		  obj.paramType(*it), 
		  obj.paramComment(*it), 
		  obj.paramAsString(*it) );
	break;
      default :  
	throw 
	  UserParameterException("Unknown kind for param with name : \""
				 + *it + "\" !");
	break;
      }
  }
  std::vector<std::string> paramVectors = obj.paramVectors();
  for ( it = paramVectors.begin(); paramVectors.end() != it; ++it ) {
    IUserParameterSet::Kind kind = obj.paramVectorKind( *it );
    switch( kind )
      {
      case IUserParameterSet::INT : 
	addParamVector( *it, 
			obj.paramVectorType(*it), 
			obj.paramVectorComment(*it), 
			obj.paramVectorAsString(*it), 
			obj.paramVectorAsDouble(*it),
			obj.paramVectorAsInt(*it) );
	break;
      case IUserParameterSet::DOUBLE : 
	addParamVector( *it, 
			obj.paramVectorType(*it), 
			obj.paramVectorComment(*it), 
			obj.paramVectorAsString(*it), 
			obj.paramVectorAsDouble(*it) );
	break;
      case IUserParameterSet::OTHER : 
	addParamVector( *it, 
			obj.paramVectorType(*it), 
			obj.paramVectorComment(*it), 
			obj.paramVectorAsString(*it) );
	break;
      default :  
	throw 
	  UserParameterException("Unknown kind for paramVector with name : \""
				 + *it + "\" !");
	break;
      }
  }
};

//---------------------------------------------------------------------------

/// Assignment operator from an IUserParameterSet
UserParameterSet& UserParameterSet::operator= ( IUserParameterSet& obj ) {
  std::vector<std::string>::iterator it;
  std::vector<std::string> params = obj.params();
  for ( it = params.begin(); params.end() != it; ++it ) {
    IUserParameterSet::Kind kind = obj.paramKind( *it );
    switch( kind )
      {
      case IUserParameterSet::INT : 
	this->addParam( *it, 
			obj.paramType(*it), 
			obj.paramComment(*it), 
			obj.paramAsString(*it), 
			obj.paramAsDouble(*it),
			obj.paramAsInt(*it) );
	break;
      case IUserParameterSet::DOUBLE : 
	this->addParam( *it, 
			obj.paramType(*it), 
			obj.paramComment(*it), 
			obj.paramAsString(*it), 
			obj.paramAsDouble(*it) );
	break;
      case IUserParameterSet::OTHER : 
	this->addParam( *it, 
			obj.paramType(*it), 
			obj.paramComment(*it), 
			obj.paramAsString(*it) );
	break;
      default :  
	throw 
	  UserParameterException("Unknown kind for param with name : \""
				 + *it + "\" !");
	break;
      }
  }
  std::vector<std::string> paramVectors = obj.paramVectors();
  for ( it = paramVectors.begin(); paramVectors.end() != it; ++it ) {
    IUserParameterSet::Kind kind = obj.paramVectorKind( *it );
    switch( kind )
      {
      case IUserParameterSet::INT : 
	this->addParamVector( *it, 
			      obj.paramVectorType(*it), 
			      obj.paramVectorComment(*it), 
			      obj.paramVectorAsString(*it), 
			      obj.paramVectorAsDouble(*it),
			      obj.paramVectorAsInt(*it) );
	break;
      case IUserParameterSet::DOUBLE : 
	this->addParamVector( *it, 
			      obj.paramVectorType(*it), 
			      obj.paramVectorComment(*it), 
			      obj.paramVectorAsString(*it), 
			      obj.paramVectorAsDouble(*it) );
	break;
      case IUserParameterSet::OTHER : 
	this->addParamVector( *it, 
			      obj.paramVectorType(*it), 
			      obj.paramVectorComment(*it), 
			      obj.paramVectorAsString(*it) );
	break;
      default :  
	throw 
	  UserParameterException("Unknown kind for paramVector with name : \""
				 + *it + "\" !");
	break;
      }
  }
  return *this;
};

//----------------------------------------------------------------------------

/// Destructor
UserParameterSet::~UserParameterSet() 
{ 
};

//----------------------------------------------------------------------------

/// Add a new double parameter.
void UserParameterSet::addParam( std::string name,
				 std::string type,
				 std::string comment,
				 std::string value,
				 double d_value ) 
{
  Param aParam;
  aParam.type = type;
  aParam.comment = comment;
  aParam.value = value;
  aParam.d_value = d_value;
  // aParam.i_value is never used
  aParam.kind = IUserParameterSet::DOUBLE;
  m_paramMap[name] = aParam;
};

//----------------------------------------------------------------------------

/// Add a new integer parameter.
void UserParameterSet::addParam( std::string name,
				 std::string type,
				 std::string comment,
				 std::string value,
				 double d_value,
				 int i_value ) 
{
  Param aParam;
  aParam.type = type;
  aParam.comment = comment;
  aParam.value = value;
  aParam.d_value = d_value;
  aParam.i_value = i_value;
  aParam.kind = IUserParameterSet::INT;
  m_paramMap[name] = aParam;
};
  
//----------------------------------------------------------------------------

/// Add a new non-numerical parameter.
void UserParameterSet::addParam( std::string name,
				 std::string type,
				 std::string comment,
				 std::string value ) 
{
  Param aParam;
  aParam.type = type;
  aParam.comment = comment;
  aParam.value = value;
  // aParam.d_value is never used
  // aParam.i_value is never used
  aParam.kind = IUserParameterSet::OTHER;
  m_paramMap[name] = aParam;
};
  
//----------------------------------------------------------------------------

/// Add a new double parameter vector.
void UserParameterSet::addParamVector( std::string name,
				       std::string type,
				       std::string comment,
				       std::vector<std::string> value,
				       std::vector<double> d_value ) 
{
  ParamVector aParamVector;
  aParamVector.type = type;
  aParamVector.comment = comment;
  aParamVector.value = value;
  aParamVector.d_value = d_value;
  // aParamVector.i_value is never used
  aParamVector.kind = IUserParameterSet::DOUBLE;
  m_paramVectorMap[name] = aParamVector;
};

//----------------------------------------------------------------------------

/// Add a new integer parameter vector.
void UserParameterSet::addParamVector( std::string name,
				       std::string type,
				       std::string comment,
				       std::vector<std::string> value,
				       std::vector<double> d_value,
				       std::vector<int> i_value ) 
{
  ParamVector aParamVector;
  aParamVector.type = type;
  aParamVector.comment = comment;
  aParamVector.value = value;
  aParamVector.d_value = d_value;
  aParamVector.i_value = i_value;
  aParamVector.kind = IUserParameterSet::INT;
  m_paramVectorMap[name] = aParamVector;
};

//----------------------------------------------------------------------------

/// Add a new non-numerical parameter vector.
void UserParameterSet::addParamVector( std::string name,
				       std::string type,
				       std::string comment,
				       std::vector<std::string> value )
{
  ParamVector aParamVector;
  aParamVector.type = type;
  aParamVector.comment = comment;
  aParamVector.value = value;
  // aParamVector.d_value is never used
  // aParamVector.i_value is never used
  aParamVector.kind = IUserParameterSet::OTHER;
  m_paramVectorMap[name] = aParamVector;
};

//----------------------------------------------------------------------------

/// Get the kind of a parameter.
IUserParameterSet::Kind UserParameterSet::paramKind( std::string name ) 
{
  if (m_paramMap.find(name) == m_paramMap.end()) {
    throw UserParameterException("No param with this name : \""
				 + name + "\" !");
  }
  return m_paramMap[name].kind;
};
  
//----------------------------------------------------------------------------

/// Get the type of a parameter.
std::string UserParameterSet::paramType( std::string name ) 
{
  if (m_paramMap.find(name) == m_paramMap.end()) {
    throw UserParameterException("No param with this name : \""
				 + name + "\" !");
  }
  return m_paramMap[name].type;
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter.
std::string UserParameterSet::paramComment( std::string name ) 
{
  if (m_paramMap.find(name) == m_paramMap.end()) {
    throw UserParameterException("No param with this name : \""
				 + name + "\" !");
  }
  return m_paramMap[name].comment;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a string.
std::string UserParameterSet::paramAsString( std::string name ) 
{
  if (m_paramMap.find(name) == m_paramMap.end()) {
    throw UserParameterException("No param with this name : \""
				 + name + "\" !");
  }
  return m_paramMap[name].value;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a int.
int UserParameterSet::paramAsInt( std::string name ) 
{
  if (m_paramMap.find(name) == m_paramMap.end()) {
    throw UserParameterException("No param with this name : \""
				 + name + "\" !");
  }
  if (m_paramMap[name].kind != IUserParameterSet::INT) {
    throw UserParameterException("param " + name +
				 " does not have an integer value.");
  }
  return m_paramMap[name].i_value;
};

//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double UserParameterSet::paramAsDouble( std::string name ) 
{
  if (m_paramMap.find(name) == m_paramMap.end()) {
    throw UserParameterException("No param with this name : \""
				 + name + "\" !");
  }
  if (m_paramMap[name].kind != IUserParameterSet::DOUBLE &&
      m_paramMap[name].kind != IUserParameterSet::INT ) {
    throw UserParameterException("param " + name +
				 " does not have a numerical value.");
  }
  return m_paramMap[name].d_value;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double UserParameterSet::param( std::string name ) 
{
  return paramAsDouble (name);
}  
  
//----------------------------------------------------------------------------

/// Get the kind of a parameter vector.
IUserParameterSet::Kind UserParameterSet::paramVectorKind( std::string name ) 
{
  if (m_paramVectorMap.find(name) == m_paramVectorMap.end()) {
    throw UserParameterException("No paramVector with this name : \""
				 + name + "\" !");
  }
  return m_paramVectorMap[name].kind;
};
  
//----------------------------------------------------------------------------

/// Get the type of a parameter vector.
std::string UserParameterSet::paramVectorType( std::string name ) 
{
  if (m_paramVectorMap.find(name) == m_paramVectorMap.end()) {
    throw UserParameterException("No paramVector with this name : \""
				 + name + "\" !");
  }
  return m_paramVectorMap[name].type;
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter vector.
std::string UserParameterSet::paramVectorComment( std::string name ) 
{
  if (m_paramVectorMap.find(name) == m_paramVectorMap.end()) {
    throw UserParameterException("No paramVector with this name : \""
				 + name + "\" !");
  }
  return m_paramVectorMap[name].comment;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a string vector.
std::vector<std::string> 
UserParameterSet::paramVectorAsString( std::string name ) 
{
  if (m_paramVectorMap.find(name) == m_paramVectorMap.end()) {
    throw UserParameterException("No paramVector with this name : \""
				 + name + "\" !");
  }
  return m_paramVectorMap[name].value;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a int vector.
std::vector<int>
UserParameterSet::paramVectorAsInt( std::string name ) 
{
  if (m_paramVectorMap.find(name) == m_paramVectorMap.end()) {
    throw UserParameterException("No paramVector with this name : \""
				 + name + "\" !");
  }
  if (m_paramVectorMap[name].kind != IUserParameterSet::INT) {
    throw UserParameterException("paramVector " + name +
				 " does not have integer values.");
  }
  return m_paramVectorMap[name].i_value;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double>
UserParameterSet::paramVectorAsDouble( std::string name ) 
{
  if (m_paramVectorMap.find(name) == m_paramVectorMap.end()) {
    throw UserParameterException("No paramVector with this name : \""
				 + name + "\" !");
  }
  if (m_paramMap[name].kind != IUserParameterSet::DOUBLE &&
      m_paramMap[name].kind != IUserParameterSet::INT ) {
    throw UserParameterException("paramVector " + name +
				 " does not have numerical values.");
  }
  return m_paramVectorMap[name].d_value;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double> UserParameterSet::paramVector( std::string name ) 
{
  return paramVectorAsDouble (name);
};

//----------------------------------------------------------------------------

/// Get the list of existing parameters.
std::vector<std::string> UserParameterSet::params() 
{
  std::vector<std::string> result;
  for (ParamMap::iterator it = m_paramMap.begin();
       m_paramMap.end() != it;
       ++it) {
    result.push_back(it->first);
  }
  return result;
};
   
//----------------------------------------------------------------------------

/// Get the list of existing parameter vectors.
std::vector<std::string> UserParameterSet::paramVectors() 
{
  std::vector<std::string> result;
  for (ParamVectorMap::iterator it = m_paramVectorMap.begin();
       m_paramVectorMap.end() != it;
       ++it) {
    result.push_back(it->first);
  }
  return result;
};

//----------------------------------------------------------------------------

/// Add a new double parameter.
void UserParameterSet::addUserParameter( std::string name,
					 std::string type,
					 std::string comment,
					 std::string value,
					 double d_value ) 
{
  return addParam( name, type, comment, value, d_value );
};

//----------------------------------------------------------------------------

/// Add a new integer parameter.
void UserParameterSet::addUserParameter( std::string name,
					 std::string type,
					 std::string comment,
					 std::string value,
					 double d_value,
					 int i_value ) 
{
  return addParam( name, type, comment, value, d_value, i_value );
};
  
//----------------------------------------------------------------------------

/// Add a new non-numerical parameter.
void UserParameterSet::addUserParameter( std::string name,
					 std::string type,
					 std::string comment,
					 std::string value ) 
{
  return addParam( name, type, comment, value );
};
  
//----------------------------------------------------------------------------

/// Add a new double parameter vector.
void UserParameterSet::addUserParameterVector( std::string name,
					       std::string type,
					       std::string comment,
					       std::vector<std::string> value,
					       std::vector<double> d_value ) 
{
  return addParamVector( name, type, comment, value, d_value );
};

//----------------------------------------------------------------------------

/// Add a new integer parameter vector.
void UserParameterSet::addUserParameterVector( std::string name,
					       std::string type,
					       std::string comment,
					       std::vector<std::string> value,
					       std::vector<double> d_value,
					       std::vector<int> i_value ) 
{
  return addParamVector( name, type, comment, value, d_value, i_value );
};

//----------------------------------------------------------------------------

/// Add a new non-numerical parameter vector.
void UserParameterSet::addUserParameterVector( std::string name,
					       std::string type,
					       std::string comment,
					       std::vector<std::string> value )
{
  return addParamVector( name, type, comment, value );
};

//----------------------------------------------------------------------------

/// Get the type of a parameter.
std::string UserParameterSet::userParameterType( std::string name ) 
{
  return paramType( name );
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter.
std::string UserParameterSet::userParameterComment( std::string name ) 
{
  return paramComment( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a string.
std::string UserParameterSet::userParameterAsString( std::string name ) 
{
  return paramAsString( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a int.
int UserParameterSet::userParameterAsInt( std::string name ) 
{
  return paramAsInt( name );
};

//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double UserParameterSet::userParameterAsDouble( std::string name ) 
{
  return paramAsDouble( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double UserParameterSet::userParameter( std::string name ) 
{
  return userParameterAsDouble (name);
};  
  
//----------------------------------------------------------------------------

/// Get the type of a parameter vector.
std::string UserParameterSet::userParameterVectorType( std::string name ) 
{
  return paramVectorType( name );
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter vector.
std::string UserParameterSet::userParameterVectorComment( std::string name ) 
{
  return paramVectorComment( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a string vector.
std::vector<std::string> 
UserParameterSet::userParameterVectorAsString( std::string name ) 
{
  return paramVectorAsString( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a int vector.
std::vector<int>
UserParameterSet::userParameterVectorAsInt( std::string name ) 
{
  return paramVectorAsInt( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double>
UserParameterSet::userParameterVectorAsDouble( std::string name ) 
{
  return paramVectorAsDouble( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double> UserParameterSet::userParameterVector( std::string name ) 
{
  return paramVector( name );
};

//----------------------------------------------------------------------------

/// Get the list of existing parameters.
std::vector<std::string> UserParameterSet::userParameters() 
{
  return params();
};
   
//----------------------------------------------------------------------------

/// Get the list of existing parameter vectors.
std::vector<std::string> UserParameterSet::userParameterVectors() 
{
  return paramVectors();
};

//----------------------------------------------------------------------------


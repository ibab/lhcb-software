//$Id: ParamSet.cpp,v 1.1 2003-04-23 10:06:49 sponce Exp $

#include "DetDesc/ParamSet.h"
#include "DetDesc/ParamException.h"

//----------------------------------------------------------------------------

/// Constructor
ParamSet::ParamSet() 
{
};

//----------------------------------------------------------------------------

/// Copy constructor from an IParamSet
ParamSet::ParamSet( IParamSet& obj ) {
  std::vector<std::string>::iterator it;
  std::vector<std::string> params = obj.params();
  for ( it = params.begin(); params.end() != it; ++it ) {
    IParamSet::Kind kind = obj.paramKind( *it );
    switch( kind )
      {
      case IParamSet::INT : 
	addParam( *it, 
		  obj.paramType(*it), 
		  obj.paramComment(*it), 
		  obj.paramAsString(*it), 
		  obj.paramAsDouble(*it),
		  obj.paramAsInt(*it) );
	break;
      case IParamSet::DOUBLE : 
	addParam( *it, 
		  obj.paramType(*it), 
		  obj.paramComment(*it), 
		  obj.paramAsString(*it), 
		  obj.paramAsDouble(*it) );
	break;
      case IParamSet::OTHER : 
	addParam( *it, 
		  obj.paramType(*it), 
		  obj.paramComment(*it), 
		  obj.paramAsString(*it) );
	break;
      default :  
	throw 
	  ParamException("Unknown kind for param with name : \""
				 + *it + "\" !");
	break;
      }
  }
  std::vector<std::string> paramVectors = obj.paramVectors();
  for ( it = paramVectors.begin(); paramVectors.end() != it; ++it ) {
    IParamSet::Kind kind = obj.paramVectorKind( *it );
    switch( kind )
      {
      case IParamSet::INT : 
	addParamVector( *it, 
			obj.paramVectorType(*it), 
			obj.paramVectorComment(*it), 
			obj.paramVectorAsString(*it), 
			obj.paramVectorAsDouble(*it),
			obj.paramVectorAsInt(*it) );
	break;
      case IParamSet::DOUBLE : 
	addParamVector( *it, 
			obj.paramVectorType(*it), 
			obj.paramVectorComment(*it), 
			obj.paramVectorAsString(*it), 
			obj.paramVectorAsDouble(*it) );
	break;
      case IParamSet::OTHER : 
	addParamVector( *it, 
			obj.paramVectorType(*it), 
			obj.paramVectorComment(*it), 
			obj.paramVectorAsString(*it) );
	break;
      default :  
	throw 
	  ParamException("Unknown kind for paramVector with name : \""
				 + *it + "\" !");
	break;
      }
  }
};

//---------------------------------------------------------------------------

/// Assignment operator from an IParamSet
ParamSet& ParamSet::operator= ( IParamSet& obj ) {
  std::vector<std::string>::iterator it;
  std::vector<std::string> params = obj.params();
  for ( it = params.begin(); params.end() != it; ++it ) {
    IParamSet::Kind kind = obj.paramKind( *it );
    switch( kind )
      {
      case IParamSet::INT : 
	this->addParam( *it, 
			obj.paramType(*it), 
			obj.paramComment(*it), 
			obj.paramAsString(*it), 
			obj.paramAsDouble(*it),
			obj.paramAsInt(*it) );
	break;
      case IParamSet::DOUBLE : 
	this->addParam( *it, 
			obj.paramType(*it), 
			obj.paramComment(*it), 
			obj.paramAsString(*it), 
			obj.paramAsDouble(*it) );
	break;
      case IParamSet::OTHER : 
	this->addParam( *it, 
			obj.paramType(*it), 
			obj.paramComment(*it), 
			obj.paramAsString(*it) );
	break;
      default :  
	throw 
	  ParamException("Unknown kind for param with name : \""
				 + *it + "\" !");
	break;
      }
  }
  std::vector<std::string> paramVectors = obj.paramVectors();
  for ( it = paramVectors.begin(); paramVectors.end() != it; ++it ) {
    IParamSet::Kind kind = obj.paramVectorKind( *it );
    switch( kind )
      {
      case IParamSet::INT : 
	this->addParamVector( *it, 
			      obj.paramVectorType(*it), 
			      obj.paramVectorComment(*it), 
			      obj.paramVectorAsString(*it), 
			      obj.paramVectorAsDouble(*it),
			      obj.paramVectorAsInt(*it) );
	break;
      case IParamSet::DOUBLE : 
	this->addParamVector( *it, 
			      obj.paramVectorType(*it), 
			      obj.paramVectorComment(*it), 
			      obj.paramVectorAsString(*it), 
			      obj.paramVectorAsDouble(*it) );
	break;
      case IParamSet::OTHER : 
	this->addParamVector( *it, 
			      obj.paramVectorType(*it), 
			      obj.paramVectorComment(*it), 
			      obj.paramVectorAsString(*it) );
	break;
      default :  
	throw 
	  ParamException("Unknown kind for paramVector with name : \""
				 + *it + "\" !");
	break;
      }
  }
  return *this;
};

//----------------------------------------------------------------------------

/// Destructor
ParamSet::~ParamSet() 
{ 
};

//----------------------------------------------------------------------------

/// Reset to initial state
void ParamSet::reset() 
{
  m_paramMap.clear();
  m_paramVectorMap.clear();
}


//----------------------------------------------------------------------------

/// Add a new double parameter.
void ParamSet::addParam( std::string name,
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
  aParam.kind = IParamSet::DOUBLE;
  m_paramMap[name] = aParam;
  return;
};

//----------------------------------------------------------------------------

/// Add a new integer parameter.
void ParamSet::addParam( std::string name,
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
  aParam.kind = IParamSet::INT;
  m_paramMap[name] = aParam;
  return;
};
  
//----------------------------------------------------------------------------

/// Add a new non-numerical parameter.
void ParamSet::addParam( std::string name,
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
  aParam.kind = IParamSet::OTHER;
  m_paramMap[name] = aParam;
  return;
};
  
//----------------------------------------------------------------------------

/// Add a new double parameter vector.
void ParamSet::addParamVector( std::string name,
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
  aParamVector.kind = IParamSet::DOUBLE;
  m_paramVectorMap[name] = aParamVector;
  return;
};

//----------------------------------------------------------------------------

/// Add a new integer parameter vector.
void ParamSet::addParamVector( std::string name,
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
  aParamVector.kind = IParamSet::INT;
  m_paramVectorMap[name] = aParamVector;
  return;
};

//----------------------------------------------------------------------------

/// Add a new non-numerical parameter vector.
void ParamSet::addParamVector( std::string name,
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
  aParamVector.kind = IParamSet::OTHER;
  m_paramVectorMap[name] = aParamVector;
  return;
};

//----------------------------------------------------------------------------

/// Get the kind of a parameter.
IParamSet::Kind ParamSet::paramKind( std::string name ) 
{
  if (m_paramMap.find(name) == m_paramMap.end()) {
    throw ParamException("No param with this name : \""
				 + name + "\" !");
  }
  return m_paramMap[name].kind;
};
  
//----------------------------------------------------------------------------

/// Get the type of a parameter.
std::string ParamSet::paramType( std::string name ) 
{
  if (m_paramMap.find(name) == m_paramMap.end()) {
    throw ParamException("No param with this name : \""
				 + name + "\" !");
  }
  return m_paramMap[name].type;
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter.
std::string ParamSet::paramComment( std::string name ) 
{
  if (m_paramMap.find(name) == m_paramMap.end()) {
    throw ParamException("No param with this name : \""
				 + name + "\" !");
  }
  return m_paramMap[name].comment;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a string.
std::string ParamSet::paramAsString( std::string name ) 
{
  if (m_paramMap.find(name) == m_paramMap.end()) {
    throw ParamException("No param with this name : \""
				 + name + "\" !");
  }
  return m_paramMap[name].value;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a int.
int ParamSet::paramAsInt( std::string name ) 
{
  if (m_paramMap.find(name) == m_paramMap.end()) {
    throw ParamException("No param with this name : \""
				 + name + "\" !");
  }
  if (m_paramMap[name].kind != IParamSet::INT) {
    throw ParamException("param " + name +
				 " does not have an integer value.");
  }
  return m_paramMap[name].i_value;
};

//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double ParamSet::paramAsDouble( std::string name ) 
{
  if (m_paramMap.find(name) == m_paramMap.end()) {
    throw ParamException("No param with this name : \""
				 + name + "\" !");
  }
  if (m_paramMap[name].kind != IParamSet::DOUBLE &&
      m_paramMap[name].kind != IParamSet::INT ) {
    throw ParamException("param " + name +
				 " does not have a numerical value.");
  }
  return m_paramMap[name].d_value;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double ParamSet::param( std::string name ) 
{
  return paramAsDouble (name);
}  
  
//----------------------------------------------------------------------------

/// Get the kind of a parameter vector.
IParamSet::Kind ParamSet::paramVectorKind( std::string name ) 
{
  if (m_paramVectorMap.find(name) == m_paramVectorMap.end()) {
    throw ParamException("No paramVector with this name : \""
				 + name + "\" !");
  }
  return m_paramVectorMap[name].kind;
};
  
//----------------------------------------------------------------------------

/// Get the type of a parameter vector.
std::string ParamSet::paramVectorType( std::string name ) 
{
  if (m_paramVectorMap.find(name) == m_paramVectorMap.end()) {
    throw ParamException("No paramVector with this name : \""
				 + name + "\" !");
  }
  return m_paramVectorMap[name].type;
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter vector.
std::string ParamSet::paramVectorComment( std::string name ) 
{
  if (m_paramVectorMap.find(name) == m_paramVectorMap.end()) {
    throw ParamException("No paramVector with this name : \""
				 + name + "\" !");
  }
  return m_paramVectorMap[name].comment;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a string vector.
std::vector<std::string> 
ParamSet::paramVectorAsString( std::string name ) 
{
  if (m_paramVectorMap.find(name) == m_paramVectorMap.end()) {
    throw ParamException("No paramVector with this name : \""
				 + name + "\" !");
  }
  return m_paramVectorMap[name].value;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a int vector.
std::vector<int>
ParamSet::paramVectorAsInt( std::string name ) 
{
  if (m_paramVectorMap.find(name) == m_paramVectorMap.end()) {
    throw ParamException("No paramVector with this name : \""
				 + name + "\" !");
  }
  if (m_paramVectorMap[name].kind != IParamSet::INT) {
    throw ParamException("paramVector " + name +
				 " does not have integer values.");
  }
  return m_paramVectorMap[name].i_value;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double>
ParamSet::paramVectorAsDouble( std::string name ) 
{
  if (m_paramVectorMap.find(name) == m_paramVectorMap.end()) {
    throw ParamException("No paramVector with this name : \""
				 + name + "\" !");
  }
  if (m_paramMap[name].kind != IParamSet::DOUBLE &&
      m_paramMap[name].kind != IParamSet::INT ) {
    throw ParamException("paramVector " + name +
				 " does not have numerical values.");
  }
  return m_paramVectorMap[name].d_value;
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double> ParamSet::paramVector( std::string name ) 
{
  return paramVectorAsDouble (name);
};

//----------------------------------------------------------------------------

/// Get the list of existing parameters.
std::vector<std::string> ParamSet::params() 
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
std::vector<std::string> ParamSet::paramVectors() 
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
void ParamSet::addUserParameter( std::string name,
					 std::string type,
					 std::string comment,
					 std::string value,
					 double d_value ) 
{
  addParam( name, type, comment, value, d_value );
  return;
};

//----------------------------------------------------------------------------

/// Add a new integer parameter.
void ParamSet::addUserParameter( std::string name,
					 std::string type,
					 std::string comment,
					 std::string value,
					 double d_value,
					 int i_value ) 
{
  addParam( name, type, comment, value, d_value, i_value );
  return;
};
  
//----------------------------------------------------------------------------

/// Add a new non-numerical parameter.
void ParamSet::addUserParameter( std::string name,
					 std::string type,
					 std::string comment,
					 std::string value ) 
{
  addParam( name, type, comment, value );
  return;
};
  
//----------------------------------------------------------------------------

/// Add a new double parameter vector.
void ParamSet::addUserParameterVector( std::string name,
					       std::string type,
					       std::string comment,
					       std::vector<std::string> value,
					       std::vector<double> d_value ) 
{
  addParamVector( name, type, comment, value, d_value );
  return;
};

//----------------------------------------------------------------------------

/// Add a new integer parameter vector.
void ParamSet::addUserParameterVector( std::string name,
					       std::string type,
					       std::string comment,
					       std::vector<std::string> value,
					       std::vector<double> d_value,
					       std::vector<int> i_value ) 
{
  addParamVector( name, type, comment, value, d_value, i_value );
  return;
};

//----------------------------------------------------------------------------

/// Add a new non-numerical parameter vector.
void ParamSet::addUserParameterVector( std::string name,
					       std::string type,
					       std::string comment,
					       std::vector<std::string> value )
{
  addParamVector( name, type, comment, value );
  return;
};

//----------------------------------------------------------------------------

/// Get the type of a parameter.
std::string ParamSet::userParameterType( std::string name ) 
{
  return paramType( name );
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter.
std::string ParamSet::userParameterComment( std::string name ) 
{
  return paramComment( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a string.
std::string ParamSet::userParameterAsString( std::string name ) 
{
  return paramAsString( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a int.
int ParamSet::userParameterAsInt( std::string name ) 
{
  return paramAsInt( name );
};

//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double ParamSet::userParameterAsDouble( std::string name ) 
{
  return paramAsDouble( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double ParamSet::userParameter( std::string name ) 
{
  return paramAsDouble (name);
};  
  
//----------------------------------------------------------------------------

/// Get the type of a parameter vector.
std::string ParamSet::userParameterVectorType( std::string name ) 
{
  return paramVectorType( name );
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter vector.
std::string ParamSet::userParameterVectorComment( std::string name ) 
{
  return paramVectorComment( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a string vector.
std::vector<std::string> 
ParamSet::userParameterVectorAsString( std::string name ) 
{
  return paramVectorAsString( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a int vector.
std::vector<int>
ParamSet::userParameterVectorAsInt( std::string name ) 
{
  return paramVectorAsInt( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double>
ParamSet::userParameterVectorAsDouble( std::string name ) 
{
  return paramVectorAsDouble( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double> ParamSet::userParameterVector( std::string name ) 
{
  return paramVector( name );
};

//----------------------------------------------------------------------------

/// Get the list of existing parameters.
std::vector<std::string> ParamSet::userParameters() 
{
  return params();
};
   
//----------------------------------------------------------------------------

/// Get the list of existing parameter vectors.
std::vector<std::string> ParamSet::userParameterVectors() 
{
  return paramVectors();
};

//----------------------------------------------------------------------------

/// Print the user parameters on a string
std::string ParamSet::printParams() {
  std::string os;
  std::vector<std::string>::iterator it;
  std::vector<std::string> pars = ((ParamSet*)this)->params();
  for( it = pars.begin(); pars.end() != it; ++it ) {
    if( pars.begin() != it ) os += "\n";
    os += "\"" + *it + "\" = "; 
    os += ((ParamSet*)this)->paramAsString(*it);
  }
  return os;
};

//----------------------------------------------------------------------------

/// Print the user parameter vectors on a string
std::string ParamSet::printParamVectors() {
  std::string os;
  std::vector<std::string>::iterator it;
  std::vector<std::string> parVecs = ((ParamSet*)this)->paramVectors();
  for( it = parVecs.begin(); parVecs.end() != it; ++it ) {
    if( parVecs.begin() != it ) os += "\n";
    os += "\"" + *it + "\" ="; 
    std::vector<std::string> parVec = 
      ((ParamSet*)this)->paramVectorAsString(*it);
    std::vector<std::string>::iterator it2;
    for( it2 = parVec.begin(); parVec.end() != it2; ++it2) {
      os += " " + *it2;
    }
  }
  return os;
};

//----------------------------------------------------------------------------

//$Id: Condition.cpp,v 1.3 2001-12-14 17:57:35 andreav Exp $
#include <string> 

#include "DetDesc/Condition.h"
#include "DetDesc/UserParameterSet.h"

#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/StatusCode.h"

//---------------------------------------------------------------------------

/// Default constructor
Condition::Condition()
  : ValidDataObject()
{
  m_parameterSet = new UserParameterSet();
}; 

//---------------------------------------------------------------------------

/// Constructor
Condition::Condition( const ITime& since, 
		      const ITime& till  )
  : ValidDataObject( since, till )
{
  m_parameterSet = new UserParameterSet();
}; 

//---------------------------------------------------------------------------

/// Copy constructor
Condition::Condition( Condition& obj )
  : ValidDataObject( (ValidDataObject&)obj )
{
  m_parameterSet = new UserParameterSet( (IUserParameterSet&)obj );
}; 

//---------------------------------------------------------------------------

/// Update using another instance of this class: deep copy all 
/// contents, except for the properties of a generic DataObject
void Condition::update ( Condition& obj )
{
  ValidDataObject::update ( obj );
  *(this->m_parameterSet) = (IUserParameterSet&)obj ;
}; 

//---------------------------------------------------------------------------

/// Destructor
Condition::~Condition() 
{
  delete m_parameterSet;
};

//----------------------------------------------------------------------------

StatusCode Condition::initialize() {
  // this is a default implementation that does nothing.
  // it is up to the user to override this in a child of Condition
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

/// Add a new double parameter.
void Condition::addUserParameter( std::string name,
				  std::string type,
				  std::string comment,
				  std::string value,
				  double d_value ) 
{
  m_parameterSet->addUserParameter( name, type, comment, value, d_value );
  return;
};

//----------------------------------------------------------------------------

/// Add a new integer parameter.
void Condition::addUserParameter( std::string name,
				  std::string type,
				  std::string comment,
				  std::string value,
				  double d_value,
				  int i_value ) 
{
  m_parameterSet->
    addUserParameter( name, type, comment, value, d_value, i_value );
  return;
};
  
//----------------------------------------------------------------------------

/// Add a new non-numerical parameter.
void Condition::addUserParameter( std::string name,
				  std::string type,
				  std::string comment,
				  std::string value ) 
{
  m_parameterSet->addUserParameter( name, type, comment, value );
  return;
};
  
//----------------------------------------------------------------------------

/// Add a new double parameter vector.
void Condition::addUserParameterVector( std::string name,
					std::string type,
					std::string comment,
					std::vector<std::string> value,
					std::vector<double> d_value ) 
{
  m_parameterSet->
    addUserParameterVector( name, type, comment, value, d_value );
  return;
};

//----------------------------------------------------------------------------

/// Add a new integer parameter vector.
void Condition::addUserParameterVector( std::string name,
					std::string type,
					std::string comment,
					std::vector<std::string> value,
					std::vector<double> d_value,
					std::vector<int> i_value ) 
{
  m_parameterSet->
    addUserParameterVector( name, type, comment, value, d_value, i_value );
  return;
};

//----------------------------------------------------------------------------

/// Add a new non-numerical parameter vector.
void Condition::addUserParameterVector( std::string name,
					std::string type,
					std::string comment,
					std::vector<std::string> value )
{
  m_parameterSet->addUserParameterVector( name, type, comment, value );
  return;
};

//----------------------------------------------------------------------------

/// Get the type of a parameter.
std::string Condition::userParameterType( std::string name ) 
{
  return m_parameterSet->userParameterType( name );
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter.
std::string Condition::userParameterComment( std::string name ) 
{
  return m_parameterSet->userParameterComment( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a string.
std::string Condition::userParameterAsString( std::string name ) 
{
  return m_parameterSet->userParameterAsString( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a int.
int Condition::userParameterAsInt( std::string name ) 
{
  return m_parameterSet->userParameterAsInt( name );
};

//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double Condition::userParameterAsDouble( std::string name ) 
{
  return m_parameterSet->userParameterAsDouble( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double Condition::userParameter( std::string name ) 
{
  return m_parameterSet->userParameter( name );
};  
  
//----------------------------------------------------------------------------

/// Get the type of a parameter vector.
std::string Condition::userParameterVectorType( std::string name ) 
{
  return m_parameterSet->userParameterVectorType( name );
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter vector.
std::string Condition::userParameterVectorComment( std::string name ) 
{
  return m_parameterSet->userParameterVectorComment( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a string vector.
std::vector<std::string> 
Condition::userParameterVectorAsString( std::string name ) 
{
  return m_parameterSet->userParameterVectorAsString( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a int vector.
std::vector<int>
Condition::userParameterVectorAsInt( std::string name ) 
{
  return m_parameterSet->userParameterVectorAsInt( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double>
Condition::userParameterVectorAsDouble( std::string name ) 
{
  return m_parameterSet->userParameterVectorAsDouble( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double> Condition::userParameterVector( std::string name ) 
{
  return m_parameterSet->userParameterVector( name );
};

//----------------------------------------------------------------------------

/// Get the list of existing parameters.
std::vector<std::string> Condition::userParameters() 
{
  return m_parameterSet->userParameters();
};
   
//----------------------------------------------------------------------------

/// Get the list of existing parameter vectors.
std::vector<std::string> Condition::userParameterVectors() 
{
  return m_parameterSet->userParameterVectors();
};

//----------------------------------------------------------------------------

/// Add a new double parameter.
void Condition::addParam( std::string name,
			  std::string type,
			  std::string comment,
			  std::string value,
			  double d_value ) 
{
  m_parameterSet->addParam( name, type, comment, value, d_value );
  return;
};

//----------------------------------------------------------------------------

/// Add a new integer parameter.
void Condition::addParam( std::string name,
			  std::string type,
			  std::string comment,
			  std::string value,
			  double d_value,
			  int i_value ) 
{
  m_parameterSet->addParam( name, type, comment, value, d_value, i_value );
  return;
};
  
//----------------------------------------------------------------------------

/// Add a new non-numerical parameter.
void Condition::addParam( std::string name,
			  std::string type,
			  std::string comment,
			  std::string value ) 
{
  m_parameterSet->addParam( name, type, comment, value );
  return;
};
  
//----------------------------------------------------------------------------

/// Add a new double parameter vector.
void Condition::addParamVector( std::string name,
				std::string type,
				std::string comment,
				std::vector<std::string> value,
				std::vector<double> d_value ) 
{
  m_parameterSet->addParamVector( name, type, comment, value, d_value );
  return;
};

//----------------------------------------------------------------------------

/// Add a new integer parameter vector.
void Condition::addParamVector( std::string name,
				std::string type,
				std::string comment,
				std::vector<std::string> value,
				std::vector<double> d_value,
				std::vector<int> i_value ) 
{
  m_parameterSet->
    addParamVector( name, type, comment, value, d_value, i_value );
  return;
};

//----------------------------------------------------------------------------

/// Add a new non-numerical parameter vector.
void Condition::addParamVector( std::string name,
				std::string type,
				std::string comment,
				std::vector<std::string> value )
{
  m_parameterSet->addParamVector( name, type, comment, value );
  return;
};

//----------------------------------------------------------------------------

/// Get the kind of a parameter.
IUserParameterSet::Kind Condition::paramKind( std::string name ) 
{
  return m_parameterSet->paramKind( name );
};
  
//----------------------------------------------------------------------------

/// Get the type of a parameter.
std::string Condition::paramType( std::string name ) 
{
  return m_parameterSet->paramType( name );
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter.
std::string Condition::paramComment( std::string name ) 
{
  return m_parameterSet->paramComment( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a string.
std::string Condition::paramAsString( std::string name ) 
{
  return m_parameterSet->paramAsString( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a int.
int Condition::paramAsInt( std::string name ) 
{
  return m_parameterSet->paramAsInt( name );
};

//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double Condition::paramAsDouble( std::string name ) 
{
  return m_parameterSet->paramAsDouble( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter, as a double.
double Condition::param( std::string name ) 
{
  return m_parameterSet->param( name );
};  
  
//----------------------------------------------------------------------------

/// Get the kind of a parameter vector.
IUserParameterSet::Kind Condition::paramVectorKind( std::string name ) 
{
  return m_parameterSet->paramVectorKind( name );
};
  
//----------------------------------------------------------------------------

/// Get the type of a parameter vector.
std::string Condition::paramVectorType( std::string name ) 
{
  return m_parameterSet->paramVectorType( name );
};
  
//----------------------------------------------------------------------------

/// Get the comment of a parameter vector.
std::string Condition::paramVectorComment( std::string name ) 
{
  return m_parameterSet->paramVectorComment( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a string vector.
std::vector<std::string> 
Condition::paramVectorAsString( std::string name ) 
{
  return m_parameterSet->paramVectorAsString( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a int vector.
std::vector<int>
Condition::paramVectorAsInt( std::string name ) 
{
  return m_parameterSet->paramVectorAsInt( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double>
Condition::paramVectorAsDouble( std::string name ) 
{
  return m_parameterSet->paramVectorAsDouble( name );
};
  
//----------------------------------------------------------------------------

/// Get the value of a parameter vector, as a double vector.
std::vector<double> Condition::paramVector( std::string name ) 
{
  return m_parameterSet->paramVector( name );
};

//----------------------------------------------------------------------------

/// Get the list of existing parameters.
std::vector<std::string> Condition::params() 
{
  return m_parameterSet->params();
};
   
//----------------------------------------------------------------------------

/// Get the list of existing parameter vectors.
std::vector<std::string> Condition::paramVectors() 
{
  return m_parameterSet->paramVectors();
};

//----------------------------------------------------------------------------

/// Print the user parameters on a string
std::string Condition::printParams() {
  return m_parameterSet->printParams();
};

//----------------------------------------------------------------------------

/// Print the user parameter vectors on a string
std::string Condition::printParamVectors() {
  return m_parameterSet->printParamVectors();
};

//----------------------------------------------------------------------------

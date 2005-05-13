//$Id: Condition.cpp,v 1.6 2005-05-13 08:26:01 marcocle Exp $
#include <string> 

#include "DetDesc/Condition.h"

#include "GaudiKernel/TimePoint.h" 
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IRegistry.h"

//---------------------------------------------------------------------------

/// Default constructor
Condition::Condition() : ParamValidDataObject() {}; 

//---------------------------------------------------------------------------

/// Constructor
Condition::Condition (const ITime& since, const ITime& till)
  : ParamValidDataObject (since, till) {}; 

//---------------------------------------------------------------------------

/// Copy constructor
Condition::Condition (Condition& obj)
  : ICondition(), ParamValidDataObject ((ParamValidDataObject&)obj) {}; 

//---------------------------------------------------------------------------

/// Update using another instance of this class: deep copy all 
/// contents, except for the properties of a generic DataObject
void Condition::update (Condition& obj)
{
  ParamValidDataObject::update ( obj );
}; 

//=========================================================================
//  Prepare an XML string representing the condition
//=========================================================================
std::string Condition::toXml(const std::string &name) {
  std::ostringstream xml;
  // XML header
  xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DDDB SYSTEM \"structure.dtd\">";
  // DDDB open
  xml << "<DDDB>";
  // condition open
  xml << "<condition name=\"";
  if (name.empty()) {
    if (registry()){
      xml << registry()->name();
    } else {
      xml << "Condition";
    }
  } else {
    xml << name;
  }
  xml <<"\">";

  std::vector<std::string> pars;
  std::vector<std::string>::const_iterator i;
  // loop over parameters
  pars = params();
  for ( i = pars.begin(); i != pars.end(); ++i ){
  	xml << "<param name=\"" << *i << "\" type=\"";
    switch (paramKind(*i)){
    case IParamSet::DOUBLE : xml << "double"; break;
    case IParamSet::INT    : xml << "int"; break;
    default                : xml << "other";
    }
    xml << "\">" << paramAsString(*i) << "</param>";
  }
  // loop over vector parameters
  pars = paramVectors();
  for ( i = pars.begin(); i != pars.end(); ++i ){
  	xml << "<paramVector name=\"" << *i << "\" type=\"";
    switch (paramVectorKind(*i)){
    case IParamSet::DOUBLE : xml << "double"; break;
    case IParamSet::INT    : xml << "int"; break;
    default                : xml << "other";
    }
    xml << "\">";
    std::vector<std::string> p_val = paramVectorAsString(*i);
    for ( std::vector<std::string>::const_iterator s = p_val.begin(); s != p_val.end() ; ++s ){
      xml << " " << *s;
    }
    xml << "</paramVector>";
  }
  
  // condition close
  xml << "</condition>";
  // DDDB close
  xml << "</DDDB>\n";
  
  return xml.str();
}
//---------------------------------------------------------------------------

/// Destructor
Condition::~Condition() {};

//----------------------------------------------------------------------------

unsigned long Condition::addRef  () {
  return ParamValidDataObject::addRef();
}

unsigned long Condition::release () {
  return ParamValidDataObject::release();
}

/// queryInterface
StatusCode Condition::queryInterface( const InterfaceID& ID , void** ppI )
{
  if ( 0 == ppI ) { return StatusCode::FAILURE; }
  *ppI = 0 ;
  if ( ICondition::interfaceID() == ID ) 
    { *ppI = static_cast<ICondition*> ( this ) ; } 
  else if ( IInterface::interfaceID() == ID ) 
    { *ppI = static_cast<IInterface*> ( this ) ; } 
  else                                                  
    { return StatusCode::FAILURE ; }
  /// add the reference 
  addRef();
  ///
  return StatusCode::SUCCESS;
};

//----------------------------------------------------------------------------

/// initialize
StatusCode Condition::initialize() {
  // this is a default implementation that does nothing.
  // it is up to the user to override this in a child of Condition
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

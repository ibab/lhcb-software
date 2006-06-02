//$Id: Condition.cpp,v 1.11 2006-06-02 06:59:01 cattanem Exp $
#include <string> 

#include "DetDesc/Condition.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IRegistry.h"

//---------------------------------------------------------------------------

/// Default constructor
Condition::Condition() : ParamValidDataObject() {}; 

//---------------------------------------------------------------------------

/// Copy constructor
Condition::Condition (Condition& obj)
  : IValidity(), ParamValidDataObject ((ParamValidDataObject&)obj) {}; 

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
std::string Condition::toXml(const std::string &name) const{
  std::ostringstream xml;
  // XML header
  xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DDDB SYSTEM \"structure.dtd\">";
  // DDDB open
  xml << "<DDDB>";
  // condition open
  xml << "<condition classID=\"" << this->clID() << "\" name=\"";
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
  pars = paramNames();
  for ( i = pars.begin(); i != pars.end(); ++i ){
  	xml << "<param";
  	bool is_vect_param = isVector(*i);
  	if ( is_vect_param ) xml << "Vector";
    xml << " name=\"" << *i << "\" type=\"";
    const std::type_info &curr_type = type(*i);
    if ( curr_type == typeid(double) || curr_type == typeid(std::vector<double>) ) {
      xml << "double";
    } else if ( curr_type == typeid(int) || curr_type == typeid(std::vector<int>) ) {
      xml << "int";
    } else { // OTHER
      xml << "other";
    }
    std::string comm = comment(*i);
    if (!comm.empty()) xml << "\" comment=\"" << comm;

    xml << "\">" << paramToString(*i) << "</param";
  	if ( is_vect_param ) xml << "Vector";
    xml << ">";
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

/// initialize
StatusCode Condition::initialize() {
  // this is a default implementation that does nothing.
  // it is up to the user to override this in a child of Condition
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------

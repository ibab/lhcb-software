//$Id: ParamValidDataObject.cpp,v 1.12 2007-11-09 17:10:07 marcocle Exp $
#include <string>

#include "GaudiKernel/System.h"
#include "DetDesc/ParamValidDataObject.h"

//---------------------------------------------------------------------------

/// Default constructor
ParamValidDataObject::ParamValidDataObject(): ValidDataObject() {}

//---------------------------------------------------------------------------

/// Copy constructor
ParamValidDataObject::ParamValidDataObject (const ParamValidDataObject& obj)
  :IValidity(), ValidDataObject (obj),
    m_paramList (obj.m_paramList),
    m_comments (obj.m_comments){}

//---------------------------------------------------------------------------

/// Destructor
ParamValidDataObject::~ParamValidDataObject() {};

//----------------------------------------------------------------------------

std::ostream& ParamValidDataObject::fillStream( std::ostream& s ) const {
  ValidDataObject::fillStream(s);
  s << std::endl << printParams();
  return s;
}

//---------------------------------------------------------------------------

void ParamValidDataObject::reset() 
{
  m_paramList.clear();
  m_comments.clear();
}

//----------------------------------------------------------------------------

void ParamValidDataObject::update( ValidDataObject& obj ){
  // first check the class
  ParamValidDataObject *pvdo = dynamic_cast<ParamValidDataObject *>(&obj);
  if (0 == pvdo){
    throw GaudiException("Trying to do a deep copy between different classes",
                         "ParamValidDataObject",StatusCode::FAILURE);
  }
  // call the 
  ValidDataObject::update(obj);
  
  // default merge
  m_paramList += pvdo->m_paramList;
  CommentMap::const_iterator i;
  for ( i = pvdo->m_comments.begin(); i != pvdo->m_comments.end(); ++i ){
  	// TODO: optimize!
    m_comments[i->first] = i->second;
  }
}

//----------------------------------------------------------------------------

bool ParamValidDataObject::exists(const std::string &name) const {
  return ( 0 != m_paramList.count(name) );
}

//----------------------------------------------------------------------------
/// TypeId of the parameter
const std::type_info &ParamValidDataObject::type(const std::string &name) const {
  ParamList::const_iterator i = m_paramList.find(name);
  if ( i == m_paramList.end() ) throw ParamException(name);
  return i->second->type();
}

//----------------------------------------------------------------------------
/// Check if the parameter is a vector.
bool ParamValidDataObject::isVector(const std::string &name) const {
  const std::type_info &t = type(name);
  return t == typeid(std::vector<int>) ||
         t == typeid(std::vector<double>) ||
         t == typeid(std::vector<std::string>);
}
//----------------------------------------------------------------------------
/// Get the comment of a parameter.
std::string ParamValidDataObject::comment (const std::string &name) const {
  if ( m_paramList.count(name) == 0) throw ParamException(name);

  CommentMap::const_iterator i = m_comments.find(name);
  if (i != m_comments.end()) return i->second;
  
  return std::string();
}

//----------------------------------------------------------------------------
/// Set the comment of a parameter.
void ParamValidDataObject::setComment (const std::string &name, const char *comm){
  if ( m_paramList.count(name) == 0) throw ParamException(name);

  CommentMap::iterator i = m_comments.find(name);
  if (i != m_comments.end()) {
    // set the comment only if is not empty (or a null pointer)
    if (comm != NULL && std::strlen(comm) != 0) {
      i->second = comm;
    } else {
      // if the comment is an empty string or a null pointer, remove the comment
      m_comments.erase(i);
    }
  } else {
    // do not add the comment if empty (ora a null pointer)
    if (comm != NULL && std::strlen(comm) != 0)
      m_comments.insert(make_pair(name,std::string(comm)));
  }
}


//----------------------------------------------------------------------------
/// Get the value of a parameter, as a string.
const std::string &ParamValidDataObject::paramAsString (const std::string &name) const {
  return param<std::string>(name);
}
  
//----------------------------------------------------------------------------
/// Get the value of a parameter, as a string (non const version).
std::string &ParamValidDataObject::paramAsString (const std::string &name){
  return param<std::string>(name);
}
  
//----------------------------------------------------------------------------
/// Get the value of a parameter, as an int.
int ParamValidDataObject::paramAsInt (const std::string &name) const {
  return param<int>(name);
}
  
//----------------------------------------------------------------------------
/// Get the value of a parameter, as a double.
double ParamValidDataObject::paramAsDouble (const std::string &name) const {
  try {
    return param<double>(name);
  } catch (std::bad_cast) {
    if (type(name) == typeid(int)){
      return param<int>(name);
  	} else throw;
  }
}
  
//----------------------------------------------------------------------------
/// Get the value of a parameter, as a string.
const std::vector<std::string> &ParamValidDataObject::paramAsStringVect (const std::string &name) const {
  return param<std::vector<std::string> >(name);
}
  
//----------------------------------------------------------------------------
/// Get the value of a parameter, as a string.
std::vector<std::string> &ParamValidDataObject::paramAsStringVect (const std::string &name){
  return param<std::vector<std::string> >(name);
}
  
//----------------------------------------------------------------------------
/// Get the value of a parameter, as an int.
const std::vector<int> &ParamValidDataObject::paramAsIntVect (const std::string &name) const {
  return param<std::vector<int> >(name);
}
  
//----------------------------------------------------------------------------
/// Get the value of a parameter, as a double.
const std::vector<double> &ParamValidDataObject::paramAsDoubleVect (const std::string &name) const {
  return param<std::vector<double> >(name);
}

//----------------------------------------------------------------------------
/// Get the value of a parameter, as an int.
std::vector<int> &ParamValidDataObject::paramAsIntVect (const std::string &name) {
  return param<std::vector<int> >(name);
}
  
//----------------------------------------------------------------------------
/// Get the value of a parameter, as a double.
std::vector<double> &ParamValidDataObject::paramAsDoubleVect (const std::string &name) {
  return param<std::vector<double> >(name);
}

//----------------------------------------------------------------------------
/// Get the list of existing parameters.
std::vector<std::string> ParamValidDataObject::paramNames() const {
  return m_paramList.getKeys();
}
 
//----------------------------------------------------------------------------
/// Print the user parameters on a string
std::string ParamValidDataObject::printParams() const {
  std::ostringstream os;
  ParamList::const_iterator i;
  for ( i = m_paramList.begin(); i != m_paramList.end() ; ++i ){    
    os << "(" << System::typeinfoName(i->second->type()) << ") " << i->first ;
    
    CommentMap::const_iterator c = m_comments.find(i->first);
    if ( c != m_comments.end() ) os << " (" << c->second << ")";
    os << " = " << i->second->toStr() << "\n";
  }
  return os.str();
}

//----------------------------------------------------------------------------
/// Convert a parameter to a string (for xml representation).
std::string ParamValidDataObject::paramToString(const std::string &name) const {
  ParamList::const_iterator i = m_paramList.find(name);
  if ( i == m_paramList.end() ) throw ParamException(name);
  std::ostringstream os;
  os << i->second->toStr();
  return os.str();
}


/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/XmlCnvAttrList.h,v 1.1.1.1 2000-11-23 13:44:44 ranjard Exp $
#ifndef DETDESC_XMLCNVSVC_XMLCNVATTRLIST_H
#define DETDESC_XMLCNVSVC_XMLCNVATTRLIST_H

/// Include files
#include <string>
#include <vector>

/// Class simulating behavior of real SAX XML attribute list,
/// but using 8 bit ASCII encoding
class XmlCnvAttributeList
{
public:

  std::string s_empty;

  /// XML ASCII attribute
  struct _attrItem  {
    std::string m_name;
    std::string m_value;
    std::string m_type;
  };
  
  /// Constructor
  XmlCnvAttributeList()                                                     {
    s_empty = "";
  }

  /// Desctructor
  ~XmlCnvAttributeList()                                                    {
  }

  /// Add a whole attribute item into the list
  void add( const XmlCnvAttributeList::_attrItem& item )                    {
    m_list.push_back( item );
  }
  
  /// Add an attribute item into the list specified by its parts
  void add( const char* const name, const char* const value, const char* const type ){
    struct _attrItem att;
    att.m_name  = name;
    att.m_value = value;
    att.m_type  = type;
    m_list.push_back( att );
  }
  
  /// Get attribute parameters by the given index in the list
  const std::string& getName( unsigned int index )                          {
    return m_list[index].m_name;
  }
  
  /// Get attribute parameters by the given index in the list
  const std::string& getValue( unsigned int index )                         {
    return m_list[index].m_value;
  }
  
  /// Get attribute parameters by the given index in the list
  const std::string& getType( unsigned int index )                          {
    return m_list[index].m_type;
  }

  /// Get attribute parameters by the attribute's name
  const std::string& getValue( const std::string& name )                    {
    std::vector<struct _attrItem>::iterator aiter;
    bool success = false;

    for( aiter = m_list.begin(); aiter != m_list.end(); aiter++ )           {
      if( (*aiter).m_name == name)                                          {
        success = true;
        break;
      }
    }
    return( success ? (*aiter).m_value : s_empty );
  }
  
  /// Get attribute parameters by the attribute's name
  const std::string& getValue( const char* name )                           {
    std::vector<struct _attrItem>::iterator aiter;
    bool success = false;

    for( aiter = m_list.begin(); aiter != m_list.end(); aiter++ )           {
      if( (*aiter).m_name == name)                                          {
        success = true;
        break;
      }
    }
    return( success ? (*aiter).m_value : s_empty );
  }

  /// Get attribute parameters by the attribute's name
  const std::string& getType( const std::string& name )                     {
    std::vector<struct _attrItem>::iterator aiter;
    bool success = false;

    for( aiter = m_list.begin(); aiter != m_list.end(); aiter++ )           {
      if( (*aiter).m_name == name)                                          {
        success = true;
        break;
      }
    }
    return( success ? (*aiter).m_type : s_empty );
  }
  
  /// Get attribute parameters by the attribute's name
  const std::string& getType( const char* name )                            {
    std::vector<struct _attrItem>::iterator aiter;
    bool success = false;

    for( aiter = m_list.begin(); aiter != m_list.end(); aiter++ )           {
      if( (*aiter).m_name == name)                                          {
        success = true;
        break;
      }
    }
    return( success ? (*aiter).m_type : s_empty );
  }

  /// Get the number of attributes in the list
  unsigned int getLength()                                                  {
    return m_list.size();
  }
  
  private:
    
  /// Attribute List Container
  std::vector<struct _attrItem> m_list;
    
};

#endif // DETDESC_XMLCNVSVC_XMLCNVATTRLIST_H


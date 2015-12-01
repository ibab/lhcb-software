#ifndef GAUDIUPI_PROPERTYEDITOR_H
#define GAUDIUPI_PROPERTYEDITOR_H 1
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiUPI/GaudiUPI/PropertyEditor.h,v 1.1 2006-10-25 09:30:45 frankb Exp $

// C++ include files
#include <map>
#include "GaudiUPI/SubMenu.h"

// Forward declarations
class Property;
class DialogItem;

/*
 *  Gaudi namespace
 */
namespace Gaudi  {

  /** @class PropertyEditor PropertyEditor.h GaudiUPI/PropertyEditor.h
    *
    * Dialog class to show the properties of an object 
    * with an IProperty interface
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class PropertyEditor : public SubMenu {
  protected:
    enum { CMD_PARAMS = CMD_SHOW + 10,
           CMD_LAST_PARAM = CMD_CLOSE-10,
           CMD_SET_PARAM  = CMD_CLOSE-2
    };
    typedef std::map<int,std::string> Lines;
    /// name of propties holder
    std::string    m_name;
    /// Pointer to property interface of object
    Property*      m_property;
    DialogItem*    m_return;
    DialogItem*    m_command;
    bool m_isVector;
    bool m_isString;
    /// Map command items to properties
    Lines          m_lines;
    Lines          m_defaults;
  public:
    /// Initializing constructor
    PropertyEditor(const std::string& nam, Property* p, CPP::Interactor* parent);
    /// Default destructor
    virtual ~PropertyEditor();
    void updateParams();
    void retrieveParams();
    void deleteItems();
    void insertItems();
    void setCurrent();
    void setProperties();
    /// Interactor overload: handle menu interupts
    void handle (const CPP::Event& event);
  };
}      // Enad namespace Gaudi
#endif // GAUDIUPI_PROPERTYEDITOR_H

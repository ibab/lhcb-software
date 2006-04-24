#ifndef GAUDIUPI_PROPERTYMENU_H
#define GAUDIUPI_PROPERTYMENU_H 1
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiUPI/Gaudi/PropertyMenu.h,v 1.1 2006-04-24 13:45:55 frankb Exp $

// C++ include files
#include <map>
#include "Gaudi/SubMenu.h"

// Forward declarations
class IProperty;
class Property;

/*
 *  Gaudi namespace
 */
namespace Gaudi  {

  /** @class PropertyMenu PropertyMenu.h GaudiUPI/PropertyMenu.h
    *
    * Dialog class to show the properties of an object 
    * with an IProperty interface
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class PropertyMenu : public SubMenu {
  protected:
    typedef std::map<int,const Property*> Lines;
    /// name of propties holder
    std::string    m_name;
    /// Pointer to property interface of object
    IProperty*     m_property;
    /// Map command items to properties
    Lines          m_lines;
  public:
    /// Initializing constructor
    PropertyMenu(const std::string& nam, IInterface* p, Interactor* parent);
    /// Default destructor
    virtual ~PropertyMenu();
    /// Interactor overload: handle menu interupts
    void handle (const Event& event);
  };
}      // Enad namespace Gaudi
#endif // GAUDIUPI_PROPERTYMENU_H

#ifndef GAUDIUPI_PROPERTYMENU_H
#define GAUDIUPI_PROPERTYMENU_H 1
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiUPI/GaudiUPI/PropertyMenu.h,v 1.1 2006-10-25 09:30:45 frankb Exp $

// C++ include files
#include <map>
#include "GaudiUPI/SubMenu.h"

// Forward declarations
class IProperty;
class Property;

/*
 *  Gaudi namespace
 */
namespace Gaudi  {

  /// Dialog class to show the properties of an object with an IProperty interface
  /** 
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
    PropertyMenu(const std::string& nam, IInterface* p, CPP::Interactor* parent);
    /// Default destructor
    virtual ~PropertyMenu();
    /// Interactor overload: handle menu interupts
    void handle (const CPP::Event& event);
  };
}      // Enad namespace Gaudi
#endif // GAUDIUPI_PROPERTYMENU_H

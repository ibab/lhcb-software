#ifndef GAUDIUPI_ALGORITHMMENU_H
#define GAUDIUPI_ALGORITHMMENU_H
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/GaudiUPI/Gaudi/AlgorithmMenu.h,v 1.1 2006-04-24 13:45:55 frankb Exp $

// C++ include files
#include <map>
#include "Gaudi/SubMenu.h"

// Forward declarations
class ISvcLocator;
class IAlgorithm;

/*
 *  Gaudi namespace
 */
namespace Gaudi  {

  /** @class AlgorithmMenu AlgorithmMenu.h GaudiUPI/AlgorithmMenu.h
    *
    * Dialog class to show /list all algorithms present in an existing
    * Gaudi application
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class AlgorithmMenu : public SubMenu  {
  protected:
    typedef std::map<int,IAlgorithm*> Lines;
    /// Pointer to service locator object
    ISvcLocator*   m_svcLoc;
    /// Map command items to Algorithms
    Lines          m_lines;

  public:
    /// Initializing constructor
    AlgorithmMenu(ISvcLocator* svc, Interactor* par);
    /// Default destructor
    virtual ~AlgorithmMenu();
    /// Interactor overload: handle menu interupts
    void handle (const Event& event);
  };
}      // End namespace Gaudi
#endif // GAUDIUPI_SERVICEMENU_H

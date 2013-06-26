// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/Printer.h,v 1.2 2007-04-26 18:22:26 frankb Exp $
//  ====================================================================
//  Printer.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_PRINTER_H
#define ONLINE_PVSS_PRINTER_H

// C++ include files
#include <memory>
#include <string>

/*
 *   PVSS namespace declaration
 */
namespace PVSS {

  // Forward declarations
  class CfgType;
  class CfgDetail;
  class CfgAttribute;
  class CfgManager;
  class ControlsManager;
  class DeviceManager;
  class DevTypeManager;
  class DevType;
  class DevTypeElement;
  class NamedIdentified;

  /** @class Printer   Printer.h  PVSS/Printer.h
    *
    *  Print PVSS items.
    *
    *   @author  M.Frank
    *   @version 1.0
    */
  class Printer {
    /// Friend class declaration
    friend class std::auto_ptr<Printer>;
  protected:
    /// Nobody except implementing class is allowed to delete
    virtual ~Printer() {}
  public:
    /// Printing named identified type printer
    virtual void print(const NamedIdentified& obj) = 0;

    /// Configuration attribute printer
    virtual void print(const CfgAttribute& obj) = 0;
    /// Configuration detail printer
    virtual void print(const CfgDetail& obj) = 0;
    /// Configuration type printer
    virtual void print(const CfgType& obj) = 0;

    /// Device type printer
    virtual void print(const DevType& obj) = 0;
    /// Device type element printer
    virtual void print(const DevTypeElement& obj) = 0;

    /// Device type manager printer
    virtual void print(const DevTypeManager& obj) = 0;
    /// Configuration manager printer
    virtual void print(const CfgManager& obj) = 0;
    /// Controls manager type printer
    virtual void print(const ControlsManager& obj) = 0;

    /// Informational printout to logger window
    virtual void info(int type, const std::string& message) = 0;
    /// Warning printout to logger window
    virtual void warning(int type, const std::string& message) = 0;
    /// Sever error printout to logger window
    virtual void error(int type, const std::string& message) = 0;
    /// Fatal printout to logger window. kill the program instance!
    virtual void fatal(int type, const std::string& message) = 0;
  };
  /// Create standard ASCII print formatter
  std::auto_ptr<Printer> createAsciiPrinter(std::ostream& os);
  /// Create standard PVSS logger print formatter
  std::auto_ptr<Printer> createLogger();
}      // End namespace PVSS
#endif // ONLINE_PVSS_PRINTER_H

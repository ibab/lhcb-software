// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/PythonCallback.h,v 1.6 2007-11-07 15:15:07 frankm Exp $
//  ====================================================================
//  PythonCallback.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_PYTHONCALLBACK_H
#define ONLINE_PVSS_PYTHONCALLBACK_H

// Framework include files
#if defined(__GCCXML) && !defined(__REAL_COMP)
struct _object;
typedef _object PyObject;
#else
#include "Python.h"
#endif
#include "PVSS/DeviceListener.h"

/*
 * PVSS namespace
 */
namespace PVSS {

  // Forward declarations
  class PythonCall;
  class ControlsManager;

  class PyInteractor : public Interactor {
    /// Dressed python callback
    PythonCall* m_call;
    /// Sensor callback data
    const Event* m_currentEvent;
  public:
    /// Default destructor
    PyInteractor(PyObject* self);
    /// Standard destructor
    virtual ~PyInteractor();
    /// Access Sensor callback  data
    const Event* event() {  return m_currentEvent; }
    /// Sensor callback
    virtual void handle(const Event& ev);
    /// Overloaded Sensor callback: call to event() is valid!
    virtual void handleEvent();
  };

  /** @class PyDeviceListener
    * 
    * @author  M.Frank
    * @version 1.0
    */
  class PyDeviceListener : public DeviceListener {
  protected:
    /// Dressed python callback
    PythonCall* m_call;
  public:
    /// Standard constructor: Argument self: instance to python object implementation
    PyDeviceListener(PyObject* self, ControlsManager* m);
    /// Default destructor
    virtual ~PyDeviceListener();
    /// Hack!
    void setSelf(PyObject* self);
    /// DIM overloaded callback calling python itself.
    virtual void handleDevices();
    /// DIM overloaded callback calling python itself.
    virtual void handleDevice();
    /// Interface callback to handle single invalid devices one by one
    virtual void handleInvalidDevice();
  };

  class PyCallbackTest  {
  protected:
    /// Dressed python callback
    PythonCall* m_call;
  public:
    /// Standard constructor: Argument self: instance to python object implementation
    PyCallbackTest(PyObject* self);
    /// Default destructor
    virtual ~PyCallbackTest();
    /// overloaded callback calling python itself.
    virtual void handle();
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_PYTHONCALLBACK_H

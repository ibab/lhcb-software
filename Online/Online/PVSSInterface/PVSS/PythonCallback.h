// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/PythonCallback.h,v 1.1 2007-03-12 10:07:24 frankb Exp $
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
#include "PVSS/DeviceListener.h"
#if defined(__GCCXML) && !defined(__REAL_COMP)
typedef struct _PyObject { void* bla; } PyObject;
#else
#include "Python.h"
#endif

/*
 * PVSS namespace
 */
namespace PVSS {

  // Forward declarations
  class PythonCall;
  class ControlsManager;

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
    /// DIM overloaded callback calling python itself.
    virtual void handleDevices();
    /// DIM overloaded callback calling python itself.
    virtual void handleDevice();
  };
}      // End namespace PVSS
#endif // ONLINE_PVSS_PYTHONCALLBACK_H

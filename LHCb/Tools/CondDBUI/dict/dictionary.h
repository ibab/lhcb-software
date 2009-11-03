#include "Python.h"
#ifdef _WIN32
#ifndef WIN32
#define WIN32
#endif
#endif
#include "CoolKernel/IFolder.h"

namespace CondDBUI {
  /// Class to hold helper functions to simplify the access to the conditions
  //  database from Python.
  struct Helpers {
    /// Convert the map returned by cool::IFolder::listChannelsWithNames() into
    //  a Python dictionary (actually as work-around for a bug in ROOT >= 5.22/00d).
    static PyObject * getChannelsWithNames(const cool::IFolderPtr &ptr) {
      typedef std::map<cool::ChannelId,std::string> ChMap;
      PyObject *m = PyDict_New();

      const ChMap channels = ptr->listChannelsWithNames();
      PyObject *k, *v;
      for (ChMap::const_iterator i = channels.begin(); i != channels.end(); ++i) {
        k = PyInt_FromLong(static_cast<long>(i->first));
        v = PyString_FromString(i->second.c_str());
        PyDict_SetItem(m, k, v);
        Py_DECREF(k);
        Py_DECREF(v);
      }
      return m;
    }
  };
}

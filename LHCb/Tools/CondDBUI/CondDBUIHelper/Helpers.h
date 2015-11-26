#ifndef CONDDBUIHELPER_HELPERS_H
#define CONDDBUIHELPER_HELPERS_H
#include "Python.h"
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IFolderSet.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/pointers.h"

namespace CondDBUI {
  /// Class to hold helper functions to simplify the access to the conditions
  //  database from Python.
  struct Helpers {
    /// Convert the map returned by cool::IFolder::listChannelsWithNames() into
    //  a Python dictionary (actually as work-around for a bug in ROOT >= 5.22/00d).
    static PyObject * getChannelsWithNames(const cool::IFolderPtr &ptr);
    static const std::string resolveTag(const cool::IFolderPtr &ptr, const std::string& ancestorTagName );
    static const std::string resolveTag(const cool::IFolderSetPtr &ptr, const std::string& ancestorTagName );
    static const std::string findTagRelation(const cool::IFolderPtr &ptr, const std::string& parentTagName);
    static const std::string findTagRelation(const cool::IFolderSetPtr &ptr, const std::string& parentTagName);
    static bool deleteTagRelation(const cool::IFolderPtr &ptr, const std::string& parentTagName);
    static bool createTagRelation(const cool::IFolderPtr &ptr, const std::string& parentTagName, const std::string& tagName);
    static bool createTagRelation(const cool::IFolderSetPtr &ptr, const std::string& parentTagName, const std::string& tagName);
    static cool::IDatabasePtr openDatabase(const cool::IDatabaseSvc &ptr, const cool::DatabaseId &dbId, bool readOnly=true);
  };
}
#endif

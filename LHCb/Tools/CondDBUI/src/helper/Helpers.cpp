#include "Python.h"
#ifdef _WIN32
#ifndef WIN32
#define WIN32
#endif
#endif
#include "CoolKernel/IFolder.h"
#include "CoolKernel/IFolderSet.h"
#include "CoolKernel/Exception.h"
#include "CoralBase/Attribute.h"
#include "RelationalAccess/ConnectionServiceException.h"
#include <iostream>
#include <stdexcept>

#include "CondDBUIHelper/Helpers.h"

namespace CondDBUI {
  /// Class to hold helper functions to simplify the access to the conditions
  //  database from Python.
    PyObject * Helpers::getChannelsWithNames(const cool::IFolderPtr &ptr) {
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
    const std::string Helpers::resolveTag(const cool::IFolderPtr &ptr, const std::string& ancestorTagName ) {
      try {
          return ptr->resolveTag(ancestorTagName);
      }
      catch(coral::AttributeException&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return "";
      }
      catch(cool::TagRelationNotFound&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return "";
      }
      catch(cool::TagNotFound&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return "";
      }
      catch(cool::ReservedHeadTag&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return "";
      }
    }
    const std::string Helpers::resolveTag(const cool::IFolderSetPtr &ptr, const std::string& ancestorTagName ) {
      try {
          return ptr->resolveTag(ancestorTagName);
      }
      catch(coral::AttributeException&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return "";
      }
      catch(cool::TagRelationNotFound&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return "";
      }
      catch(cool::TagNotFound&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return "";
      }
      catch(cool::ReservedHeadTag&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return "";
      }
    }
    const std::string Helpers::findTagRelation(const cool::IFolderPtr &ptr, const std::string& parentTagName) {
      try {
          return ptr->findTagRelation(parentTagName);
      }
      catch(cool::TagRelationNotFound&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return "";
      }
/*      catch(cool::TagNotFound&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return "";
      }*/
    }
    const std::string Helpers::findTagRelation(const cool::IFolderSetPtr &ptr, const std::string& parentTagName) {
      try {
          return ptr->findTagRelation(parentTagName);
      }
      catch(cool::TagRelationNotFound&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return "";
      }
/*      catch(cool::TagNotFound&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return "";
      }*/
    }
    bool Helpers::deleteTagRelation(const cool::IFolderPtr &ptr, const std::string& parentTagName) {
      try {
          ptr->deleteTagRelation(parentTagName);
      }
      catch(cool::TagRelationNotFound&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return false;
      }
      catch(cool::TagNotFound&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return false;
      }
      return true;
    }
    bool Helpers::createTagRelation(const cool::IFolderPtr &ptr, const std::string& parentTagName, const std::string& tagName) {
      try {
          ptr->createTagRelation(parentTagName, tagName);
      }
      catch(cool::ReservedHeadTag&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return false;
      }
      return true;
    }
    bool Helpers::createTagRelation(const cool::IFolderSetPtr &ptr, const std::string& parentTagName, const std::string& tagName) {
      try {
          ptr->createTagRelation(parentTagName, tagName);
      }
      catch(cool::ReservedHeadTag&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return false;
      }
      return true;
    }
    cool::IDatabasePtr Helpers::openDatabase(const cool::IDatabaseSvc &ptr, const cool::DatabaseId &dbId, bool readOnly ) {
      try {
          return ptr->openDatabase(dbId, readOnly);
      }
      catch(coral::ConnectionNotAvailableException&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return NULL;
      }
      catch(cool::DatabaseDoesNotExist&){
          // Will not do anything in c++
//          std::cout<<"Exception found"<<std::endl;
          return NULL;
      }
    }
}

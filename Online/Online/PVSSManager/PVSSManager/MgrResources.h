// The Resource file for the Demo manager
#ifndef  PVSSMANAGER_MGRRESOURCES_H
#define  PVSSMANAGER_MGRRESOURCES_H

#include "Resources.hxx"

/*
 * PVSS namespace
 */
namespace PVSS {

  /** @class MgrResources MgrResources.h MgrResources.h
    *
    *  @author  M.Frank
    *  @version 1.0
    *
    */
  class  MgrResources : public Resources   {
    public:
      // These functions initializes the manager
      static  void  init(int &argc, char *argv[]);  
      // Read the config section
      static  PVSSboolean  readSection();

    public:
      // Get the name of our Datapoint to set
      static const CharString & dllName() {return m_dllName;}
      // Get the name of our Datapoint to set
      static const CharString & callName() {return m_callName;}

    private:
      // The name of our Datapoint to set
      static  CharString  m_dllName;
      static  CharString  m_callName;
  };
}

#endif  /* PVSSMANAGER_MGRRESOURCES_H */

// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiPackage.h,v 1.6 2002-01-28 18:49:18 mato Exp $
#ifndef DADIPACKAGE_H 
#define DADIPACKAGE_H 1

// Include files
#include <list>

#include "dom/DOMString.hpp"

#include "DaDiNamespace.h"
#include "DaDiClass.h"

/** @class DaDiPackage DaDiPackage.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiPackage {
public:
  /// Standard constructor
  DaDiPackage() {}; 

  virtual ~DaDiPackage() {}; ///< Standard destructor

  DOMString packageName();
  void setPackageName(DOMString value);

  std::string popImpSoftList();
  void pushImpSoftList(std::string value);
  int sizeImpSoftList();
  void remDblImpSoftList();
  
  std::string popImpStdList();
  void pushImpStdList(std::string value);
  int sizeImpStdList();
  void remDblImpStdList();
  
  std::string popImportList();
  void pushImportList(std::string value);
  int sizeImportList();
  void remDblImportList();
  
  DaDiClass* popDaDiClass();
  void pushDaDiClass(DaDiClass* value);
  int sizeDaDiClass();

  DaDiNamespace* popDaDiNamespace();
  void pushDaDiNamespace(DaDiNamespace* value);
  int sizeDaDiNamespace();

protected:

private:

  DOMString                 m_packageName;
  std::list<std::string>    m_impSoftList,
                            m_impStdList,
                            m_importList;
  std::list<DaDiClass*>     m_daDiClass;
  std::list<DaDiNamespace*> m_daDiNamespace;

};

inline DOMString DaDiPackage::packageName()
{
  return m_packageName;
}

inline void DaDiPackage::setPackageName(DOMString value)
{
  m_packageName = value;
}

inline std::string DaDiPackage::popImpSoftList()
{
  std::string pt =  m_impSoftList.front();
  m_impSoftList.push_back(pt);
  m_impSoftList.pop_front();
  return pt;
}

inline void DaDiPackage::pushImpSoftList(std::string value)
{
  bool doImport = true;
  for (int i=0; i<sizeImportList(); ++i)
  {
    if (value == popImportList())
    {
      doImport = false;
    }
  }
  if (doImport)
  {
	  m_impSoftList.push_back(value);
  }
}

inline int DaDiPackage::sizeImpSoftList()
{
  return m_impSoftList.size();
}

inline void DaDiPackage::remDblImpSoftList()
{
	m_impSoftList.sort();
	m_impSoftList.unique();
}

inline std::string DaDiPackage::popImpStdList()
{
  std::string pt =  m_impStdList.front();
  m_impStdList.push_back(pt);
  m_impStdList.pop_front();
  return pt;
}

inline void DaDiPackage::pushImpStdList(std::string value)
{
  m_impStdList.push_back(value);
}

inline int DaDiPackage::sizeImpStdList()
{
  return m_impStdList.size();
}

inline void DaDiPackage::remDblImpStdList()
{
	m_impStdList.sort();
	m_impStdList.unique();
}

inline std::string DaDiPackage::popImportList()
{
  std::string pt = m_importList.front();
  m_importList.push_back(pt);
  m_importList.pop_front();
  return pt;
}

inline void DaDiPackage::pushImportList(std::string value)
{
  int lastspace;
	std::string import, lastword;

	while (value != "")
	{
		int i = value.find_first_of(":,<>");

		if (i == -1)
		{
			import = value;
			value = "";
		}
		else
		{
			import = value.substr(0,i);
			value = value.substr(i+1,std::string::npos);
		}

    lastspace = import.find_last_of(" ");
    lastword = import.substr(lastspace+1, import.size()-lastspace);
    if ((lastword != "bool")   && (lastword != "short")    &&
        (lastword != "long")   && (lastword != "int")      && 
        (lastword != "float")  && (lastword != "double")   &&
        (lastword != "char")   && (lastword != "unsigned") &&
        (lastword != "signed") && (lastword != "")         &&
        (lastword != "string") && (lastword != "std"))
		{
			if ((import == "vector") || (import == "list")   ||
				(import == "deque")  || (import == "queue")    ||
				(import == "stack")  || (import == "map")      ||
				(import == "set")    || (import == "bitset")   ||
        (import == "pair"))
			{
				m_impStdList.push_back(import);
			}
			else
			{
        for (int i=0; i<sizeImpSoftList(); ++i)
        {
          if (import == m_impSoftList.front())
          {
            m_impSoftList.pop_front();
          }
        }
				m_importList.push_back(import);
			}
		}
	}
}

inline int DaDiPackage::sizeImportList()
{
  return m_importList.size();
}

inline void DaDiPackage::remDblImportList()
{
	m_importList.sort();
	m_importList.unique();
}

inline DaDiClass* DaDiPackage::popDaDiClass()
{
  DaDiClass* pt = m_daDiClass.front();
  m_daDiClass.push_back(pt);
  m_daDiClass.pop_front();
  return pt;
}

inline void DaDiPackage::pushDaDiClass(DaDiClass* value)
{
  m_daDiClass.push_back(value);
}

inline int DaDiPackage::sizeDaDiClass()
{
  return m_daDiClass.size();
}

inline DaDiNamespace* DaDiPackage::popDaDiNamespace()
{
  DaDiNamespace* pt = m_daDiNamespace.front();
  m_daDiNamespace.push_back(pt);
  m_daDiNamespace.pop_front();
  return pt;
}

inline void DaDiPackage::pushDaDiNamespace(DaDiNamespace* value)
{
  m_daDiNamespace.push_back(value);
}

inline int DaDiPackage::sizeDaDiNamespace()
{
  return m_daDiNamespace.size();
}

#endif // DADIPACKAGE_H


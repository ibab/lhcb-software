// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiNamespace.h,v 1.3 2002-02-01 18:01:52 mato Exp $
#ifndef DADINAMESPACE_H 
#define DADINAMESPACE_H 1

// Include files
#include <list>
#include <string>

#include "dom/DOMString.hpp"

#include "DaDiClass.h"
#include "DaDiEnum.h"

/** @class DaDiNamespace DaDiNamespace.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */

class DaDiNamespace {
public:
	/// Standard constructor
	DaDiNamespace() {}; 

	virtual ~DaDiNamespace() {}; ///< Standard destructor

	DOMString name();
	void setName(DOMString value);

	DOMString desc();
	void setDesc(DOMString value);
  
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
  
	DaDiEnum* popDaDiEnum();
	void pushDaDiEnum(DaDiEnum* value);
	int sizeDaDiEnum();
  
  DaDiAttribute* popDaDiAttribute();
	void pushDaDiAttribute(DaDiAttribute* value);
	int sizeDaDiAttribute();
  
protected:

private:

	DOMString                 m_name, 
                            m_desc;
	std::list<std::string>    m_impSoftList,
                            m_impStdList,
                            m_importList;
	std::list<DaDiEnum*>      m_daDiEnum;
	std::list<DaDiClass*>     m_daDiClass;
	std::list<DaDiAttribute*> m_daDiAttribute;
};


inline DOMString DaDiNamespace::name()
{
	return m_name;
}

inline void DaDiNamespace::setName(DOMString value)
{
	m_name = value;
}

inline DOMString DaDiNamespace::desc()
{
	return m_desc;
}

inline void DaDiNamespace::setDesc(DOMString value)
{
  m_desc = value;
}

inline std::string DaDiNamespace::popImpSoftList()
{
	std::string pt = m_impSoftList.front();
	m_impSoftList.push_back(pt);
	m_impSoftList.pop_front();
	return pt;
}

inline void DaDiNamespace::pushImpSoftList(std::string value)
{
  int i;
  unsigned long l;
  bool inList = false;
  for (i=0; i<sizeImportList(); ++i)
  {
    if (value == popImportList())
    {
      inList = true;
    }
  }
  for (l=0; l<m_impSoftList.size(); ++l)
  {
    if (value == popImpSoftList())
    {
      inList = true;
    }
  }
  if (!inList)
  {
	  m_impSoftList.push_back(value);
  }
}

inline int DaDiNamespace::sizeImpSoftList()
{
	return m_impSoftList.size();
}

inline void DaDiNamespace::remDblImpSoftList()
{
	m_impSoftList.sort();
	m_impSoftList.unique();
}

inline std::string DaDiNamespace::popImpStdList()
{
	std::string pt = m_impStdList.front();
	m_impStdList.push_back(pt);
	m_impStdList.pop_front();
	return pt;
}

inline void DaDiNamespace::pushImpStdList(std::string value)
{
  bool inList = false;
  for(unsigned long i=0; i<m_impStdList.size(); ++i)
  {
    if (value == popImpStdList())
    {
      inList = true;
    }
  }
  if (!inList)
  {
	  m_impStdList.push_back(value);
  }
}

inline int DaDiNamespace::sizeImpStdList()
{
	return m_impStdList.size();
}

inline void DaDiNamespace::remDblImpStdList()
{
	m_impStdList.sort();
	m_impStdList.unique();
}

inline std::string DaDiNamespace::popImportList()
{
	std::string pt = m_importList.front();
	m_importList.push_back(pt);
	m_importList.pop_front();
	return pt;
}

inline void DaDiNamespace::pushImportList(std::string value)
{
  unsigned long l;
  int i, lastspace;
	std::string import, lastword;

	while (value != "")
	{
		i = value.find_first_of(":,<>");

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

    while ((import[import.size()-1] == '*') ||
      (import[import.size()-1] == '&'))
    {
      import = import.substr(0,import.size()-1);
    }

    lastspace = import.find_last_of(" ");
    lastword = import.substr(lastspace+1, import.size()-lastspace);
    if ((lastword != "bool")   && (lastword != "short")    &&
        (lastword != "long")   && (lastword != "int")      && 
        (lastword != "float")  && (lastword != "double")   &&
        (lastword != "char")   && (lastword != "unsigned") &&
        (lastword != "signed") && (lastword != "")         &&
        (lastword != "std")    && (lastword != "string"))
		{
			if ((import == "vector") || (import == "list")   ||
				(import == "deque")    || (import == "queue")  ||
				(import == "stack")    || (import == "map")    ||
				(import == "set")      || (import == "bitset") ||
        (import == "pair"))
			{
				pushImpStdList(import);
			}
			else
			{
        for (l=0; l<m_impSoftList.size(); ++l)
        {
          if (import == m_impSoftList.front())
          {
            m_impSoftList.pop_front();
          }
        }
        bool inList = false;
        for (l=0; l<m_importList.size(); ++l)
        {
          if (import == popImportList())
          {
            inList = true;
          }
        }
        if (!inList)
        {
				  m_importList.push_back(import);
        }
			}
		}
	}
}

inline int DaDiNamespace::sizeImportList()
{
	return m_importList.size();
}

inline void DaDiNamespace::remDblImportList()
{
	m_importList.sort();
	m_importList.unique();
}

inline DaDiEnum* DaDiNamespace::popDaDiEnum()
{
	DaDiEnum* pt = m_daDiEnum.front();
	m_daDiEnum.push_back(pt);
	m_daDiEnum.pop_front();
	return pt;
}

inline void DaDiNamespace::pushDaDiEnum(DaDiEnum* value)
{
	m_daDiEnum.push_back(value);
}

inline int DaDiNamespace::sizeDaDiEnum()
{
	return m_daDiEnum.size();
}

inline DaDiClass* DaDiNamespace::popDaDiClass()
{
	DaDiClass* pt = m_daDiClass.front();
	m_daDiClass.push_back(pt);
	m_daDiClass.pop_front();
	return pt;
}

inline void DaDiNamespace::pushDaDiClass(DaDiClass* value)
{
	m_daDiClass.push_back(value);
}

inline int DaDiNamespace::sizeDaDiClass()
{
	return m_daDiClass.size();
}

inline DaDiAttribute* DaDiNamespace::popDaDiAttribute()
{
	DaDiAttribute* pt = m_daDiAttribute.front();
	m_daDiAttribute.push_back(pt);
	m_daDiAttribute.pop_front();
	return pt;
}

inline void DaDiNamespace::pushDaDiAttribute(DaDiAttribute* value)
{
	m_daDiAttribute.push_back(value);
}

inline int DaDiNamespace::sizeDaDiAttribute()
{
	return m_daDiAttribute.size();
}


#endif // DADINAMESPACE_H


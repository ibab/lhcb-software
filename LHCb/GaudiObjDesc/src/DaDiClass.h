// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiClass.h,v 1.13 2002-01-30 20:29:04 mato Exp $
#ifndef DADICLASS_H 
#define DADICLASS_H 1

// Include files
#include <list>
#include <string>

#include "dom/DOMString.hpp"

#include "DaDiRelation.h"
#include "DaDiMethod.h"
#include "DaDiAttribute.h"
#include "DaDiBaseClass.h"
#include "DaDiConstructor.h"
#include "DaDiDestructor.h"
#include "DaDiEnum.h"
#include "DaDiLocation.h"

/** @class DaDiClass DaDiClass.h
 *  
 *
 *  @author Stefan Roiser
 *  @date   14/06/2001
 */
class DaDiClass {
public:
	/// Standard constructor
	DaDiClass() {}; 

	virtual ~DaDiClass() {}; ///< Standard destructor

	DOMString className();
	void setClassName(DOMString value);

	DOMString classDesc();
	void setClassDesc(DOMString value);

	DOMString classAuthor();
	void setClassAuthor(DOMString value);

	DOMString classDerived();
	void setClassDerived(DOMString value);

	DOMString classID();
	void setClassID(DOMString value);

	DOMString longDesc();
	void setLongDesc(DOMString value);

	DOMString location();
	void setLocation(DOMString value);

	bool classTemplateVector();
	void setClassTemplateVector(bool value);
  
	bool classTemplateList();
	void setClassTemplateList(bool value);

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
  
	DaDiBaseClass* popDaDiBaseClass();
	void pushDaDiBaseClass(DaDiBaseClass* value);
	int sizeDaDiBaseClass();
  
	DaDiAttribute* popDaDiAttribute();
	void pushDaDiAttribute(DaDiAttribute* value);
	int sizeDaDiAttribute();
  
	DaDiRelation* popDaDiRelation();
	void pushDaDiRelation(DaDiRelation* value);
	int sizeDaDiRelation();
  
	DaDiMethod* popDaDiMethod();
	void pushDaDiMethod(DaDiMethod* value);
	int sizeDaDiMethod();
  
	DaDiConstructor* popDaDiConstructor();
	void pushDaDiConstructor(DaDiConstructor* value);
	int sizeDaDiConstructor();
  
	DaDiDestructor* popDaDiDestructor();
	void pushDaDiDestructor(DaDiDestructor* value);
	int sizeDaDiDestructor();

  DaDiEnum* popDaDiEnum();
	void pushDaDiEnum(DaDiEnum* value);
	int sizeDaDiEnum();
  
  DaDiLocation* popDaDiLocation();
	void pushDaDiLocation(DaDiLocation* value);
	int sizeDaDiLocation();
  
protected:

private:

	DOMString                   m_className, 
                              m_classDesc,
                              m_classAuthor, 
                              m_classDerived,
                              m_classID,
                              m_longDesc,
                              m_location;
  bool                        m_classTemplateVector,
                              m_classTemplateList;
	std::list<std::string>      m_impSoftList,
                              m_impStdList,
                              m_importList;
	std::list<DaDiBaseClass*>	  m_daDiBaseClass;
	std::list<DaDiAttribute*>	  m_daDiAttribute;
	std::list<DaDiRelation*>    m_daDiRelation;
	std::list<DaDiMethod*>      m_daDiMethod;
  std::list<DaDiConstructor*> m_daDiConstructor;
  std::list<DaDiDestructor*>  m_daDiDestructor;
  std::list<DaDiEnum*>        m_daDiEnum;
  std::list<DaDiLocation*>    m_daDiLocation;
};

inline DOMString DaDiClass::className()
{
	return m_className;
}

inline void DaDiClass::setClassName(DOMString value)
{
	m_className = value;
}

inline DOMString DaDiClass::classDesc()
{
	return m_classDesc;
}

inline void DaDiClass::setClassDesc(DOMString value)
{
	m_classDesc = value;
}

inline DOMString DaDiClass::classAuthor()
{
	return m_classAuthor;
}

inline void DaDiClass::setClassAuthor(DOMString value)
{
	m_classAuthor = value;
}

inline DOMString DaDiClass::classDerived()
{
	return m_classDerived;
}

inline void DaDiClass::setClassDerived(DOMString value)
{
	m_classDerived = value;
}

inline DOMString DaDiClass::classID()
{
	return m_classID;
}

inline void DaDiClass::setClassID(DOMString value)
{
	m_classID = value;
}

inline DOMString DaDiClass::longDesc()
{
	return m_longDesc;
}

inline void DaDiClass::setLongDesc(DOMString value)
{
	m_longDesc = value;
}

inline DOMString DaDiClass::location()
{
	return m_location;
}

inline void DaDiClass::setLocation(DOMString value)
{
	m_location = value;
}

inline bool DaDiClass::classTemplateVector()
{
	return m_classTemplateVector;
}

inline void DaDiClass::setClassTemplateVector(bool value)
{
	m_classTemplateVector = value;
}

inline bool DaDiClass::classTemplateList()
{
	return m_classTemplateList;
}

inline void DaDiClass::setClassTemplateList(bool value)
{
	m_classTemplateList = value;
}

inline std::string DaDiClass::popImpSoftList()
{
	std::string pt = m_impSoftList.front();
	m_impSoftList.push_back(pt);
	m_impSoftList.pop_front();
	return pt;
}

inline void DaDiClass::pushImpSoftList(std::string value)
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

inline int DaDiClass::sizeImpSoftList()
{
	return m_impSoftList.size();
}

inline void DaDiClass::remDblImpSoftList()
{
	m_impSoftList.sort();
	m_impSoftList.unique();
}

inline std::string DaDiClass::popImpStdList()
{
	std::string pt = m_impStdList.front();
	m_impStdList.push_back(pt);
	m_impStdList.pop_front();
	return pt;
}

inline void DaDiClass::pushImpStdList(std::string value)
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

inline int DaDiClass::sizeImpStdList()
{
	return m_impStdList.size();
}

inline void DaDiClass::remDblImpStdList()
{
	m_impStdList.sort();
	m_impStdList.unique();
}

inline std::string DaDiClass::popImportList()
{
	std::string pt = m_importList.front();
	m_importList.push_back(pt);
	m_importList.pop_front();
	return pt;
}

inline void DaDiClass::pushImportList(std::string value)
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

inline int DaDiClass::sizeImportList()
{
	return m_importList.size();
}

inline void DaDiClass::remDblImportList()
{
	m_importList.sort();
	m_importList.unique();
}


inline DaDiBaseClass* DaDiClass::popDaDiBaseClass()
{
	DaDiBaseClass* pt = m_daDiBaseClass.front();
	m_daDiBaseClass.push_back(pt);
	m_daDiBaseClass.pop_front();
	return pt;
}

inline void DaDiClass::pushDaDiBaseClass(DaDiBaseClass* value)
{
	m_daDiBaseClass.push_back(value);
}

inline int DaDiClass::sizeDaDiBaseClass()
{
	return m_daDiBaseClass.size();
}

inline DaDiAttribute* DaDiClass::popDaDiAttribute()
{
	DaDiAttribute* pt = m_daDiAttribute.front();
	m_daDiAttribute.push_back(pt);
	m_daDiAttribute.pop_front();
	return pt;
}

inline void DaDiClass::pushDaDiAttribute(DaDiAttribute* value)
{
	m_daDiAttribute.push_back(value);
}

inline int DaDiClass::sizeDaDiAttribute()
{
	return m_daDiAttribute.size();
}

inline DaDiRelation* DaDiClass::popDaDiRelation()
{
	DaDiRelation* pt = m_daDiRelation.front();
	m_daDiRelation.push_back(pt);
	m_daDiRelation.pop_front();
	return pt;
}

inline void DaDiClass::pushDaDiRelation(DaDiRelation* value)
{
	m_daDiRelation.push_back(value);
}

inline int DaDiClass::sizeDaDiRelation()
{
	return m_daDiRelation.size();
}

inline DaDiMethod* DaDiClass::popDaDiMethod()
{
	DaDiMethod* pt = m_daDiMethod.front();
	m_daDiMethod.push_back(pt);
	m_daDiMethod.pop_front();
	return pt;
}

inline void DaDiClass::pushDaDiMethod(DaDiMethod* value)
{
	m_daDiMethod.push_back(value);
}

inline int DaDiClass::sizeDaDiMethod()
{
	return m_daDiMethod.size();
}

inline DaDiConstructor* DaDiClass::popDaDiConstructor()
{
	DaDiConstructor* pt = m_daDiConstructor.front();
	m_daDiConstructor.push_back(pt);
	m_daDiConstructor.pop_front();
	return pt;
}

inline void DaDiClass::pushDaDiConstructor(DaDiConstructor* value)
{
	m_daDiConstructor.push_back(value);
}

inline int DaDiClass::sizeDaDiConstructor()
{
	return m_daDiConstructor.size();
}

inline DaDiDestructor* DaDiClass::popDaDiDestructor()
{
	DaDiDestructor* pt = m_daDiDestructor.front();
	m_daDiDestructor.push_back(pt);
	m_daDiDestructor.pop_front();
	return pt;
}

inline void DaDiClass::pushDaDiDestructor(DaDiDestructor* value)
{
	m_daDiDestructor.push_back(value);
}

inline int DaDiClass::sizeDaDiDestructor()
{
	return m_daDiDestructor.size();
}

inline DaDiEnum* DaDiClass::popDaDiEnum()
{
	DaDiEnum* pt = m_daDiEnum.front();
	m_daDiEnum.push_back(pt);
	m_daDiEnum.pop_front();
	return pt;
}

inline void DaDiClass::pushDaDiEnum(DaDiEnum* value)
{
	m_daDiEnum.push_back(value);
}

inline int DaDiClass::sizeDaDiEnum()
{
	return m_daDiEnum.size();
}

inline DaDiLocation* DaDiClass::popDaDiLocation()
{
	DaDiLocation* pt = m_daDiLocation.front();
	m_daDiLocation.push_back(pt);
	m_daDiLocation.pop_front();
	return pt;
}

inline void DaDiClass::pushDaDiLocation(DaDiLocation* value)
{
	m_daDiLocation.push_back(value);
}

inline int DaDiClass::sizeDaDiLocation()
{
	return m_daDiLocation.size();
}


#endif // DADICLASS_H


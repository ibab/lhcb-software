// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiClass.h,v 1.3 2001-10-10 16:09:32 mato Exp $
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

	DOMString classTemplateVector();
	void setClassTemplateVector(DOMString value);
  
	DOMString classTemplateList();
	void setClassTemplateList(DOMString value);

	DOMString classID();
	void setClassID(DOMString value);

	std::string popImpSoftList();
	void pushImpSoftList(std::string value);
	int sizeImpSoftList();
  
	std::string popImpStdList();
	void pushImpStdList(std::string value);
	int sizeImpStdList();
  
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
  
protected:

private:

	DOMString                 m_className, 
                            m_classDesc, 
                            m_classAuthor, 
                            m_classDerived,
                            m_classTemplateVector,
                            m_classTemplateList,
                            m_classID;
	std::list<std::string>    m_impSoftList,
                            m_impStdList,
                            m_importList;
	std::list<DaDiBaseClass*>	m_daDiBaseClass;
	std::list<DaDiAttribute*>	m_daDiAttribute;
	std::list<DaDiRelation*>  m_daDiRelation;
	std::list<DaDiMethod*>    m_daDiMethod;
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

inline DOMString DaDiClass::classTemplateVector()
{
	return m_classTemplateVector;
}

inline void DaDiClass::setClassTemplateVector(DOMString value)
{
	m_classTemplateVector = value;
}

inline DOMString DaDiClass::classTemplateList()
{
	return m_classTemplateList;
}

inline void DaDiClass::setClassTemplateList(DOMString value)
{
	m_classTemplateList = value;
}

inline DOMString DaDiClass::classID()
{
	return m_classID;
}

inline void DaDiClass::setClassID(DOMString value)
{
	m_classID = value;
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
	m_impSoftList.push_back(value);
}

inline int DaDiClass::sizeImpSoftList()
{
	return m_impSoftList.size();
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
	m_impStdList.push_back(value);
}

inline int DaDiClass::sizeImpStdList()
{
	return m_impStdList.size();
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

	std::string import;

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

		if ((import != "")       && (import != "long")     &&
			(import != "bool")   && (import != "short")    && 
			(import != "long")   && (import != "string")   &&
			(import != "int")    && (import != "float")    &&
			(import != "double") && (import != "unsigned") &&
			(import != "signed") && (import != "std")      &&
			(import != "pair")   && (import != "char"))
		{
			if ((import == "vector") || (import == "list")   ||
				(import == "deque")  || (import == "queue")  ||
				(import == "stack")  || (import == "map")    ||
				(import == "set")    || (import == "bitset"))
			{
				m_impStdList.push_back(import);
			}
			else
			{
				m_importList.push_back(import);
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

#endif // DADICLASS_H


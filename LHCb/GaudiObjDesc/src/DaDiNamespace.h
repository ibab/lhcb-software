// $Header: /afs/cern.ch/project/cvs/reps/lhcb/GaudiObjDesc/src/DaDiNamespace.h,v 1.2 2002-01-30 20:29:05 mato Exp $
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
  
	DaDiClass* popDaDiClass();
	void pushDaDiClass(DaDiClass* value);
	int sizeDaDiClass();
  
	DaDiEnum* popDaDiEnum();
	void pushDaDiEnum(DaDiEnum* value);
	int sizeDaDiEnum();
  
protected:

private:

	DOMString             m_name, 
                            m_desc;
	std::list<DaDiEnum*>  m_daDiEnum;
	std::list<DaDiClass*> m_daDiClass;
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


#endif // DADINAMESPACE_H


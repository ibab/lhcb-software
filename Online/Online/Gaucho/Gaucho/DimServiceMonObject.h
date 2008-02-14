#ifndef DIMSERVICEMONOBJECT_H
#define DIMSERVICEMONOBJECT_H 1

#include <string>
#include <iostream>
#include "dis.hxx"
#include <TStopwatch.h>
#include <sstream>
#include "GaudiKernel/IMessageSvc.h"

class MonObject;
class DimMonObjectManager;

class DimServiceMonObject : public DimService {

private:
	DimMonObjectManager* 	m_dimMonObjectManager;
	int 		m_size;
	void* 		m_data;
	MonObject*	m_monObject;
	std::string 	m_svcName;
	std::stringstream m_ss;

public: 
	DimServiceMonObject(DimMonObjectManager* manager, std::string svcName, MonObject *monObject, int size);
	virtual ~DimServiceMonObject();
        void updateService(bool endOfRun);
	void setDataFromMonObject();
	void serviceHandler();
	MonObject* monObject(){return m_monObject;};
	void deleteMonObject();
	
};

#endif // DIMSERVICEMONOBJECT_H

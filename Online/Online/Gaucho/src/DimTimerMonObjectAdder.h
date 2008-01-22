#ifndef DIMTIMERMONOBJECTADDER_H
#define DIMTIMERMONOBJECTADDER_H 1

#include <dis.hxx>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>


class DimInfoMonObject;
class DimServiceMonObject;
class MonObject;
class DimMonObjectManager;

class DimTimerMonObjectAdder: public DimTimer {
private: 
	DimMonObjectManager* 	m_dimMonObjectManager;
	int m_time;
	std::vector<std::vector<DimInfoMonObject*> >	m_dimInfoMonObjectVect2D;
	std::vector<DimServiceMonObject*>		m_dimServiceMonObjectVect1D;
public: 
	DimTimerMonObjectAdder (DimMonObjectManager* manager, int ttime, std::vector<std::vector<DimInfoMonObject*> > dimInfo2D, std::vector<DimServiceMonObject*> dimService1D);
	void timerHandler();
  
};   


#endif    // DIMTIMERMONOBJECTADDER_H

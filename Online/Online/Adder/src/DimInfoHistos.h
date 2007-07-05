#ifndef DimInfoHistos_DimInfoHistos_H
#define DimInfoHistos_DimInfoHistos_H 1


#include "dic.hxx"
#include "dis.hxx"
#include <string>
#include <iostream>
#include <vector>
using namespace std;

//forward declaration


/*
  Class DimInfoHistos
  A DimInfo that retrieve a histogram published by Gaucho
  and fill a root histogram
  
  Author: J. Helder Lopes, 27/07/2006
  
*/

class DimService;


class DimInfoHistoService : public DimService  {
public:
   DimInfoHistoService(std::string hname, float* data, int size, int dimension);
   virtual ~DimInfoHistoService();
   int dimension;
   float* data;
   private:
   void serviceHandler();
   int size;	
};



class DimInfoHistos : public DimInfo {
  
public : 
	DimInfoHistos(std::string hSvcname, int rtime);
	DimInfoHistos(std::string hSvcname);
	virtual ~DimInfoHistos();
	bool add(DimInfoHistos* h);
	bool add2d(DimInfoHistos* h);
	bool addp(DimInfoHistos* h);
	void declareInfo(std::string SumSvcname);
	void declareTitleInfo(std::string CommentSvcname, char* Title);
	void updateSum();
	std::string m_histoname;
	int entries() const;
	int m_dimension;
	int m_entries;
	int m_size;
	float* m_data;
	float* m_data_sum;
	float getbuffercontent(int ibuffer) const;
	DimInfoHistoService* m_dimSvc;
	DimService* m_dimTitleSvc;


private:
  void infoHandler();// Overloaded method infoHandler, called whenever commands arrive 


  };
#endif    // DimInfoHistos_DimInfoHistos_H

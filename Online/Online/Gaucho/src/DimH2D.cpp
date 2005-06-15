// $Id: DimH2D.cpp,v 1.3 2005-06-15 15:02:30 cattanem Exp $

#include "AIDA/IAxis.h"
#include "AIDA/IHistogram2D.h"
#include "DimH2D.h"
#include <iostream>
using namespace AIDA;

//constructor
DimH2D::DimH2D(std::string h2dname, const IHistogram2D* InfoVar) 
  //  : DimCommand(("H2D"+h2dname).c_str(),"I"){
  : DimRpc(("H2D"+h2dname).c_str(),"I:1","F") {

  m_name = h2dname;
  m_h2d = InfoVar;
  m_nbinsX = (m_h2d->xAxis()).bins();
  m_nbinsY = (m_h2d->yAxis()).bins();
  m_pars = new float[6];
  m_pars[0] = (float)m_nbinsX;
  m_pars[1] = (m_h2d->xAxis()).lowerEdge();
  m_pars[2] = (m_h2d->xAxis()).upperEdge();
  m_pars[3] = (float)m_nbinsY;
  m_pars[4] = (m_h2d->yAxis()).lowerEdge();
  m_pars[5] = (m_h2d->yAxis()).upperEdge();
  int total=m_nbinsX*m_nbinsY;
  m_data = new float[2*total];// entries and error contents
  int i,j;
  for (i=0; i<m_nbinsX; ++i) {
    for (j=0; j<m_nbinsY; ++j) {
      m_data[m_nbinsY*i+j]=0.0;         // Entries
      m_data[total+m_nbinsY*i+j]=0.0;   // Errors
    }
  }
}

//destructor
DimH2D::~DimH2D() {
  delete [] m_pars;
  delete [] m_data;
}

// Overloaded method commandHandler gets called whenever commands arrive, 
void DimH2D::rpcHandler() {
  //commandHandler() {
  int i,j,total;
  int command=getInt();
  //  std::cout << "DimH2D received command "<<command<< std::endl;
  switch (command) {
	case 1: 
    // request for histo parameters transfer
    //  std::cout <<  "DIMH2D received command 1:  request for histo parameters transfer" << std::endl;
		setData(m_pars,6*sizeof(float));
    break;
	case 2:
    //request for histo bin contents transfer
    //  std::cout <<  "DIMH2D received command 2: request for histo bin contents transfer" << std::endl;
    total=m_nbinsX*m_nbinsY;
    for (i=0; i<m_nbinsX; ++i) {
      for (j=0; j<m_nbinsY; ++j) {
        m_data[      m_nbinsY*i+j]= m_h2d->binEntries(i,j);
        m_data[total+m_nbinsY*i+j]= m_h2d->binError  (i,j);
        // std::cout << "DimH2D:" << i << " " << j << " " 
        // << m_h2d->binEntries(i,j) << " " << m_nbinsY*i+j
        // << " " << m_data[m_nbinsY*i+j] << " " 
        // << total+m_nbinsY*i+j << " " << m_h2d->binError(i,j) 
        // << " "<< m_data[total+m_nbinsY*i+j] << std::endl;
      }
    }
		setData(m_data,2*total*sizeof(float));
    break;
	} // end switch
} // end commandhandler

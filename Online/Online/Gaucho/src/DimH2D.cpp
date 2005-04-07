//$Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/Gaucho/src/DimH2D.cpp,v 1.1 2005-04-07 14:44:30 evh Exp $

#include "AIDA/IAxis.h"
#include "AIDA/IHistogram2D.h"
#include "Gaucho/DimH2D.h"
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
  m_entries = new float[m_nbinsX*m_nbinsY];
  m_errors  = new float[m_nbinsX*m_nbinsY];
  int i,j;
  for (i=0; i<m_nbinsX; ++i) {
    for (j=0; j<m_nbinsY; ++j) {
      m_entries[m_nbinsX*i+j]=0.0; 
      m_errors [m_nbinsX*i+j]=0.0;
    }
  }
}

//destructor
DimH2D::~DimH2D() {
  delete [] m_pars;
  delete [] m_entries;
  delete [] m_errors;
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
//                std::cout <<  "DIMH2D received command 1:  request for histo parameters transfer" << std::endl;
		setData(m_pars,6*sizeof(float));
    break;
	case 2:
    //request for histo bin contents transfer
//                std::cout <<  "DIMH2D received command 2: request for histo bin contents transfer" << std::endl;
                for (i=0; i<m_nbinsX; ++i) {
                  for (j=0; j<m_nbinsY; ++j) {
                    m_entries[m_nbinsX*i+j]= m_h2d->binEntries(i,j);
//                    std::cout << "DimH2D " << i << " " << j << " " << m_nbinsX*i+j << " " << m_entries[m_nbinsX*i+j] << std::endl;
                  }
                }
                total=m_nbinsX*m_nbinsY;
		setData(m_entries,total*sizeof(float));
    break;
        case 3:
    //request for histo bin errors transfer
//                std::cout <<  "DIMH2D received command 3: request for histo bin errors transfer" << std::endl;
                for (i=0; i<m_nbinsX; ++i) {
                  for (j=0; j<m_nbinsY; ++j) {
                    m_errors[m_nbinsX*i+j]= m_h2d->binError(i,j);
                  }
                }
                total=m_nbinsX*m_nbinsY;
                setData(m_errors,total*sizeof(float));
    break;
	} // end switch
} // end commandhandler

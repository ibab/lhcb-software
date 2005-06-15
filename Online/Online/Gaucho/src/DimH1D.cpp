// $Id: DimH1D.cpp,v 1.6 2005-06-15 15:02:30 cattanem Exp $

#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
#include "DimH1D.h"
#include <iostream>
using namespace AIDA;

//constructor
DimH1D::DimH1D(std::string h1dname, IHistogram1D* InfoVar) 
  //  : DimCommand(("H1D"+h1dname).c_str(),"I"){
  : DimRpc(("H1D"+h1dname).c_str(),"I:1","F") {
  m_name = h1dname;
  m_h1d = InfoVar;
  m_nbins = (m_h1d->axis()).bins();
  m_pars = new float[3];
  m_pars[0] = (float)m_nbins;
  m_pars[1] = (m_h1d->axis()).lowerEdge();
  m_pars[2] = (m_h1d->axis()).upperEdge();
  m_data = new float[2*m_nbins];
  int i;
  for (i=0; i<m_nbins; i++) m_data[i]=0.0; 
}

//destructor
DimH1D::~DimH1D() {
  delete [] m_pars;
  delete [] m_data;
}

// Overloaded method commandHandler gets called whenever commands arrive, 
void DimH1D::rpcHandler() {
  //commandHandler() {
  int command=getInt();
  switch (command) {
	case 1: 
    // request for histo parameters transfer
    //    std::cout << "DIMH1D received command 1" << std::endl;
		setData(m_pars,3*sizeof(float));
    break;
	case 2:
    //request for histo bin contents transfer
		int binnr,total;
    //		 std::cout << "DIMH1D received command 2" << std::endl;
		for (binnr=0;binnr<m_nbins;binnr++) {
			m_data[binnr] = m_h1d->binHeight(binnr);
		}
		total=2*m_nbins;
		for (binnr=0;binnr<m_nbins;binnr++) {
			m_data[m_nbins+binnr] = m_h1d->binError(binnr);
		}
		setData(m_data,total*sizeof(float));
    //		setData(m_data,m_nbins*sizeof(float));
	} // end switch
} // end commandhandler

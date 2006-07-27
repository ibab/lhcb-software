// $Id: DimHisto.cpp,v 1.1 2006-07-27 15:45:59 evh Exp $

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "DimHisto.h"
#include <iostream>
using namespace AIDA;

//DimHisto constructor
DimHisto::DimHisto(std::string hname, const AIDA::IHistogram* InfoVar, ISvcLocator* svclocator)
{
  // get msg logging
  StatusCode sc = svclocator->service("MessageSvc", m_msgsvc);
  MsgStream msg(m_msgsvc, "DimHisto");
  if ( sc.isFailure() ) {
    msg << MSG::FATAL << "Error locating MessageSvc." << endreq;
  }
  else {
    msg << MSG::DEBUG << "MessageSvc located." << endreq;
  }
  
	int dimension = InfoVar->dimension();
	int size;
  if( 1 == dimension ){
    int nXBins = (dynamic_cast<const AIDA::IHistogram1D*>(InfoVar))->axis().bins();
    // 1Hd data: dimension,nXBins,xMin,xMax,entries,2*(UNDERFLOW,"in range" bins, OVERFLOW): entries and errors
    size = (5+2*(nXBins+2))*sizeof(float);
  }
  else if( 2 == dimension ){
    int nXBins = (dynamic_cast<const AIDA::IHistogram2D*>(InfoVar))->xAxis().bins();
    int nYBins = (dynamic_cast<const AIDA::IHistogram2D*>(InfoVar))->yAxis().bins();
    // 2Hd data: dimension,nXBins,xMin,xMax,nYBins,yMin,yMax,entries,
    // 2*((UNDERFLOW,"in range" bins, OVERFLOW)_x*(UNDERFLOW,"in range" bins,OVERFLOW)_y)
    size = (8+2*(nXBins+2)*(nYBins+2))*sizeof(float);
  }
  else {
    msg << MSG::ERROR << "Unable to instantiate a DimHisto for " << dimension << "D histogram " 
        <<  hname << endmsg;
    exit(1);
  }
  
  m_dimHistoScv = new DimHistoService(hname, InfoVar, size, svclocator);
  
}

//destructor
DimHisto::~DimHisto() {
  if(!m_dimHistoScv) delete [] m_dimHistoScv;
}


//DimHistoService constructor
DimHistoService::DimHistoService(std::string hname, const AIDA::IHistogram* InfoVar, 
                                 int size, ISvcLocator* svclocator) 
  : DimService(hname.c_str(),"F",m_data,size),
    m_size(size)
    ,m_data(0)
    ,m_h1D(0)
    ,m_h2D(0)
{
  // get msg logging
  StatusCode sc = svclocator->service("MessageSvc", m_msgsvc);
  MsgStream msg(m_msgsvc, "DimHisto");
  if ( sc.isFailure() ) {
    msg << MSG::FATAL << "Error locating MessageSvc." << endreq;
  }
  else {
    msg << MSG::DEBUG << "MessageSvc located." << endreq;
  }
  
  m_name = hname;
  m_data = new float[size];
  m_dimension = InfoVar->dimension();
  
  if( 1 == m_dimension ){
    m_h1D = (dynamic_cast<const AIDA::IHistogram1D*>(InfoVar));
    
    m_nXBins = (m_h1D->axis()).bins();
    m_nYBins =0;
    m_data[0]=m_dimension;
    m_data[1]=m_nXBins;
    m_data[2]=(m_h1D->axis()).lowerEdge();
    m_data[3]=(m_h1D->axis()).upperEdge();
    m_data[4]=0;
    for (int i=5; i<2*(m_nXBins+2); i++) m_data[i]=0.0; 
    
  }
  else if( 2 == m_dimension ){
    m_h2D = (dynamic_cast<const AIDA::IHistogram2D*>(InfoVar));
    
    m_nXBins = (m_h2D->xAxis()).bins();
    m_nYBins = (m_h2D->yAxis()).bins();
    m_data[0]=m_dimension;
    m_data[1]=m_nXBins;
    m_data[2]=(m_h2D->xAxis()).lowerEdge();
    m_data[3]=(m_h2D->xAxis()).upperEdge();
    m_data[4]=m_nYBins;
    m_data[5]=(m_h2D->yAxis()).lowerEdge();
    m_data[6]=(m_h2D->yAxis()).upperEdge();
    m_data[7]=0;
    for (int i=8; i<2*(m_nXBins+2)*(m_nYBins+2); i++) m_data[i]=0.0; 
  }
  else {
    msg << MSG::ERROR << "Unable to instantiate a DimHistoService for " << m_dimension << "D histogram " 
        <<  hname << endmsg;
    exit(1);
  }
  msg << MSG::DEBUG << "Successfully instantiated DimHistoService for " << m_dimension << "D histogram " 
      <<  hname << endmsg;
  msg << MSG::DEBUG << "nXBins=" << m_nXBins << "  nYBins=" << m_nYBins 
      << " data size=" << m_size/sizeof(float) << " elements(" << m_size << " bytes)" << endmsg;
}

//destructor
DimHistoService::~DimHistoService() {
  if(!m_data) delete [] m_data;
}

// Overloaded method serviceHandler gets called whenever commands arrive, 
void DimHistoService::serviceHandler() {
  MsgStream msg(m_msgsvc, "DimHisto");
  int iData;
  // Update DimHisto data
  
  msg << MSG::DEBUG  << "DimHisto::serviceHandler - AIDA::IHistogram" << m_name 
      << ": UNDERFLOW bin= " << AIDA::IAxis::UNDERFLOW_BIN 
      << " OVERFLOW bin=" << AIDA::IAxis::OVERFLOW_BIN << endmsg;
  switch (m_dimension) {
	case 1:
    //Sum of the entries in all the IHistogram's bins, i.e in-range bins, UNDERFLOW and OVERFLOW. 
    //This is equivalent to the number of times the method fill was invoked.
		m_data[4]= m_h1D->allEntries();
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h1D " << m_name 
        << ": Entries: " << m_h1D->allEntries() << endmsg;
    
    // Entries (or should be height=entry*weight ?)
    iData = 5;
		m_data[iData++]= m_h1D->binHeight(AIDA::IAxis::UNDERFLOW_BIN);
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h1D " << m_name << ": ixBin = " << "UNDERFLOW"
        << " iData = " << iData-1 
        << " binHeight = " << m_h1D->binHeight(AIDA::IAxis::UNDERFLOW_BIN) << endmsg;		
    for (int ixBin=0; ixBin<m_nXBins; ixBin++){
			m_data[iData++]= m_h1D->binHeight(ixBin);
      msg << MSG::DEBUG  << "DimHisto::serviceHandler - h1D " << m_name << ": ixBin = " << ixBin
		      << " iData = " << iData-1 
		      << " binHeight = " << m_h1D->binHeight(ixBin) << endmsg;
    }
		m_data[iData++]= m_h1D->binHeight(AIDA::IAxis::OVERFLOW_BIN);
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h1D " << m_name << ": ixBin = " << "OVERFLOW"
        << " iData = " << iData-1 
        << " binHeight = " << m_h1D->binHeight(AIDA::IAxis::OVERFLOW_BIN) << endmsg;
		
		// Errors
		m_data[iData++]= m_h1D->binError(AIDA::IAxis::UNDERFLOW_BIN);
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h1D " << m_name << ": ixBin = " << "UNDERFLOW"
        << " iData = " << iData-1 
        << " binError = " << m_h1D->binError(AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
    for (int ixBin=0; ixBin<m_nXBins; ixBin++){		
			m_data[iData++]= m_h1D->binError (ixBin);
      msg << MSG::DEBUG  << "DimHisto::serviceHandler - h1D " << m_name << ": ixBin = " << ixBin
		      << " iData = " << iData-1 
		      << " binError = " << m_h1D->binError(ixBin) << endmsg;
    }
		m_data[iData++]= m_h1D->binError(AIDA::IAxis::OVERFLOW_BIN);
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h1D " << m_name << ": ixBin = " << "OVERFLOW"
        << " iData = " << iData-1 
        << " binError = " << m_h1D->binError(AIDA::IAxis::OVERFLOW_BIN) << endmsg;
    break;
		
	case 2:
    //Sum of the entries in all the IHistogram's bins, i.e in-range bins, UNDERFLOW and OVERFLOW. 
    //This is equivalent to the number of times the method fill was invoked.
		m_data[7]= m_h2D->allEntries();
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name 
        << ": Entries: " << m_h2D->allEntries() << endmsg;
		
    // Entries (in fact, height=entry*weight ?)
    iData = 8;
		m_data[iData++]= m_h2D->binHeight(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN);
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << "UNDERFLOW"
        << " iyBin = " << "UNDERFLOW" << " iData = " << iData-1 
        <<" binHeight = " << m_h2D->binHeight(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
		for (int iyBin=0; iyBin<m_nYBins; iyBin++){
      m_data[iData++]= m_h2D->binHeight(AIDA::IAxis::UNDERFLOW_BIN,iyBin);
      msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << "UNDERFLOW"
		      << " iyBin = " << iyBin << " iData = " << iData-1 
		      << " binHeight = " << m_h2D->binHeight(AIDA::IAxis::UNDERFLOW_BIN,iyBin) << endmsg;
		}
		m_data[iData++]= m_h2D->binHeight(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN);
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << "UNDERFLOW"
        << " iyBin = " << "OVERFLOW" << " iData = " << iData-1 
        << " binHeight = " << m_h2D->binHeight(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN) << endmsg;
    for (int ixBin=0; ixBin<m_nXBins; ixBin++){		
      m_data[iData++]= m_h2D->binHeight(ixBin,AIDA::IAxis::UNDERFLOW_BIN);
      msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << ixBin
		      << " iyBin = " << "UNDERFLOW" << " iData = " << iData-1 
		      << " binHeight = " << m_h2D->binHeight(ixBin,AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
      for (int iyBin=0; iyBin<m_nYBins; iyBin++){
        m_data[iData++]= m_h2D->binHeight(ixBin,iyBin);
        msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << ixBin
            << " iyBin = " << iyBin << " iData = " << iData-1 
            << " binHeight = " << m_h2D->binHeight(ixBin,iyBin) << endmsg;
      }
      m_data[iData++]= m_h2D->binHeight(ixBin,AIDA::IAxis::OVERFLOW_BIN);
      msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << ixBin
		      << " iyBin = " << "OVERFLOW" << " iData = " << iData-1 
		      << " binHeight = " << m_h2D->binHeight(ixBin,AIDA::IAxis::OVERFLOW_BIN) << endmsg;
    }
    
		m_data[iData++]= m_h2D->binHeight(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN);
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << "OVERFLOW"
        << " iyBin = " << "UNDERFLOW" << " iData = " << iData-1 
        << " binHeight = " << m_h2D->binHeight(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
		for (int iyBin=0; iyBin<m_nYBins; iyBin++){
      m_data[iData++]= m_h2D->binHeight(AIDA::IAxis::OVERFLOW_BIN,iyBin);
      msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << "OVERFLOW"
		      << " iyBin = " << iyBin << " iData = " << iData-1 
		      << " binHeight = " << m_h2D->binHeight(AIDA::IAxis::OVERFLOW_BIN,iyBin) << endmsg;
		}
    m_data[iData++]= m_h2D->binHeight(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN);
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << "OVERFLOW"
        << " iyBin = " << "OVERFLOW" << " iData = " << iData-1 
        << " binHeight = " << m_h2D->binHeight(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN) << endmsg;
    
    // Errors
		m_data[iData++]= m_h2D->binError(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN);
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << "UNDERFLOW"
        << " iyBin = " << "UNDERFLOW" << " iData = " << iData-1 
        << " binError = " << m_h2D->binError(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
		for (int iyBin=0; iyBin<m_nYBins; iyBin++){
      m_data[iData++]= m_h2D->binError(AIDA::IAxis::UNDERFLOW_BIN,iyBin);
      msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << "UNDERFLOW"
		      << " iyBin = " << iyBin << " iData = " << iData-1 
		      << " binError = " << m_h2D->binError(AIDA::IAxis::UNDERFLOW_BIN,iyBin) << endmsg;
		}
		m_data[iData++]= m_h2D->binError(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN);
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << "UNDERFLOW"
        << " iyBin = " << "OVERFLOW" << " iData = " << iData-1 
        << " binError = " << m_h2D->binError(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN) << endmsg;
    for (int ixBin=0; ixBin<m_nXBins; ixBin++){		
      m_data[iData++]= m_h2D->binError(ixBin,AIDA::IAxis::UNDERFLOW_BIN);
      msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << ixBin
		      << " iyBin = " << "UNDERFLOW" << " iData = " << iData-1 
		      << " binError = " << m_h2D->binError(ixBin,AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
      for (int iyBin=0; iyBin<m_nYBins; iyBin++){
        m_data[iData++]= m_h2D->binError(ixBin,iyBin);
        msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << ixBin
            << " iyBin = " << iyBin << " iData = " << iData-1 
            << " binError = " << m_h2D->binError(ixBin,iyBin) << endmsg;
      }
      m_data[iData++]= m_h2D->binError(ixBin,AIDA::IAxis::OVERFLOW_BIN);
      msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << ixBin
		      << " iyBin = " << "OVERFLOW" << " iData = " << iData-1 
		      << " binError = " << m_h2D->binError(ixBin,AIDA::IAxis::OVERFLOW_BIN) << endmsg;
    }
    
		m_data[iData++]= m_h2D->binError(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN);
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << "OVERFLOW"
        << " iyBin = " << "UNDERFLOW" << " iData = " << iData-1 
        << " binError = " << m_h2D->binError(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
		for (int iyBin=0; iyBin<m_nYBins; iyBin++){
      m_data[iData++]= m_h2D->binError(AIDA::IAxis::OVERFLOW_BIN,iyBin);
      msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << "OVERFLOW"
		      << " iyBin = " << iyBin << " iData = " << iData-1 
		      << " binError = " << m_h2D->binError(AIDA::IAxis::OVERFLOW_BIN,iyBin) << endmsg;
		}
    m_data[iData++]= m_h2D->binError(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN);
    msg << MSG::DEBUG  << "DimHisto::serviceHandler - h2D " << m_name << ": ixBin = " << "OVERFLOW"
        << " iyBin = " << "OVERFLOW" << " iData = " << iData-1 
        << " binError = " << m_h2D->binError(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN) << endmsg;
    
 	} 
  setData(m_data,m_size);
}

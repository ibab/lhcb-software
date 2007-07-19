// $Id: DimHisto.cpp,v 1.4 2007-07-19 07:22:44 evh Exp $

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"
#include "DimHisto.h"
#include <iostream>
using namespace AIDA;

//DimHisto constructor
DimHisto::DimHisto(std::string hname, const AIDA::IBaseHistogram* InfoVar, ISvcLocator* svclocator)
{
  // get msg logging
  StatusCode sc = svclocator->service("MessageSvc", m_msgsvc);
  if ( sc.isFailure() ) {
    std::cout << hname << "\tMSG::FATAL Error locating MessageSvc." << std::endl;
    exit(1);
  }

  // Set the output level by hand. DimHisto is not a Gaudi algorithm: Don't have properties to be set by options
  // To be fixed.
  m_msgsvc->setOutputLevel(MSG::INFO);
  MsgStream msg(m_msgsvc, "DimHisto");

  msg << MSG::DEBUG << "MessageSvc located." << endreq;


  int size;
  if( 0 != dynamic_cast<const AIDA::IProfile1D * >(InfoVar) ) {
    int nXBins = (dynamic_cast<const AIDA::IProfile1D*>(InfoVar))->axis().bins();
    // 1d profile data: dimension,nXBins,xMin,xMax,entries,3*(UNDERFLOW,"in range" bins, OVERFLOW): entries, weights and squares of weights 
    size = (5+3*(nXBins+2))*sizeof(float);
  }
  else if( 0 != dynamic_cast<const AIDA::IHistogram1D * >(InfoVar) ) {
    int nXBins = (dynamic_cast<const AIDA::IHistogram1D*>(InfoVar))->axis().bins();
    // 1Hd data: dimension,nXBins,xMin,xMax,entries,2*(UNDERFLOW,"in range" bins, OVERFLOW): entries and errors
    size = (5+2*(nXBins+2))*sizeof(float);
  }
  else if( 0 != dynamic_cast<const AIDA::IHistogram2D * >(InfoVar) ) {
    int nXBins = (dynamic_cast<const AIDA::IHistogram2D*>(InfoVar))->xAxis().bins();
    int nYBins = (dynamic_cast<const AIDA::IHistogram2D*>(InfoVar))->yAxis().bins();
    // 2Hd data: dimension,nXBins,xMin,xMax,nYBins,yMin,yMax,entries,
    // 2*((UNDERFLOW,"in range" bins, OVERFLOW)_x*(UNDERFLOW,"in range" bins,OVERFLOW)_y)
    size = (8+2*(nXBins+2)*(nYBins+2))*sizeof(float);
  }
  else {
    msg << MSG::ERROR << "Unknown histogram type: " << hname << endreq;
    return;
  }
  
  m_dimHistoScv = new DimHistoService(hname, InfoVar, size, svclocator);
  
}

//destructor
DimHisto::~DimHisto() {
    delete m_dimHistoScv;
}


//DimHistoService constructor
DimHistoService::DimHistoService(std::string hname, const AIDA::IBaseHistogram* InfoVar, 
                                 int size, ISvcLocator* svclocator) 
  : DimService(hname.c_str(),"F",m_data,size),
    m_size(size)
    ,m_data(0)
    ,m_h1D(0)
    ,m_h2D(0)
    ,m_hprof(0)
{
  // get msg logging
  StatusCode sc = svclocator->service("MessageSvc", m_msgsvc);
  if ( sc.isFailure() ) {
    std::cout << hname << "\tMSG::FATAL Error locating MessageSvc." << std::endl;
    exit(1);
  }

  // Set the output level by hand. DimHisto is not a Gaudi algorithm: Don't have properties to be set by options
  // To be fixed.
  m_msgsvc->setOutputLevel(MSG::INFO);
  MsgStream msg(m_msgsvc, "DimHisto");

  msg << MSG::DEBUG << "MessageSvc located." << endreq;

  m_name = hname;
  m_data = new float[size];
  m_dimension = InfoVar->dimension();
  
  m_h1D = (dynamic_cast<const AIDA::IHistogram1D*>(InfoVar));
  if( 0 != m_h1D ) {
    m_nXBins = (m_h1D->axis()).bins();
    m_nYBins =0;
    m_data[0]=m_dimension;
    m_data[1]=m_nXBins;
    m_data[2]=(m_h1D->axis()).lowerEdge();
    m_data[3]=(m_h1D->axis()).upperEdge();
    m_data[4]=0;
    for (int i=5; i<2*(m_nXBins+2); i++) m_data[i]=0.0;
  }
  m_h2D = (dynamic_cast<const AIDA::IHistogram2D*>(InfoVar));
  if( 0 != m_h2D ) {
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
  m_hprof = (dynamic_cast<const AIDA::IProfile1D*>(InfoVar));
  if( 0 != m_hprof ) {
    m_nXBins = (m_hprof->axis()).bins();
    m_nYBins =0;
    m_data[0]=m_dimension;
    m_data[1]=m_nXBins;
    m_data[2]=(m_hprof->axis()).lowerEdge();
    m_data[3]=(m_hprof->axis()).upperEdge();
    m_data[4]=0;
    for (int i=5; i<3*(m_nXBins+2); i++) m_data[i]=0.0;
  }

  if( 0 == m_h1D &&  0 == m_h2D &&  0 == m_hprof )
    msg << MSG::DEBUG << "Unable to instantiate DimHistoService " <<  hname << endmsg;
  else { 
    msg << MSG::DEBUG << "Successfully instantiated DimHistoService " <<  hname << endmsg;
    msg << MSG::DEBUG << "Dimension: " << m_data[0] << " nXBins=" << m_nXBins << "  nYBins=" << m_nYBins 
        << " data size=" << m_size/sizeof(float) << " elements(" << m_size << " bytes)" << endmsg;
  }
}

//destructor
DimHistoService::~DimHistoService() {  
  if(!m_data) delete [] m_data;
}

// Overloaded method serviceHandler gets called whenever commands arrive, 
void DimHistoService::serviceHandler() {
  MsgStream msg(m_msgsvc, "DimHisto::serviceHandler");
  int iData;
  // Update DimHisto data
  
  msg << MSG::DEBUG  << "AIDA::IHistogram" << m_name 
      << ": UNDERFLOW bin= " << AIDA::IAxis::UNDERFLOW_BIN 
      << " OVERFLOW bin=" << AIDA::IAxis::OVERFLOW_BIN << endmsg;

  if ( 0 != m_h1D ){
    //Sum of the entries in all the IHistogram's bins, i.e in-range bins, UNDERFLOW and OVERFLOW. 
    //This is equivalent to the number of times the method fill was invoked.
		m_data[4]= m_h1D->allEntries();
    msg << MSG::DEBUG  << "h1D " << m_name 
        << ": Entries: " << m_h1D->allEntries() << endmsg;
    
    // Entries (or should be height=entry*weight ?)
    iData = 5;
		m_data[iData++]= m_h1D->binHeight(AIDA::IAxis::UNDERFLOW_BIN);
    msg << MSG::DEBUG  << "h1D " << m_name << ": ixBin = " << "UNDERFLOW"
        << " iData = " << iData-1 
        << " binHeight = " << m_h1D->binHeight(AIDA::IAxis::UNDERFLOW_BIN) << endmsg;		
    for (int ixBin=0; ixBin<m_nXBins; ixBin++){
			m_data[iData++]= m_h1D->binHeight(ixBin);
      msg << MSG::DEBUG  << "h1D " << m_name << ": ixBin = " << ixBin
		      << " iData = " << iData-1 
		      << " binHeight = " << m_h1D->binHeight(ixBin) << endmsg;
    }
		m_data[iData++]= m_h1D->binHeight(AIDA::IAxis::OVERFLOW_BIN);
    msg << MSG::DEBUG  << "h1D " << m_name << ": ixBin = " << "OVERFLOW"
        << " iData = " << iData-1 
        << " binHeight = " << m_h1D->binHeight(AIDA::IAxis::OVERFLOW_BIN) << endmsg;
		
		// Errors
		m_data[iData++]= m_h1D->binError(AIDA::IAxis::UNDERFLOW_BIN);
    msg << MSG::DEBUG  << "h1D " << m_name << ": ixBin = " << "UNDERFLOW"
        << " iData = " << iData-1 
        << " binError = " << m_h1D->binError(AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
    for (int ixBin=0; ixBin<m_nXBins; ixBin++){		
			m_data[iData++]= m_h1D->binError (ixBin);
      msg << MSG::DEBUG  << "h1D " << m_name << ": ixBin = " << ixBin
		      << " iData = " << iData-1 
		      << " binError = " << m_h1D->binError(ixBin) << endmsg;
    }
		m_data[iData++]= m_h1D->binError(AIDA::IAxis::OVERFLOW_BIN);
    msg << MSG::DEBUG  << "h1D " << m_name << ": ixBin = " << "OVERFLOW"
        << " iData = " << iData-1 
        << " binError = " << m_h1D->binError(AIDA::IAxis::OVERFLOW_BIN) << endmsg;
  }

  if ( 0 != m_h2D ){
    //Sum of the entries in all the IHistogram's bins, i.e in-range bins, UNDERFLOW and OVERFLOW. 
    //This is equivalent to the number of times the method fill was invoked.
		m_data[7]= m_h2D->allEntries();
    msg << MSG::DEBUG  << "h2D " << m_name 
        << ": Entries: " << m_h2D->allEntries() << endmsg;
		
    // Entries (in fact, height=entry*weight ?)
    iData = 8;
		m_data[iData++]= m_h2D->binHeight(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN);
    msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << "UNDERFLOW"
        << " iyBin = " << "UNDERFLOW" << " iData = " << iData-1 
        <<" binHeight = " << m_h2D->binHeight(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
		for (int iyBin=0; iyBin<m_nYBins; iyBin++){
      m_data[iData++]= m_h2D->binHeight(AIDA::IAxis::UNDERFLOW_BIN,iyBin);
      msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << "UNDERFLOW"
		      << " iyBin = " << iyBin << " iData = " << iData-1 
		      << " binHeight = " << m_h2D->binHeight(AIDA::IAxis::UNDERFLOW_BIN,iyBin) << endmsg;
		}
		m_data[iData++]= m_h2D->binHeight(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN);
    msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << "UNDERFLOW"
        << " iyBin = " << "OVERFLOW" << " iData = " << iData-1 
        << " binHeight = " << m_h2D->binHeight(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN) << endmsg;
    for (int ixBin=0; ixBin<m_nXBins; ixBin++){		
      m_data[iData++]= m_h2D->binHeight(ixBin,AIDA::IAxis::UNDERFLOW_BIN);
      msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << ixBin
		      << " iyBin = " << "UNDERFLOW" << " iData = " << iData-1 
		      << " binHeight = " << m_h2D->binHeight(ixBin,AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
      for (int iyBin=0; iyBin<m_nYBins; iyBin++){
        m_data[iData++]= m_h2D->binHeight(ixBin,iyBin);
        msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << ixBin
            << " iyBin = " << iyBin << " iData = " << iData-1 
            << " binHeight = " << m_h2D->binHeight(ixBin,iyBin) << endmsg;
      }
      m_data[iData++]= m_h2D->binHeight(ixBin,AIDA::IAxis::OVERFLOW_BIN);
      msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << ixBin
		      << " iyBin = " << "OVERFLOW" << " iData = " << iData-1 
		      << " binHeight = " << m_h2D->binHeight(ixBin,AIDA::IAxis::OVERFLOW_BIN) << endmsg;
    }
    
		m_data[iData++]= m_h2D->binHeight(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN);
    msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << "OVERFLOW"
        << " iyBin = " << "UNDERFLOW" << " iData = " << iData-1 
        << " binHeight = " << m_h2D->binHeight(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
		for (int iyBin=0; iyBin<m_nYBins; iyBin++){
      m_data[iData++]= m_h2D->binHeight(AIDA::IAxis::OVERFLOW_BIN,iyBin);
      msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << "OVERFLOW"
		      << " iyBin = " << iyBin << " iData = " << iData-1 
		      << " binHeight = " << m_h2D->binHeight(AIDA::IAxis::OVERFLOW_BIN,iyBin) << endmsg;
		}
    m_data[iData++]= m_h2D->binHeight(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN);
    msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << "OVERFLOW"
        << " iyBin = " << "OVERFLOW" << " iData = " << iData-1 
        << " binHeight = " << m_h2D->binHeight(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN) << endmsg;
    
    // Errors
		m_data[iData++]= m_h2D->binError(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN);
    msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << "UNDERFLOW"
        << " iyBin = " << "UNDERFLOW" << " iData = " << iData-1 
        << " binError = " << m_h2D->binError(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
		for (int iyBin=0; iyBin<m_nYBins; iyBin++){
      m_data[iData++]= m_h2D->binError(AIDA::IAxis::UNDERFLOW_BIN,iyBin);
      msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << "UNDERFLOW"
		      << " iyBin = " << iyBin << " iData = " << iData-1 
		      << " binError = " << m_h2D->binError(AIDA::IAxis::UNDERFLOW_BIN,iyBin) << endmsg;
		}
		m_data[iData++]= m_h2D->binError(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN);
    msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << "UNDERFLOW"
        << " iyBin = " << "OVERFLOW" << " iData = " << iData-1 
        << " binError = " << m_h2D->binError(AIDA::IAxis::UNDERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN) << endmsg;
    for (int ixBin=0; ixBin<m_nXBins; ixBin++){		
      m_data[iData++]= m_h2D->binError(ixBin,AIDA::IAxis::UNDERFLOW_BIN);
      msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << ixBin
		      << " iyBin = " << "UNDERFLOW" << " iData = " << iData-1 
		      << " binError = " << m_h2D->binError(ixBin,AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
      for (int iyBin=0; iyBin<m_nYBins; iyBin++){
        m_data[iData++]= m_h2D->binError(ixBin,iyBin);
        msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << ixBin
            << " iyBin = " << iyBin << " iData = " << iData-1 
            << " binError = " << m_h2D->binError(ixBin,iyBin) << endmsg;
      }
      m_data[iData++]= m_h2D->binError(ixBin,AIDA::IAxis::OVERFLOW_BIN);
      msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << ixBin
		      << " iyBin = " << "OVERFLOW" << " iData = " << iData-1 
		      << " binError = " << m_h2D->binError(ixBin,AIDA::IAxis::OVERFLOW_BIN) << endmsg;
    }
    
		m_data[iData++]= m_h2D->binError(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN);
    msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << "OVERFLOW"
        << " iyBin = " << "UNDERFLOW" << " iData = " << iData-1 
        << " binError = " << m_h2D->binError(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::UNDERFLOW_BIN) << endmsg;
		for (int iyBin=0; iyBin<m_nYBins; iyBin++){
      m_data[iData++]= m_h2D->binError(AIDA::IAxis::OVERFLOW_BIN,iyBin);
      msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << "OVERFLOW"
		      << " iyBin = " << iyBin << " iData = " << iData-1 
		      << " binError = " << m_h2D->binError(AIDA::IAxis::OVERFLOW_BIN,iyBin) << endmsg;
		}
    m_data[iData++]= m_h2D->binError(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN);
    msg << MSG::DEBUG  << "h2D " << m_name << ": ixBin = " << "OVERFLOW"
        << " iyBin = " << "OVERFLOW" << " iData = " << iData-1 
        << " binError = " << m_h2D->binError(AIDA::IAxis::OVERFLOW_BIN,AIDA::IAxis::OVERFLOW_BIN) << endmsg;
    
 	} 

  if( 0 != m_hprof ) {
    //Sum of the entries in all the IHistogram's bins, i.e in-range bins, UNDERFLOW and OVERFLOW.
    //This is equivalent to the number of times the method fill was invoked.
    m_data[4]= m_hprof->allEntries();
    msg << MSG::DEBUG  << "hprof " << m_name
        << ": Entries: " << m_hprof->allEntries() << endmsg;

    // AIDA::IProfile1D doesn't provide the sum of the squares of the weights.
    // It has been recalculated from the #entries, the rms and the mean (sum of eights)
    // Assuming the relation rms^2 = sum(y^2)/entries - (sum(y)/entries)^2
    // Also assuming all entries have weight = 1

    double entries, mean, rms;
    double sumOfWeights, sumOfSquaredWeights;

    // Initial indices in m_data array
    int iDataEntries = 5;
    int iDataSumOfWeights = iDataEntries+m_nXBins+2;
    int iDataSumOfSquaredWeights = iDataSumOfWeights+m_nXBins+2;

    // Underflow bins
    entries = m_hprof->binEntries(AIDA::IAxis::UNDERFLOW_BIN);
    mean    = m_hprof->binHeight(AIDA::IAxis::UNDERFLOW_BIN);
    rms     = m_hprof->binRms(AIDA::IAxis::UNDERFLOW_BIN);

    sumOfWeights = mean*entries;
    sumOfSquaredWeights = (rms*rms + mean*mean)*entries;
    
    m_data[iDataEntries++] = entries;
    m_data[iDataSumOfWeights++] = sumOfWeights;
    m_data[iDataSumOfSquaredWeights++] = sumOfSquaredWeights;

    msg << MSG::DEBUG  << "hprof " << m_name << ": ixBin = " << "UNDERFLOW"
        << " iDataEntries = " << iDataEntries-1 << " binEntries = " << entries << " binHeight = " << mean << " binRms = " << rms
        << " binError = " << m_hprof->binError(AIDA::IAxis::UNDERFLOW_BIN)
        << " iDataSumOfWeights = " << iDataSumOfWeights-1 << " sumOfWeights = " << sumOfWeights
        << " iDataSumOfSquaredWeights = " << iDataSumOfSquaredWeights-1 << " sumOfSquaredWeights = " << sumOfSquaredWeights 
        << endmsg;

    // The "in range" bins
    for (int ixBin=0; ixBin<m_nXBins; ixBin++){
      entries = m_hprof->binEntries( ixBin );
      mean    = m_hprof->binHeight( ixBin );
      rms     = m_hprof->binRms( ixBin );

      sumOfWeights = mean*entries;
      sumOfSquaredWeights = (rms*rms + mean*mean)*entries;
   
      m_data[iDataEntries++] = entries;
      m_data[iDataSumOfWeights++] = sumOfWeights;
      m_data[iDataSumOfSquaredWeights++] = sumOfSquaredWeights;

      msg << MSG::DEBUG  << "hprof " << m_name << ": ixBin = " << ixBin
          << " iDataEntries = " << iDataEntries-1 << " binEntries = " << entries << " binHeight = " << mean << " binRms = " << rms
          << " binError = " << m_hprof->binError(ixBin)
          << " iDataSumOfWeights = " << iDataSumOfWeights-1 << " sumOfWeights = " << sumOfWeights
          << " iDataSumOfSquaredWeights = " << iDataSumOfSquaredWeights-1 << " sumOfSquaredWeights = " << sumOfSquaredWeights 
          << endmsg;

    }

    // The overflow bins
    entries = m_hprof->binEntries(AIDA::IAxis::OVERFLOW_BIN);
    mean    = m_hprof->binHeight(AIDA::IAxis::OVERFLOW_BIN);
    rms     = m_hprof->binRms(AIDA::IAxis::OVERFLOW_BIN);

    sumOfWeights = mean*entries;
    sumOfSquaredWeights = (rms*rms + mean*mean)*entries;
   
    m_data[iDataEntries++] = entries;
    m_data[iDataSumOfWeights++] = sumOfWeights;
    m_data[iDataSumOfSquaredWeights++] = sumOfSquaredWeights;

    msg << MSG::DEBUG  << "hprof " << m_name << ": ixBin = " << "OVERFLOW"
        << " iDataEntries = " << iDataEntries-1 << " binEntries = " << entries << " binHeight = " << mean << " binRms = " << rms
        << " binError = " << m_hprof->binError(AIDA::IAxis::OVERFLOW_BIN)
        << " iDataSumOfWeights = " << iDataSumOfWeights-1 << " sumOfWeights = " << sumOfWeights
        << " iDataSumOfSquaredWeights = " << iDataSumOfSquaredWeights-1 << " sumOfSquaredWeights = " << sumOfSquaredWeights
        << endmsg;
 

  }

  setData(m_data,m_size);
  return;
}

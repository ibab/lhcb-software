
//  ====================================================================
//  MEPInjector.h
//  --------------------------------------------------------------------
//
//  Author    : David Svantesson
//
//  ====================================================================
#ifndef MEP_MEPINJECTOR_H
#define MEP_MEPINJECTOR_H

#include "Event/RawEvent.h"
#include "MDF/MDFWriter.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IRunable.h"

#include <map>

/*
 *    LHCb namespace
 */
namespace LHCb {
 
 /** @class MEPInjector MEPInjector.cpp
  *  Creates and injects MEPs from MDF files. 
  *
  *  @author David Svantesson
  *  @date   2008-06-26
  */
  class MEPInjector : public Algorithm {
   private:  
    /// MEP Event Packing Factor
    int           m_packingFactor;
    /// Option to remove DAQStatus (should be true)
    bool          m_removeDAQStatus;
    
    int           m_IPProtoIn;
    
    int           m_mepSock;    /* Raw socket to send MEPs.*/
    int           m_sockBuf;  
    int		  m_ethInterface;    
  
    std::string   m_IPAddrTo;        //IP Address to send to
    
    std::vector< std::string > m_IPAddrFrom;   //IP Addresses to send FROM 
    
    //int m_probEventID;
    //int m_probPartID;
    //int m_probSize;
    
    
    
    
    /// Event ID
    int m_evID;
    
    std::map<unsigned int, RawEvent*> m_events;
    StatusCode sendMEP();
    
    //int openSocket(int protocol);    
    StatusCode error(const std::string& msg);
    
    void printMEP(MEPEvent * me, int size);
    
   public: 
    /// Standard constructor
    MEPInjector( const std::string& name, ISvcLocator* pSvcLocator );

    /// Destructor
    virtual ~MEPInjector()  {} 

    /// Initialize 
    virtual StatusCode initialize();
    
    /// Main execution
    virtual StatusCode execute();     
    
    /// Streambuffer to hold uncompressed data. For encoding MEPs
    StreamBuffer  m_data;
  };


} // namespace LHCb

#endif // MEP_MEPINJECTOR_H

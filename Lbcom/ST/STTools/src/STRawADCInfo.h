// $Id: STRawADCInfo.h,v 1.1 2009-08-21 17:09:55 mneedham Exp $
#ifndef STRawADCInfo_H
#define STRawADCInfo_H 1

// ST tool base class
#include "Kernel/STToolBase.h"
#include "GaudiKernel/IIncidentListener.h"

#include "Event/STTELL1Data.h"

// LHCbKernel
#include "Kernel/ISTRawADCInfo.h"

/** @class STSelectChannelIDByElement STSelectChannelIDByElement.h
 *
 *  Tool for selecting clusters using the conditions
 *
 *  @author M.Needham
 *  @date   08/08/2009
 */

namespace LHCb{
  class STChannelID;
}

class STRawADCInfo: public ST::ToolBase, 
                         virtual public ISTRawADCInfo,
                         virtual public IIncidentListener {

 public: 
   
  /// constructer
  STRawADCInfo( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /** intialize */
  StatusCode initialize();

  /// destructer
  virtual ~STRawADCInfo();
   
  virtual StatusCode link(const LHCb::STChannelID& chan, unsigned int& value) const ;

  virtual StatusCode link(const std::vector<LHCb::STChannelID>& chans, 
                  std::vector<unsigned int>& values) const;

  virtual StatusCode link(const LHCb::STChannelID& chan, 
                          const unsigned int window,
                          std::vector<unsigned int>& values) const ;

  /** Implement the handle method for the Incident service.
  *  This is used to nform the tool of software incidents.
  *
  *  @param incident The incident identifier
  */
  void handle( const Incident& incident );

private:

  void initEvent() const; 

  mutable bool m_configured;
  std::string m_dataLocation;
  mutable LHCb::STTELL1Datas* m_data;

};

#endif // STSelectChannelIDByElement_H

#include <iostream>
#include <TString.h>
#include <TPRegexp.h>
#include <TObjString.h>
#include <TObjArray.h>

#include "presenter.h"
#include "HistogramIdentifier.h"

using namespace pres;

HistogramIdentifier::HistogramIdentifier(const std::string & histogramUrl):
  m_histogramUrlTS(histogramUrl.c_str()),
  m_identifier(""),
  m_histogramType(""),
  m_partitionName(""),
  m_nodeName(""),
  m_taskName(""),
  m_instanceOnNode(""),
  m_histogramUTGID(""),
  m_algorithmName(""),
  m_isFromHistogramSet(false),
  m_isEFF(false),
  m_setName(""),
  m_histogramName(""),
  m_isPlausible(false),
  m_isDimFormat(false),
  m_gauchocommentBeat(""),
  m_gauchocommentEric(""),
  m_dbDimServiceName("")
{
  setIdentifiersFromDim(m_histogramUrlTS.Data());
}
HistogramIdentifier::~HistogramIdentifier()
{
}

void HistogramIdentifier::setIdentifiersFromDim(std::string newDimServiceName)
{
// groups:
//  $0 matches magic, returns histogramUrl
//  $1 histogram type
//  $2 m_histogramUTGID
//    $1 partition name
//    $2 node name
//    $3 taskname
//    $4 instance on node
//  $3 algorithmname
//  $4 full histogram name
//  $5 histogramSetName or histogramName
//  $6 set switch: is "_$"
//  $7 histogram name

  m_histogramUrlTS = newDimServiceName.c_str();
  int matchOffset(0);

  TObjArray* histogramUrlMatchGroup = s_histogramUrlRegexp.MatchS(m_histogramUrlTS);

  if (!histogramUrlMatchGroup->IsEmpty()) {
    m_histogramType = (((TObjString *)histogramUrlMatchGroup->At(1))->
      GetString()).Data();
    if (m_histogramType.empty()) {
      m_histogramType = s_CNT;
    } else if (m_histogramUrlTS.BeginsWith(s_CNT.c_str())) {
      m_histogramType = s_CNT;
      m_histogramUrlTS.Remove(0, s_CNT.length()+1);
    }
    m_histogramUTGID = (((TObjString *)histogramUrlMatchGroup->At(2))->
      GetString()).Data(); 

    TObjArray* histogramUTGIDMatchGroup = s_histogramUTGIDRegexp.MatchS(m_histogramUTGID);

    if (!histogramUTGIDMatchGroup->IsEmpty()) {
      m_partitionName = (((TObjString *)histogramUTGIDMatchGroup->At(1))->
                        GetString()).Data();
      if ( 4 == histogramUTGIDMatchGroup->GetLast() ) {
        m_instanceOnNode = (((TObjString *)histogramUTGIDMatchGroup->At(4))->
                             GetString()).Data();
        m_nodeName = (((TObjString *)histogramUTGIDMatchGroup->At(2))->
                     GetString()).Data();
        m_taskName = (((TObjString *)histogramUTGIDMatchGroup->At(3))->
                     GetString()).Data();
      } else if ( 3 == histogramUTGIDMatchGroup->GetLast()) {
        m_isEFF = true;
        m_nodeName = "";
        m_taskName = (((TObjString *)histogramUTGIDMatchGroup->At(2))->
                     GetString()).Data();
      } else {
        m_taskName = m_histogramUTGID;
      }
    }
    if (histogramUTGIDMatchGroup) {
      histogramUTGIDMatchGroup->Delete();
      delete histogramUTGIDMatchGroup;
    }
    m_algorithmName = (((TObjString *)histogramUrlMatchGroup->At(3 + matchOffset))->
      GetString()).Data();
      
    TString setSwitch = ((TObjString *)histogramUrlMatchGroup->At(6 + matchOffset))->
      GetString();

    if (s_setSwitch == setSwitch) {
      m_isFromHistogramSet = true;
      m_setName = (((TObjString *)histogramUrlMatchGroup->At(5 + matchOffset))->
        GetString()).Data();
      m_histogramName = (((TObjString *)histogramUrlMatchGroup->At(7 + matchOffset))->
        GetString()).Data();
    } else if ("" == setSwitch) {
      m_isFromHistogramSet = false;
      m_setName = "";
      m_histogramName = (((TObjString *)histogramUrlMatchGroup->At(4 + matchOffset))->
        GetString()).Data();
    }
    m_isPlausible = true;                    
    if (m_isPlausible && m_histogramType != "") {
      m_isDimFormat = true;
    }
    m_gauchocommentBeat = newDimServiceName + s_gauchocomment;
    m_gauchocommentEric = newDimServiceName.substr(m_histogramType.length() + 1,
                                                   newDimServiceName.length())
                                                   + s_gauchocomment;
  }
  
  histogramUrlMatchGroup->Delete();
  delete histogramUrlMatchGroup;
  
  if (m_isFromHistogramSet) {
    m_identifier =  m_taskName + s_slash + m_algorithmName +
                    s_slash + m_setName + s_setSwitch + m_histogramName;    
  } else {
    m_identifier =  m_taskName + s_slash + m_algorithmName +
                    s_slash + m_histogramName;    
  }
  
  m_dbDimServiceName = m_histogramType + s_slash + m_identifier;
  
//   typedef enum { H1D, H2D, P1D, P2D, CNT, SAM} HistType;  
  if (s_H1D == m_histogramType) {
    m_dbHistogramType = 0;
  } else if (s_H2D == m_histogramType) {
    m_dbHistogramType = 1;
  } else if ( (s_P1D == m_histogramType) ||
              (s_HPD == m_histogramType) ) {
    m_dbHistogramType = 2;
  } else if (s_CNT == m_histogramType) {
    m_dbHistogramType = 4;
  } 

  m_lastName = m_histogramName.substr((m_histogramName.find_last_of(s_slash,
                                      std::string::npos)-std::string::npos),
                                      m_histogramName.length());  
  
  if (m_isEFF) {
    TString fileName(m_lastName);
    fileName.ReplaceAll("_", "_001_");
    fileName.ReplaceAll(",", "_002_");
    fileName.ReplaceAll(">", "_003_");
    fileName.ReplaceAll("<", "_004_");
    fileName.ReplaceAll("&", "_005_");
    m_fileName = fileName;
  }
}
#include <iostream>
#include <TString.h>
#include <TPRegexp.h>
#include <TObjString.h>
#include <TObjArray.h>

#include "HistogramIdentifier.h"

HistogramIdentifier::HistogramIdentifier(std::string histogramUrl):
  m_histogramUrlTS(histogramUrl.c_str()),
  m_histogramUrlRegexp("^(H1D|H2D|P1D|HPD|P2D|CNT)?/?([^/_]+_)?([^/_]*)(_[^/]*)?/([^/]*)/(([^_]*)(_\\$)?(.*))$"),
  m_identifier(""),
  m_histogramType(""),
  m_histogramUTGID(""),
  m_histogramUTGIDprefix(""),
  m_taskName(""),
  m_histogramUTGIDpostfix(""),
  m_algorithmName(""),
  m_isFromHistogramSet(false),
  m_setName(""),
  m_histogramName(""),
  m_isPlausible(false),
  m_isDimFormat(false),
  m_gauchocommentBeat(""),
  m_gauchocommentEric("")
{
// ^(H1D|H2D|P1D|HPD|P2D|CNT)?/?(([^/_]+_)?([^/_]*)(_[^/]*)?)?/([^/]*)/(([^_]*)(_\$)?(.*))$
// test cases:
//  CC-PC/CCPCAlg/TELL1Mult3_$T3/L2/Q1/myTell1
//  H1D/node00101_taskname_1/CCPCAlg/TELL1Mult3_$T3/L2/Q1/myTell1
//  H2D/node00101_taskname_1/CCPCAlg/TELL1Mult3_$T3/L2/Q1/myTell1
//  CC-PC/CCPCAlg/TELL1Mult1_$T3/L2/Q0/myTell1
//  CC-PC/er_CCPCAlg/TELL1Mult1_$T3/L2/Q0/myTell1
//  CC-PC/CCPCAlg/TELL1Mult1_1//sdf/sf/sCC-PC/CCPCAlg/myTell1
//  CC-PC/CCPCAlg/T3/L2/Q1/myTell1
// groups:
//  $0 matches magic
//  $1 histogram type
//  $. UTGID
//  $2 UTGID prefix
//  $3 taskname
//  $4 UTGID postfix
//  $5 algorithmname
//  $6 full histogram name
//  $7 histogramSetName or histogramName
//  $8 set switch: is "_$" if $3 is histogramSetName "" if histogramName
//  $9 histogram name

  TObjArray* histogramUrlMatchGroup = m_histogramUrlRegexp.
    MatchS(m_histogramUrlTS);

  if (!histogramUrlMatchGroup->IsEmpty()) {
    m_histogramType = (((TObjString *)histogramUrlMatchGroup->At(1))->
      GetString()).Data();
    m_histogramUTGIDprefix = (((TObjString *)histogramUrlMatchGroup->At(2))->
      GetString()).Data();
    m_taskName = (((TObjString *)histogramUrlMatchGroup->At(3))->
      GetString()).Data();
    m_histogramUTGIDpostfix = (((TObjString *)histogramUrlMatchGroup->
      At(4))->GetString()).Data();
    m_histogramUTGID = m_histogramUTGIDprefix + m_taskName +
      m_histogramUTGIDpostfix;
    m_algorithmName = (((TObjString *)histogramUrlMatchGroup->At(5))->
      GetString()).Data();
    m_isPlausible = true;
    TString setSwitch = ((TObjString *)histogramUrlMatchGroup->At(8))->
      GetString();

    if ("_$" == setSwitch) {
      m_isFromHistogramSet = true;
      m_setName = (((TObjString *)histogramUrlMatchGroup->At(7))->
        GetString()).Data();
      m_histogramName = (((TObjString *)histogramUrlMatchGroup->At(9))->
        GetString()).Data();
    } else if ("" == setSwitch) {
      m_isFromHistogramSet = false;
      m_setName = "";
      m_histogramName = (((TObjString *)histogramUrlMatchGroup->At(7))->
        GetString()).Data();
    }

    m_identifier = m_histogramType + "/" + m_taskName + "/" + m_algorithmName
      + "/" + (((TObjString *)histogramUrlMatchGroup->At(6))->GetString()).
      Data();
    if (m_isPlausible ) { // && m_histogramType != ""
      m_isDimFormat = true;
    }
    m_gauchocommentBeat = histogramUrl + "/gauchocomment";
    m_gauchocommentEric = histogramUrl.erase(0, m_histogramType.length()+1) +
      "/gauchocomment";
  }
  histogramUrlMatchGroup->Delete();
  delete histogramUrlMatchGroup;
}
HistogramIdentifier::~HistogramIdentifier()
{
}

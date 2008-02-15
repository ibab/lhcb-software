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
// groups:
//  $0 matches magic, returns histogramUrl
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

  TObjArray* histogramUrlMatchGroup = s_histogramUrlRegexp.
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

    if (s_setSwitch == setSwitch) {
      m_isFromHistogramSet = true;
      m_setName = (((TObjString *)histogramUrlMatchGroup->At(7))->
        GetString()).Data();
      m_histogramName = (((TObjString *)histogramUrlMatchGroup->At(9))->
        GetString()).Data();
    } else if ("" == setSwitch) {
      m_isFromHistogramSet = false;
      m_setName = "";
      m_histogramName = (((TObjString *)histogramUrlMatchGroup->At(6))->
        GetString()).Data();
    }
    m_identifier =  m_taskName + s_slash + m_algorithmName +
                    s_slash + (((TObjString *)histogramUrlMatchGroup->
                    At(6))->GetString()).Data();
    if (m_isPlausible && m_histogramType != "") {
      m_isDimFormat = true;
    }
    m_gauchocommentBeat = histogramUrl + s_gauchocomment;
    m_gauchocommentEric = histogramUrl.substr(m_histogramType.length() + 1,
                                              histogramUrl.length())
                          + s_gauchocomment;
  }
  histogramUrlMatchGroup->Delete();
  delete histogramUrlMatchGroup;
}
HistogramIdentifier::~HistogramIdentifier()
{
}

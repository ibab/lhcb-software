#ifndef HISTOGRAMIDENTIFIER_H_
#define HISTOGRAMIDENTIFIER_H_

class TString;
#include <TPRegexp.h>

// Class to handle histogram identifier parsing / splitting
class HistogramIdentifier
{
  public:
    HistogramIdentifier(std::string histogramUrl);
    virtual ~HistogramIdentifier();

    std::string histogramIdentifier() { return m_identifier; };
    std::string histogramType() { return m_histogramType; };
    std::string histogramUTGID() { return m_histogramUTGID; };
    std::string histogramUTGIDprefix() { return m_histogramUTGIDprefix; };
    std::string taskName() {return m_taskName; };
    std::string histogramUTGIDpostfix() { return m_histogramUTGIDpostfix; };
    std::string algorithmName() {return m_algorithmName; };
    bool isFromHistogramSet() {return m_isFromHistogramSet; };
    std::string histogramSetName() { return m_setName; };
    std::string histogramName() { return m_histogramName; };
    bool isPlausible() { return m_isPlausible; };
    bool isDimFormat() { return m_isDimFormat; };
    std::string gauchocommentBeat() { return m_gauchocommentBeat; };
    std::string gauchocommentEric() { return m_gauchocommentEric; };

  protected:
    TString m_histogramUrlTS;
    TPRegexp m_histogramUrlRegexp;
    std::string m_identifier;
    std::string m_histogramType;
    std::string m_histogramUTGID;
    std::string m_histogramUTGIDprefix;
    std::string m_taskName;
    std::string m_histogramUTGIDpostfix;
    std::string m_algorithmName;
    bool m_isFromHistogramSet;
    std::string m_setName;
    std::string m_histogramName;
    bool m_isPlausible;
    bool m_isDimFormat;
    std::string m_gauchocommentBeat;
    std::string m_gauchocommentEric;
};

#endif /*HISTOGRAMIDENTIFIER_H_*/

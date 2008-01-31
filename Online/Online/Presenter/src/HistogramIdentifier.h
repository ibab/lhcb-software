#ifndef HISTOGRAMIDENTIFIER_H_
#define HISTOGRAMIDENTIFIER_H_

class TString;
//#include <TPRegexp.h>

// Class to handle histogram identifier parsing / splitting
class HistogramIdentifier
{
  public:
    HistogramIdentifier(const std::string & histogramUrl);
    virtual ~HistogramIdentifier();

    std::string histogramIdentifier() const { return m_identifier; };
    std::string histogramType() const { return m_histogramType; };
    std::string histogramUTGID() const { return m_histogramUTGID; };
    std::string histogramUTGIDprefix() const { return m_histogramUTGIDprefix; };
    std::string taskName() const {return m_taskName; };
    std::string histogramUTGIDpostfix() const { return m_histogramUTGIDpostfix; };
    std::string algorithmName() const {return m_algorithmName; };
    bool isFromHistogramSet() const {return m_isFromHistogramSet; };
    std::string histogramSetName() const { return m_setName; };
    std::string histogramName() const { return m_histogramName; };
    bool isPlausible() const { return m_isPlausible; };
    bool isDimFormat() const { return m_isDimFormat; };
    std::string gauchocommentBeat() const { return m_gauchocommentBeat; };
    std::string gauchocommentEric() const { return m_gauchocommentEric; };

  protected:
    TString m_histogramUrlTS;
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

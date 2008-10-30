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
    
    void setIdentifiersFromDim(std::string newDimServiceName);

    std::string histogramIdentifier() const { return m_identifier; };
    std::string histogramType() const { return m_histogramType; };
    std::string partitionName() const { return m_partitionName; };
    std::string nodeName() const { return m_nodeName; };
    std::string taskName() const { return m_taskName; };
    std::string instanceOnNode() const { return m_instanceOnNode; };
    std::string histogramUTGID() const { return m_histogramUTGID; };
    std::string algorithmName() const {return m_algorithmName; };
    bool isFromHistogramSet() const {return m_isFromHistogramSet; };
    std::string histogramSetName() const { return m_setName; };
    std::string histogramName() const { return m_histogramName; };
    bool isPlausible() const { return m_isPlausible; };
    bool isDimFormat() const { return m_isDimFormat; };
    std::string gauchocommentBeat() const { return m_gauchocommentBeat; };
    std::string gauchocommentEric() const { return m_gauchocommentEric; };
    std::string m_histogramUrl() const { return m_histogramUrlTS.Data(); };

  protected:
    TString m_histogramUrlTS;
    std::string m_identifier;
    std::string m_histogramType;
    std::string m_partitionName;
    std::string m_nodeName;
    std::string m_taskName;
    std::string m_instanceOnNode;
    std::string m_histogramUTGID;
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

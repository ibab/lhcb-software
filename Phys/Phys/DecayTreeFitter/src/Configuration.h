#ifndef DECAYTREEFITTER_CONFIG_H
#define DECAYTREEFITTER_CONFIG_H

class ITrackExtrapolator ;

namespace DecayTreeFitter
{
  class Configuration
  {
  public:
    Configuration(bool forceFitAll=true, ITrackExtrapolator* stateprovider=0) :
      m_forceFitAll(forceFitAll),
      m_stateProvider(stateprovider) {}
    
    const ITrackExtrapolator* stateProvider() const { return m_stateProvider ; }
    
    bool forceFitAll() const { return m_forceFitAll ; }
    
  private:
    bool m_forceFitAll ;
    const ITrackExtrapolator* m_stateProvider ;
  } ;
}

#endif

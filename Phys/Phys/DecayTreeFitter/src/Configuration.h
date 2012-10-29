#ifndef DECAYTREEFITTER_CONFIG_H
#define DECAYTREEFITTER_CONFIG_H

class ITrackStateProvider ;

namespace DecayTreeFitter
{
  class Configuration
  {
  public:
  Configuration(bool forceFitAll=true, const ITrackStateProvider* stateprovider=0) 
    : m_forceFitAll(forceFitAll),
      m_stateprovider(stateprovider),
      m_useTrackTraj( true )
	{}
    
    const ITrackStateProvider* stateProvider() const { return m_stateprovider ; }
    
    bool forceFitAll() const { return m_forceFitAll ; }
    bool useTrackTraj() const { return m_stateprovider && m_useTrackTraj ; }
    
  private:
    bool m_forceFitAll ;
    const ITrackStateProvider* m_stateprovider ;
    bool m_useTrackTraj ;
  } ;
}

#endif

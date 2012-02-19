#ifndef PVFITTRACK_H
#define PVFITTRACK_H 1

class PVFitTrack
{
private:

  // pointer to the reference track
  const LHCb::Track* m_track;

  // State of the track at the current point
  LHCb::State m_state;

  // weight assigned to the track
  double m_weight;

  //fitted slope parameters
  ROOT::Math::SVector<double, 2> m_slopes;

public:

  PVFitTrack() : m_track(NULL), m_weight(0)
  {
  }

  explicit PVFitTrack(const LHCb::Track* track)
    : m_track(track),
      m_state(track->firstState()),
      m_weight(1.0),
      m_slopes(ROOT::Math::SVector<double, 2>(m_state.tx(), m_state.ty()) )
  {
  }

  inline const LHCb::Track* getTrack() const { return m_track; }

  inline const LHCb::State& getState() const { return m_state; }

  inline LHCb::State& getState() { return m_state; }

  inline double getWeight() const { return m_weight;  }

  inline const ROOT::Math::SVector<double,2>& getFittedSlopes() const 
  { 
    return m_slopes;  
  }

  void setTrack(const LHCb::Track* tr)
  {
    m_track  = tr;
    m_state  = tr->firstState();
    m_weight = 1.0;
    m_slopes = ROOT::Math::SVector<double, 2>(m_state.tx(), m_state.ty());
  }

  inline void setState(const LHCb::State& s ) { m_state = s;  }

  inline void setWeight(const double w) { m_weight = w;  }

  inline void setFittedSlopes(const ROOT::Math::SVector<double,2>& sl) 
  {
    m_slopes = sl;  
  }

};

#endif

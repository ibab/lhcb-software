#ifndef PVFITTRACK_H
#define PVFITTRACK_H 1

class PVFitTrack
{
private:

  // pointer to the reference track
  LHCb::Track* m_track;

  // State of the track at the current point
  LHCb::State m_state;

  // weight assigned to the track
  double m_weight;

  //fitted slope parameters
  ROOT::Math::SVector<double, 2> m_slopes;

public:

  inline LHCb::Track* getTrack() { return m_track;  }

  inline LHCb::State getState() { return m_state;  }

  inline double getWeight() { return m_weight;  }

  inline ROOT::Math::SVector<double,2> getFittedSlopes() { return m_slopes;  }

  void setTrack(LHCb::Track* tr)
  {
    m_track = tr;
    m_state = tr->firstState();
    m_weight = 1.0;
    m_slopes = ROOT::Math::SVector<double, 2>(m_state.tx(), m_state.ty());
  }

  inline void setState(LHCb::State& s ) { m_state = s;  }

  inline void setWeight(double w) { m_weight = w;  }

  inline void setFittedSlopes(ROOT::Math::SVector<double,2>& sl) { m_slopes = sl;  }
  
};

#endif

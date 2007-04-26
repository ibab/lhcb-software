#ifndef PVHISTO_H 
#define PVHISTO_H 1

// Include files
#include <vector>
#include "GaudiKernel/MsgStream.h"

#include "VeloTrack.h"

/** @class PVHisto PVHisto.h
 *  Histogram class for PV pre-search
 *
 *  @author Philippe Charpentier
 *  @date   2002-12-02
 */

class PVHisto {
public:
  typedef std::vector<int>::iterator iterator;
  /// Standard constructor
  PVHisto();
  PVHisto( double min, double max, int bins );
  PVHisto( double min, double max, double width );
  
  virtual ~PVHisto( ){}; ///< Destructor
  PVHisto::iterator fill( double val );
  PVHisto::iterator getbin( double val );
  inline PVHisto::iterator begin(){ return m_Histo.begin(); };
  inline PVHisto::iterator end(){ return m_Histo.end(); };
  void print( MsgStream& msg, MSG::Level lvl, PVHisto::iterator& start, PVHisto::iterator& end );
  void reset();
  inline double maxVal() { return m_MaxVal; };
  inline PVHisto::iterator maxBin() { return m_MaxBin; };

  StatusCode FindPVTracks(VeloTracks& aPV, double m_z_min,
			  double m_z_max, int m_PV_trackmin, double m_z_sigma, 
			  double m_IPmax, double m_TrIPmax, int& nPV); 

  StatusCode FitPV(VeloTracks& aPV, int PV_number);

protected:

private:
  double m_Min, m_Max;
  double m_BinWidth;
  int m_Bins;
  std::vector<int> m_Histo;
  double m_MaxVal;
  PVHisto::iterator m_MaxBin;
};
#endif // PVHISTO_H

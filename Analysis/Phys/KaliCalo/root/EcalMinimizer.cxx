#include "EcalMinimizer.h"

double EcalMinimizer::fcn( const double *par ){
  double sum = 0.0;
  double sumw = 0.0;
  int entry = 0;
  while ( m_chain->GetEntry( entry ) ) {
    entry++;
    int necal1 = m_parameters[ m_cellIDs[ m_cellID1+(CALOSHIFT*2) ] ];
    int necal2 = m_parameters[ m_cellIDs[ m_cellID2+(CALOSHIFT*2) ] ];
    // Ecal consts
    double a1 = 1.0;
    double a2 = 1.0;
    if ( necal1 >= 0 ) a1 = par[necal1];
    if ( necal2 >= 0 ) a2 = par[necal2];
    int area1 = (m_cellID1 & ((1<<2)-1)<<12) >> 12;
    int area2 = (m_cellID2 & ((1<<2)-1)<<12) >> 12;   
    // Corrected mas calculation
    float massdiff = m_mass * sqrt( a1*a2 ) - pionmass;
    float sigma = max(m_sigma[area1], m_sigma[area2]);
    if ( abs(massdiff) > (m_sigmaCut * sigma) ) continue;
    float correction = 1.0 + m_bkg * m_correction;
    int bkg = 1 - 2 * m_bkg; // Bkg : -1 , signal : 1
    float w = weight( massdiff, sigma );
    // Function to minimize
    sumw = sumw + correction * bkg * w;
    sum = sum + correction * bkg * w * massdiff * massdiff;
  }
  if ( gDebug ) std::cout << abs((sum/sumw)) << std::endl;
  return abs((sum/sumw));
}


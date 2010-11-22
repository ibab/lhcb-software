#include "PrsMinimizer.h"

double PrsMinimizer::fcn( const double *par ){
  double sum = 0.0;
  double sumw = 0.0;
  int entry = 0;
  while ( m_chain->GetEntry( entry ) ) {
    entry++;
    int necal1 = m_parameters[ m_cellIDs[ m_cellID1+(CALOSHIFT*2) ] ];
    int necal2 = m_parameters[ m_cellIDs[ m_cellID2+(CALOSHIFT*2) ] ];
    int nprs1 = m_parameters[ m_cellIDs[ m_cellID1+CALOSHIFT ] ];
    int nprs2 = m_parameters[ m_cellIDs[ m_cellID2+CALOSHIFT ] ];
    // Ecal consts
    double a1 = 1.0;
    double a2 = 1.0;
    // PRS consts
    double b1 = 1.0;
    double b2 = 1.0;
    if ( necal1 >= 0 ) a1 = par[necal1];
    if ( necal2 >= 0 ) a2 = par[necal2];
    if ( nprs1 >= 0 )  b1 = par[nprs1];
    if ( nprs2 >= 0 )  b2 = par[nprs2];
    int area1 = (m_cellID1 & ((1<<2)-1)<<12) >> 12;
    int area2 = (m_cellID2 & ((1<<2)-1)<<12) >> 12;   
    // Corrected mas caluclation
    float f1 = m_prs1*m_prsBeta[area1]*(b1-a1)+a1*m_e1;
    float f2 = m_prs2*m_prsBeta[area2]*(b2-a2)+a2*m_e2;
    float newmass = m_mass * sqrt( f1*f2/(m_e1*m_e2) );
    float massdiff = newmass - pionmass;
    float sigma = max(m_sigma[area1], m_sigma[area2]);
    if ( abs(massdiff) > (m_sigmaCut * sigma) ) continue;
    float correction = 1.0 + m_bkg * m_correction;
    int bkg = 1 - 2 * m_bkg; // Bkg : -1 , signal : 1
    float w = weight( massdiff, sigma );
    // Function to minimize
    sumw = sumw + correction * bkg * w;
    sum = sum + correction * bkg * w * massdiff * massdiff;
  }
  return abs((sum/sumw));
}


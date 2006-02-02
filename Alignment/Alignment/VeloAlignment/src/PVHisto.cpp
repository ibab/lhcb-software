// Include files
// from STL
#include <string>

// local
#include "PVHisto.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PVHisto
//
// 2002-12-02 : Philippe Charpentier
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVHisto::PVHisto( double min, double max, double width )
  : m_Min(min)
  , m_Max(max)
  , m_BinWidth(width)
  , m_Bins (int(0.99 + (max-min)/width))
  , m_Histo(m_Bins, 0)
  , m_MaxVal(0)
  { 
    m_MaxBin = m_Histo.begin();
  }; 
PVHisto::PVHisto( double min, double max, int bins )
  : m_Min(min)
  , m_Max(max)
  , m_BinWidth((max-min)/bins)
  , m_Bins (bins)
  , m_Histo(bins, 0)
  , m_MaxVal(0)
  { 
    m_MaxBin = m_Histo.begin();
  }; 
//=============================================================================
PVHisto::iterator PVHisto::fill( double val )
{
  int bin = int((val - m_Min)/m_BinWidth);
  if( bin < 0 || bin >= m_Bins ) return m_Histo.end(); // return an iterator outside the histogram
  int newContent = ++m_Histo[bin];
  if( newContent > m_MaxVal ) {
    m_MaxVal = newContent;
    m_MaxBin = m_Histo.begin() + bin;
  }
  return m_Histo.begin() + bin;
}

PVHisto::iterator PVHisto::getbin( double val )
{
  return (m_Histo.begin() + int((val - m_Min)/m_BinWidth));
}

void PVHisto::reset()
{
  m_Histo = std::vector<int>( m_Bins, 0);
  m_MaxVal = 0.;
  m_MaxBin = m_Histo.begin();  
}


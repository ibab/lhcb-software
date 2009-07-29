#include "Kernel/ITDetectorPlot.h"

using namespace ST;

ITDetectorHistoProperties::ITDetectorHistoProperties(const std::string& detType)
  :  m_detType( detType ), m_channelID( 0 )
{
  if ( m_detType == "IT" )
    {
      m_xMin = -14.5;
      m_xMax = 14.5;
      m_yMin = -0.5;
      m_yMax = 50.5;
    }
  else
    {
      m_xMin = -14.5;
      m_xMax = 14.5;
      m_yMin = -0.5;
      m_yMax = 50.5;
    }

  m_nBinX = static_cast<unsigned int>(m_xMax - m_xMin);
  m_nBinY = static_cast<unsigned int>(m_yMax - m_yMin);
}

ITDetectorHistoProperties::~ITDetectorHistoProperties()
{}

bool ITDetectorHistoProperties::setChannelID(const LHCb::STChannelID& chanID)
{
  m_channelID = chanID;

  if ( m_detType == "IT" )
    {
      if ( ! chanID.isIT() )
	return false;

      unsigned int station( chanID.station() ), box( chanID.detRegion() ),
	layer( chanID.layer() ), sector( chanID.sector() );

      m_yBin = 16. * (station - 1.);
      m_xBin = 0.;

      if (box == 3 || box == 4)
	{
	  m_xBin = 4. - static_cast<double>(sector);
	  if (box == 3) m_yBin += 4. + static_cast<double>(layer);
	  if (box == 4) m_yBin += 12. + static_cast<double>(layer);
	}
      else if (box == 1)
	{
	  m_yBin += 8. + static_cast<double>(layer);
	  m_xBin = 11. - static_cast<double>(sector);
	}
      else
	{
	  m_yBin += 8. + static_cast<double>(layer);
	  m_xBin = -3. - static_cast<double>(sector);
	}

      return true;
    }
  else if ( m_detType == "TT" )
    {
      if ( ! chanID.isTT() )
	return false;
      
      m_xBin = 0.;
      m_yBin = 0.;
      
      return true;
    }
  else
    return false;
}

const double& ITDetectorHistoProperties::xBin() const
{
  return m_xBin;
}

const double& ITDetectorHistoProperties::yBin() const
{
  return m_yBin;
}

const double& ITDetectorHistoProperties::minBinX() const
{
  return m_xMin;
}

const double& ITDetectorHistoProperties::maxBinX() const
{
  return m_xMax;
}

const double& ITDetectorHistoProperties::minBinY() const
{
  return m_yMin;
}

const double& ITDetectorHistoProperties::maxBinY() const
{
  return m_yMax;
}

const unsigned int& ITDetectorHistoProperties::nBinX() const
{
  return m_nBinX;
}

const unsigned int& ITDetectorHistoProperties::nBinY() const
{
  return m_nBinY;
}

std::string ITDetectorHistoProperties::histoTitle() const
{
  return m_detType + " Efficiencies";
}

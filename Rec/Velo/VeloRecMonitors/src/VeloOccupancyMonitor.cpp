// Include files
// -------------

#include <iomanip>
#include <cstring>
#include <string>
#include <fstream>
#include <algorithm>

#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#endif // !WIN32

/// from Gaudi
#include "GaudiUtils/Aida2ROOT.h"

// from VeloDet
#include "VeloDet/DeVeloSensor.h"
#include "VeloDet/VeloTell1Channel.h"

// local
#include "VeloOccupancyMonitor.h"

// from Boost
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : VeloOccupancyMonitor
//
// VELO occupancy monitoring algorithm.
// Produces a set of histograms from the cluster banks in the TES.
// Can also be configured to produce strip masking conditions XML.
// 2011-10-20 :	Manuel Schiller
// 2011-12-14	do not update histograms in each event to save some speed
// 	`	(saves a factor 35 in execution time)
// 		label plot axes (I'm sure I forgot some, though)
// 		make nice plots to enable occupancy based dead/noisy strip
// 		monitoring
//		saner defaults for XML and txt files with dead/noisy Channel
//		lists
// 2009-08-18 :	James Michael Keaveney
// 2009-08-04 :	Kurt Rinnert
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace Velo
{
  DECLARE_ALGORITHM_FACTORY(VeloOccupancyMonitor)
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloOccupancyMonitor::VeloOccupancyMonitor(const std::string& name,
    ISvcLocator* pSvcLocator) :
  Velo::VeloMonitorBase(name , pSvcLocator),
  m_nstrips((84 + 4) * 2048),
  m_tell1Map(0), m_timeStamps(0), m_pvssTell1Names(0),
  m_occupancyDenom(0), m_histOccSpectAll(0), m_histOccSpectLow(0),
  m_fastHistOccSpectAll(0), m_fastHistOccSpectLow(0), m_histAvrgSensor(0), m_histAvrgSensorPU(0),
  m_histBCIDSpec(0), m_inOccAccu(0), m_deadPerSensor(0), m_noisyPerSensor(0),
  m_deadOrNoisyPerSensor(0), m_deadPerSensorAndLink(0),
  m_noisyPerSensorAndLink(0), m_deadOrNoisyPerSensorAndLink(0)

{
  declareProperty("WriteXML", m_writeXML = false);
  declareProperty("XMLDirectory", m_xmlDir = ".");
  declareProperty("XMLDirectoryDate", m_xmlDirDate = true);
  declareProperty("ParamName", m_paramName = "link_mask");
  declareProperty("HighOccCut", m_highOccCut = 5.0); // percent
  declareProperty("LowOccCut", m_lowOccCut = 0.0); // percent
  declareProperty("VeloClusterLocation", m_clusterCont = LHCb::VeloClusterLocation::Default);
  declareProperty("OccupancyResetFrequency", m_occupancyResetFreq = 0);
  declareProperty("UseOdin", m_useOdin = true);
  declareProperty("WriteDeadToXML", m_writeDeadToXML = false);
}

//=============================================================================
// Destructor
//=============================================================================
Velo::VeloOccupancyMonitor::~VeloOccupancyMonitor()
{
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloOccupancyMonitor::initialize()
{
  StatusCode sc = VeloMonitorBase::initialize(); // must be executed first
  if (sc.isFailure()) return sc;

  // get tools needed for writing XML files
  if (m_writeXML) {
#ifndef WIN32
    m_tell1Map   = tool<Velo::ITELL1SensorMap> ("Velo::LivDBTELL1SensorMap", "Tell1Map");
    m_timeStamps = tool<Velo::ITimeStampProvider> ("Velo::TimeStamps", "TimeStamps");
#else
    info() << "Writing strip mask XML files is not supported on Windows!" << endmsg;
#endif // !WIN32
  }

  // access to PVSS TELL1 names
  m_pvssTell1Names = tool<Velo::IPvssTell1Names> ("Velo::PvssTell1Names", "PvssTell1Names");

  // try to find TAE sample name in algo instance name (for histo titles)
  std::string tmpTae = name();
  std::size_t posPrev = tmpTae.find("Prev");
  std::size_t posNext = tmpTae.find("Next");
  if (std::string::npos != posPrev) {
    m_tae = tmpTae.substr(posPrev, 5);
  } else if (std::string::npos != posNext) {
    m_tae = tmpTae.substr(posNext, 5);
  } else {
    m_tae = "Default";
  }

  // find the largest sensor number from detector element (to avoid making any assumptions
  // about the sensor nubmering scheme)
  unsigned maxSensNum = 0;
  for (std::vector<DeVeloSensor*>::const_reverse_iterator si = m_veloDet->sensorsReverseBegin();
      si != m_veloDet->sensorsReverseEnd(); ++si) {
    if ((*si)->sensorNumber() > maxSensNum) {
      maxSensNum = (*si)->sensorNumber();
    }
  }

  m_occupancies.resize(maxSensNum + 1, 0);
  m_occupanciesCh.resize(maxSensNum + 1, 0);
  m_stripOccupancyHistPerSensor.resize(maxSensNum + 1, 0);
  m_channelOccupancyHistPerSensor.resize(maxSensNum + 1, 0);
  m_occAccu.resize(1 + maxSensNum, std::vector<unsigned char>(2048, 0u));
  m_occAccuTouched.resize(1 + maxSensNum, 0u);
  m_inOccAccu = 0u;

  if (m_useOdin) {
    m_veloOccVsBunchId.push_back(bookProfile1D("h_veloOccVsBunchId_ASide",
          "Percentage Velo Occupancy vs LHC bunch-ID (ASide);bunch id;occupancy [%]",
          -0.5, 3563.5, 3564));
    m_veloOccVsBunchId.push_back(bookProfile1D("h_veloOccVsBunchId_CSide"
          , "Percentage Velo Occupancy vs LHC bunch-ID (CSide);bunch id;occupancy [%]"
          , -0.5, 3563.5, 3564));

    m_histBCIDSpec = Gaudi::Utils::Aida2ROOT::aida2root(book1D("BCID Spectrum", "BCID Spectrum;bunch id;entries", -0.5, 3563.5, 3564));

  }

  for (std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
      si != m_veloDet->sensorsEnd();
      ++si) {
    unsigned s = (*si)->sensorNumber();

    boost::format fmtName("OccPerStripSens%d") ;
    fmtName % s;
    boost::format fmtTitle("Strip Occupancy, Sensor %d (") ;
    fmtTitle % s;
    std::string title = fmtTitle.str() + m_pvssTell1Names->pvssName(s) + ") " + m_tae +
      ";strip number;occupancy [%]";

    m_stripOccupancyHistPerSensor[s] = 
      Gaudi::Utils::Aida2ROOT::aida2root(book1D(fmtName.str(), title, -0.5, 2047.5, 2048));
    m_occupancies[s] = new Velo::Hist1D(m_stripOccupancyHistPerSensor[s]);

    boost::format fmtNameCh("OccPerChannelSens%d") ;
    fmtNameCh % s;
    boost::format fmtTitleCh("Channel Occupancy, Sensor %d (") ;
    fmtTitleCh % s;
    std::string titleCh = fmtTitleCh.str() + m_pvssTell1Names->pvssName(s) + ") " + m_tae +
      ";strip number;occupancy [%]";

    m_channelOccupancyHistPerSensor[s] =
      Gaudi::Utils::Aida2ROOT::aida2root(book1D(fmtNameCh.str(), titleCh, -0.5, 2047.5, 2048));
    m_occupanciesCh[s] = new Velo::Hist1D(m_channelOccupancyHistPerSensor[s]);
  }
  m_histOccSpectAll = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccSpectAll", "Occupancy Spectrum;occupancy [%];entries", -0.0025, 100.0025, 20001));
  m_histOccSpectLow = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccSpectMaxLow", "Occupancy Spectrum;occupancy [%];entries", -0.0005, 20.0005, 20001));
  m_fastHistOccSpectAll = new Velo::Hist1D(m_histOccSpectAll);
  m_fastHistOccSpectLow = new Velo::Hist1D(m_histOccSpectLow);
  m_histAvrgSensor = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccAvrgSens", "Avrg. Occupancy vs. Sensor;sensor number;average occupancy [%]", -0.5, 131.5, 132));
  m_histAvrgSensorPU = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccAvrgSensPU", "Avrg. Occupancy vs. Sensor PU;sensor number;average occupancy [%]", 127.5, 131.5, 4));

  m_deadPerSensor = Gaudi::Utils::Aida2ROOT::aida2root(
      book1D("DeadPerSensor",
        "dead channels per sensor number;sensor number;number of dead channels",
        -0.5, 131.5, 132));
  m_noisyPerSensor = Gaudi::Utils::Aida2ROOT::aida2root(
      book1D("NoisyPerSensor",
        "noisy channels per sensor number;sensor number;number of noisy channels",
        -0.5, 131.5, 132));
  m_deadOrNoisyPerSensor = Gaudi::Utils::Aida2ROOT::aida2root(
      book1D("DeadOrNoisyPerSensor",
        "dead/noisy channels per sensor number;sensor number;number of channels dead/noisy",
        -0.5, 131.5, 132));
  m_deadPerSensorAndLink = Gaudi::Utils::Aida2ROOT::aida2root(
      book2D("DeadPerSensorAndLink",
        "dead channels per sensor number and analogue link;sensor number;analogue link;number of dead channels",
        -0.5, 131.5, 132, -0.5, 63.5, 64));
  m_noisyPerSensorAndLink = Gaudi::Utils::Aida2ROOT::aida2root(
      book2D("NoisyPerSensorAndLink",
        "noisy channels per sensor number and analogue link;sensor number;analogue link;number of noisy channels",
        -0.5, 131.5, 132, -0.5, 63.5, 64));
  m_deadOrNoisyPerSensorAndLink = Gaudi::Utils::Aida2ROOT::aida2root(
      book2D("DeadOrNoisyPerSensorAndLink",
        "dead/noisy channels per sensor number and analogue link;sensor number;analogue link;number of channels dead/noisy",
        -0.5, 131.5, 132, -0.5, 63.5, 64));

  m_runsseen.clear();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloOccupancyMonitor::execute()
{
  // Increment the occpancy denominator, reset at configurable
  // frequency
  if (m_occupancyResetFreq != 0 &&
      0 == (m_occupancyDenom % m_occupancyResetFreq)) {
    latchOccupancyFromAccu(true);
    updateDeadAndNoisy();
    m_occupancyDenom = 0;
    for (std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
        si != m_veloDet->sensorsEnd();
        ++si) {
      const unsigned s = (*si)->sensorNumber();
      m_occupancies[s]->reset();
      m_occupanciesCh[s]->reset();
    }
  }

  monitorOccupancy();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloOccupancyMonitor::finalize()
{
  if (m_debugLevel) debug() << "==> Finalize" << endmsg;

  latchOccupancyFromAccu(true);
  updateDeadAndNoisy();

  // create conditions and write them to XML, if requested.
  // not supported on Windows.
#ifndef WIN32
  if (m_writeXML) {
    struct stat statbuf;

    mode_t perm = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
    umask(~perm);

    if (-1 == stat(m_xmlDir.c_str(), &statbuf)) {
      if (-1 == mkdir(m_xmlDir.c_str(), perm)) {
        error() << "Could not create directory '" << m_xmlDir << "'." << endmsg;
        return StatusCode::FAILURE;
      }
    }

    std::string tsDir = m_xmlDir;
    if (m_xmlDirDate) {
      tsDir += std::string("/") + m_timeStamps->initTime();
    }
    if (-1 == stat(tsDir.c_str(), &statbuf)) {
      if (-1 == mkdir(tsDir.c_str(), perm)) {
        error() << "Could not create directory '" << tsDir << "'." << endmsg;
        return StatusCode::FAILURE;
      }
    }

    // gather some information to put into text file and XML file(s) to
    // help to track where things come from
    char host[64], domain[128], userbuf[4096], username[32];
    struct passwd pw, *ppw = NULL;
    host[0] = domain[0] = username[0] = 0;
    if (0 != gethostname(host, 64)) host[0] = 0;
    if (0 != getdomainname(domain, 128)) domain[0] = 0;
    if (0 == getpwuid_r(getuid(), &pw, userbuf, 4096, &ppw) &&
        ppw == &pw && std::strlen(pw.pw_name) > 0) {
      std::strncpy(username, pw.pw_name, 32);
    }

    host[63] = domain[127] = username[31] = 0;
    std::string runby = std::string("run by ") + username + " on " +
      host + "." + domain + " on " + m_timeStamps->initTime();
    // make sure runby is also a valid XML comment (replace "--" by "__")
    for (unsigned i = 1; i < runby.size(); ++i) {
      if ('-' == runby[i - 1] && '-' == runby[i])
        runby[i - 1] = runby[i] = '_';
    }
    // format a nice list of runs seen so far (break lines at 76 characters)
    std::vector<std::string> runsseen;
    runsseen.push_back("");
    for (std::set<unsigned>::const_iterator it = m_runsseen.begin();
        m_runsseen.end() != it; ++it) {
      const std::string str = boost::lexical_cast<std::string>(*it);
      if (76 < 1 + runsseen.back().size() + str.size()) {
        // new line
        runsseen.back() += "\n";
        runsseen.push_back("");
      }
      if (runsseen.back().empty()) {
        // first one can just be appended
        runsseen.back() += str;
      } else {
        // subsequent ones have to have a space separating run numbers
        runsseen.back() += " ";
        runsseen.back() += str;
      }
    }
    if (!runsseen.empty()) runsseen.back() += "\n";

    std::ofstream txtFile("masked_channels.txt");
    txtFile << std::string(78, '#') << std::endl <<
      "# " << runby << std::endl;
    if (!runsseen.empty()) {
      txtFile << "#" << std::endl << "# runs numbers seen:" << std::endl;
      for (unsigned i = 0; i < runsseen.size(); ++i)
        txtFile << "# " << runsseen[i];
    }
    txtFile << std::string(78, '#') << std::endl << std::endl;

    std::string fileNameBase = tsDir + "/strip_mask_tell1_";
    for (std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
        si != m_veloDet->sensorsEnd();
        ++si) {
      unsigned sensorNumber = (*si)->sensorNumber();
      unsigned tell1Number = m_tell1Map->sensorToTell1(sensorNumber);

      std::vector<int> masks(2048, 0);
      Velo::Hist1D* occHist = m_occupanciesCh[sensorNumber];
      for (unsigned chipChannel = 0; chipChannel < 2048; ++chipChannel) {
        double occ = occHist->getBinContent(chipChannel + 1);
        if (occ >= m_highOccCut || occ <= m_lowOccCut) {
          unsigned tell1Channel = Velo::chipChannelToTell1Channel(chipChannel);
          unsigned strip = (*si)->ChipChannelToStrip(chipChannel);
          // dead channels are only written to XML files if the user
          // explicitly requests it
          if (m_writeDeadToXML || occ >= m_highOccCut)
            masks[tell1Channel] = 1;
          // try to get a bit more information on the strip from
          // CondDB
          DeVeloSensor::StripInfo nfo =
            (*si)->stripInfo((*si)->ChipChannelToStrip(chipChannel));
          std::string nfostr;
          if (nfo.stripIsOK()) nfostr += " Ok";
          if (nfo.stripIsLowGain()) nfostr += " LowGain";
          if (nfo.stripIsNoisy()) nfostr += " Noisy";
          if (nfo.stripIsDead()) nfostr += " Dead";
          if (nfo.stripIsOpen()) nfostr += " Open";
          if (nfo.stripIsPinhole()) nfostr += " Pinhole";
          if (nfo.stripIsShort()) nfostr += " Short";
          txtFile << "sensor " << std::setw(3) << sensorNumber <<
            " strip " << std::setw(4) << strip <<
            " chip channel " << std::setw(4) << chipChannel <<
            " Tell1 channel " << std::setw(4) << tell1Channel << 
            " --- " << std::setw(5) << ((m_lowOccCut >= occ)?"DEAD":"NOISY") <<
            " (CondDB:" << nfostr << ")" << std::endl;
        }
      }

      Condition sensorCond;
      sensorCond.addParam(m_paramName, masks);

      std::string tell1NumberStr = boost::lexical_cast<std::string> (tell1Number);
      std::string fileName = fileNameBase + tell1NumberStr + ".xml";
      std::string condName = "VeloTELL1Board" + tell1NumberStr;

      std::ofstream xmlFile(fileName.c_str());
      xmlFile << sensorCond.toXml(condName, true) << std::endl;
      xmlFile << "<!-- " << std::endl << runby << std::endl;
      if (!runsseen.empty()) {
        xmlFile << std::endl << "runs numbers seen:" << std::endl;
        for (unsigned i = 0; i < runsseen.size(); ++i) {
          xmlFile << runsseen[i];
        }
      }
      xmlFile << "-->" << std::endl;
      xmlFile.close();
    }
    txtFile.close();
  }
#endif // !WIN32

  delete m_fastHistOccSpectAll;
  delete m_fastHistOccSpectLow;

  while (! m_occupancies.empty()) {
    delete m_occupancies.back();
    m_occupancies.pop_back();
    delete m_occupanciesCh.back();
    m_occupanciesCh.pop_back();
  }

  return VeloMonitorBase::finalize(); // must be called after all other actions
}

//=============================================================================
// Retrieve the VeloClusters
//=============================================================================
LHCb::VeloClusters* Velo::VeloOccupancyMonitor::veloClusters()
{
  if (m_debugLevel) {
    debug() << "Retrieving VeloClusters from " << m_clusterCont << endmsg;
  }

  LHCb::VeloClusters* clusters = getIfExists<LHCb::VeloClusters>(m_clusterCont);
  if ( NULL == clusters ) {
    info() << "No VeloClusters found for this event!" << endmsg;
  } else {
    if (m_debugLevel) {
      debug() << "  -> number of clusters found in TES: "
        << clusters->size() << endmsg;
    }
  }
  return clusters;
}

//=============================================================================
// Retrieve the ODIN bank
//=============================================================================
LHCb::ODIN* Velo::VeloOccupancyMonitor::getOdinBank()
{
  if (m_debugLevel) {
    debug() << "Retrieving ODIN bank from " << LHCb::ODINLocation::Default
      << endmsg;
  }

  LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(LHCb::ODINLocation::Default);
  if ( NULL == odin ) {
    if (UNLIKELY(msgLevel(MSG::DEBUG))) {
      debug() << "No ODIN bank found. Histograms involving bunch "
        "IDs disabled." << endmsg;
    }
  }
  return odin;
}

// update the histograms which summarise the number of dead and noisy channels
void Velo::VeloOccupancyMonitor::updateDeadAndNoisy()
{
  m_deadPerSensor->Reset();
  m_deadPerSensorAndLink->Reset();
  m_noisyPerSensor->Reset();
  m_noisyPerSensorAndLink->Reset();
  m_deadOrNoisyPerSensor->Reset();
  m_deadOrNoisyPerSensorAndLink->Reset();

  for (std::vector<DeVeloSensor*>::const_iterator si = m_veloDet->sensorsBegin();
      si != m_veloDet->sensorsEnd(); ++si) {
    unsigned sensorNumber = (*si)->sensorNumber();

    Velo::Hist1D* occHist = m_occupanciesCh[sensorNumber];
    for (unsigned chipChannel = 0; chipChannel < 2048; ++chipChannel) {
      double occ = occHist->getBinContent(chipChannel + 1);
      if (occ >= m_highOccCut || occ <= m_lowOccCut) {
        int link = chipChannel / 32;;
        if (occ >= m_highOccCut) {
          m_noisyPerSensor->Fill(sensorNumber);
          m_noisyPerSensorAndLink->Fill(sensorNumber, link);
          m_deadOrNoisyPerSensor->Fill(sensorNumber);
          m_deadOrNoisyPerSensorAndLink->Fill(sensorNumber, link);
        } else if (occ <= m_lowOccCut) {
          m_deadPerSensor->Fill(sensorNumber);
          m_deadPerSensorAndLink->Fill(sensorNumber, link);
          m_deadOrNoisyPerSensor->Fill(sensorNumber);
          m_deadOrNoisyPerSensorAndLink->Fill(sensorNumber, link);
        }
      }
    }
  }
}

//=============================================================================
// Monitor the VeloClusters
//=============================================================================
void Velo::VeloOccupancyMonitor::monitorOccupancy()
{
  // Retrieve the VeloClusters
  // -------------------------
  LHCb::VeloClusters* clusters = veloClusters();
  if (0 == clusters) return;  // no clusters in this event

  LHCb::ODIN* odin = 0;
  if (m_useOdin) {
    odin = getOdinBank();
    m_runsseen.insert(odin->runNumber());
  }

  unsigned nClustersOnSide[2] = { 0u, 0u };

  // Loop over the VeloClusters
  LHCb::VeloClusters::const_iterator itVC;
  for (itVC = clusters->begin(); itVC != clusters->end(); ++itVC) {
    LHCb::VeloCluster* cluster = (*itVC);
    const unsigned sensor = cluster -> channelID().sensor();
    const bool isRight = m_veloDet->sensor(sensor)->isRight();
    const unsigned strip = cluster->channelID().strip();
    ++m_occAccu[sensor][strip];
    m_occAccuTouched[sensor] |= 1u << (strip / 64u);

    if (m_useOdin && 0 != odin) {
      // side is either 0 (left/A) or 1 (right/C)
      ++nClustersOnSide[(isRight ? 1 : 0)];
    }
  } // end clusters loop
  ++m_inOccAccu;

  // periodically latch occupancy accumulator into histograms
  latchOccupancyFromAccu();

  // these are only available when the ODIN bank is present
  if (m_useOdin && 0 != odin) {
    m_histBCIDSpec->Fill(odin->bunchId());
    // side is either 0 (left/A) or 1 (right/C)
    for (unsigned side = 0; side < 2; ++side) {
      // the number of strips is the same in both halves and half
      // the total number of strips, occupancy is in %
      m_veloOccVsBunchId[side]->fill(odin->bunchId(),
          nClustersOnSide[side] / (m_nstrips * 0.5) * 100.0);
    }
  }
}

void Velo::VeloOccupancyMonitor::latchOccupancyFromAccu(bool force)
{
  // don't do anything unless we have to
  if (!force && 255u != m_inOccAccu) return;
  if (0u == m_inOccAccu) return;

  // ok, there are events to be accumulated
  const double scale = double(m_occupancyDenom) / double(m_occupancyDenom +
      m_inOccAccu);
  for (std::vector<DeVeloSensor*>::const_iterator si =
      m_veloDet->sensorsBegin(); si != m_veloDet->sensorsEnd(); ++si) {
    const unsigned sensor = (*si)->sensorNumber();	
    unsigned& mask = m_occAccuTouched[sensor];
    Velo::Hist1D* h_occ = m_occupancies[sensor];
    Velo::Hist1D* h_occCh = m_occupanciesCh[sensor];
    // scale the histograms (we form a running average)
    h_occ->scale(scale);
    h_occCh->scale(scale);
    // if the sensor was not hit, we're done
    if (!mask) continue;
    // get accumulator for sensor
    std::vector<unsigned char>& accu = m_occAccu[sensor];
    for (unsigned strip = 0u; strip < 2048u; strip += 64u) {
      if (!((mask >> (strip / 64u)) & 1u)) continue;
      for (unsigned ofs = 0u; ofs < 64u; ++ofs) {
        // calculate by how much the occupancy moves (in percent)
        const double incr = 100. * double(accu[strip + ofs]) /
          double(m_inOccAccu + m_occupancyDenom);
        // update per strip occupancy (value given in percent)
        double tmp = h_occ->getBinContent(1 + strip + ofs);
        h_occ->setBinContent(1 + strip + ofs, tmp + incr);
        // update per channel occupancy (value given in percent)
        const unsigned chipChannel = (*si)->StripToChipChannel(strip + ofs);
        tmp = h_occCh->getBinContent(1 + chipChannel);
        h_occCh->setBinContent(1 + chipChannel, tmp + incr);
      } // loop over group of 64 strips
    } // loop over strip groups
    // reset accumulator
    mask = 0u;
    std::fill(accu.begin(), accu.end(), 0u);
  } // loop over sensors
  m_occupancyDenom += m_inOccAccu;
  m_inOccAccu = 0;

  // Produce occupancy spectra, average occupancy per sensor
  m_fastHistOccSpectAll->reset();
  m_fastHistOccSpectLow->reset();
  for (std::vector<DeVeloSensor*>::const_iterator si =
      m_veloDet->sensorsBegin(); si != m_veloDet->sensorsEnd(); ++si) {
    const unsigned sensor = (*si)->sensorNumber();
    double avgOcc = 0.;
    for (unsigned strip = 0; strip < 2048; ++strip) {
      const double occ = m_occupancies[sensor]->getBinContent(strip + 1);
      avgOcc += occ;
      m_fastHistOccSpectAll->fillFast(occ);
      if (occ <= 20.0) {
        m_fastHistOccSpectLow->fillFast(occ);
      }
    }
    // set number of entries here to avoid separate loop
    m_occupancies[sensor]->setEntries(m_occupancyDenom);
    m_occupanciesCh[sensor]->setEntries(m_occupancyDenom);
    avgOcc /= 2048.;
    m_histAvrgSensor->SetBinContent(sensor + 1, avgOcc);
    if ( sensor > 127 ) { // PU only average occupancies
      m_histAvrgSensorPU->SetBinContent(sensor + 1 - 128, avgOcc);
    }
  }
  m_fastHistOccSpectAll->updateEntries();
  m_fastHistOccSpectLow->updateEntries();
  m_histAvrgSensor->SetEntries(m_occupancyDenom);
}

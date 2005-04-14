#include "Event/TrackKeys.h"

std::vector<std::string> TrackKeys::History = std::vector<std::string>();
std::vector<std::string> TrackKeys::HistoryFit = std::vector<std::string>();
std::vector<std::string> TrackKeys::Types = std::vector<std::string>();
std::vector<std::string> TrackKeys::Status = std::vector<std::string>();
std::vector<std::string> TrackKeys::Flags = std::vector<std::string>();

TrackKeys TrackKeys::s_trackKeys;

TrackKeys::TrackKeys() 
{
  History.push_back("unknown");
  History.push_back("long");
  History.push_back("seeding");
  History.push_back("kshort");
  History.push_back("matching");
  History.push_back("velo");
  History.push_back("veloTT");
  History.push_back("trgVelo");
  History.push_back("trgVeloTT");
  History.push_back("trgForward");

  HistoryFit.push_back("unknown");
  HistoryFit.push_back("kalman");

  Types.push_back("unknown");
  Types.push_back("velo");
  Types.push_back("veloR");
  Types.push_back("backward");
  Types.push_back("long");
  Types.push_back("upstream");
  Types.push_back("downstream");
  Types.push_back("ttrack");

  Status.push_back("unknown");
  Status.push_back("patRec");
  Status.push_back("readyToFit");
  Status.push_back("fitted");
  Status.push_back("failed");

  Flags.push_back("unknown");
  Flags.push_back("valid");
  Flags.push_back("unique");
  Flags.push_back("ipselected");
  Flags.push_back("pidselected");

}

unsigned int Keys::bitIndex(const std::vector<std::string>& keys, 
                            const std::string& key) 
{
  unsigned int index = Keys::index(keys,key);
  index = (unsigned int) pow(2,index);
  return index;
}

unsigned int Keys::index(const std::vector<std::string>& keys, 
                         const std::string& key) {
  unsigned int index = 0;
  for (std::vector<std::string>::const_iterator it = keys.begin();
       it != keys.end(); it++) {
    if (key == *it) return index;
    index += 1;
  }
  return 0;
}

const std::string& Keys::key(const std::vector<std::string>& keys, 
                             unsigned int index) {
  if (index > 0 && index < keys.size()) return keys[(int) index];
  return keys[0];
}

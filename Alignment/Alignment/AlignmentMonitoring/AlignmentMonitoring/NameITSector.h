#ifndef NAMEITSECTOR_H
#define NAMEITSECTOR_H

#include <string>

/** @class  NameITSector
 *  @author Zhirui Xu
 *  @date   2015.10.16
 */

class NameITSector{
  public:
    // standard constructor
    NameITSector(const std::string name);
    virtual ~NameITSector() {};

    std::string GetBoxName();
    std::string GetLayerName();
    int GetUniqueSector();
    int GetUniqueLayer();
    int GetUniqueBox();
    int GetNumberOfSensors();
    int GetSectorID();
    int GetStationID();
    int GetChannel();

  protected:

  private:
    std::string m_station;
    std::string m_box;
    std::string m_sector;
    std::string m_layer;

    int m_uniqueBox;
    int m_uniqueLayer;
    int m_uniqueSector;
    int m_sectorID;
    int m_stationID;
    int m_numberOfSensors;
    
    int m_channel;
    void ConvertName2Channel();
};

#endif

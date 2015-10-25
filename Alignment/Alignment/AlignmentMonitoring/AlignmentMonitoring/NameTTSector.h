#ifndef NAMETTSECTOR_H
#define NAMETTSECTOR_H

#include <string>

/** @class  NameTTSector
 *  @author Zhirui Xu
 *  @date   2015.10.16
 */

class NameTTSector{
  public:
    // standard constructor
    NameTTSector(const std::string name);
    virtual ~NameTTSector() {};

    int GetUniqueSector();
    int GetNumberOfSensors();
    int GetSectorID();
    int GetChannel();
    std::string GetStation();
    std::string GetLayer();
    std::string GetRegion(); 

  protected:

  private:
    std::string m_station;
    std::string m_layer;
    std::string m_region;
    std::string m_sector;

    int m_uniqueSector;
    int m_sectorID;
    int m_numberOfSensors;

    int m_channel;
    void ConvertName2Channel();
};

#endif

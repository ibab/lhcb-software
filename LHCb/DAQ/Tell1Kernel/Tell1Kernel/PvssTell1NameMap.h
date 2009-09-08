// $Id: PvssTell1NameMap.h,v 1.1 2009-09-08 16:18:42 krinnert Exp $
#ifndef INCLUDE_PVSSTELL1NAMEMAP_H
#define INCLUDE_PVSSTELL1NAMEMAP_H 1


namespace Velo {
  /** @class PvssTell1NameMap PvssTell1NameMap.h
   *  Maps sensor numbers to PVSS TELL1 names.
   *
   *  This mapping is static. It is determined by the rack
   *  position of the TELL1 boards. 
   *
   * @author Kurt Rinnert <kurt.rinnert@cern.ch>
   * @date   2009-09-07
   */
  class PvssTell1NameMap {

    public:

      /// Standard Constructor
      inline PvssTell1NameMap() {
        m_pvssNames[128] = "tputell02";
        m_pvssNames[130] = "tputell03";
        m_pvssNames[129] = "tputell04";
        m_pvssNames[131] = "tputell05";
        m_pvssNames[ 94] = "vetella01";
        m_pvssNames[ 40] = "vetella02";
        m_pvssNames[ 32] = "vetella03";
        m_pvssNames[ 98] = "vetella04";
        m_pvssNames[102] = "vetella05";
        m_pvssNames[ 36] = "vetella06";
        m_pvssNames[ 28] = "vetella07";
        m_pvssNames[ 90] = "vetella08";
        m_pvssNames[ 30] = "vetella09";
        m_pvssNames[ 34] = "vetella10";
        m_pvssNames[ 96] = "vetella11";
        m_pvssNames[ 26] = "vetella12";
        m_pvssNames[ 38] = "vetella13";
        m_pvssNames[100] = "vetella14";
        m_pvssNames[ 92] = "vetella15";
        m_pvssNames[104] = "vetella16";
        m_pvssNames[ 70] = "vetella17";
        m_pvssNames[ 24] = "vetella18";
        m_pvssNames[ 82] = "vetella19";
        m_pvssNames[ 86] = "vetella20";
        m_pvssNames[  8] = "vetella21";
        m_pvssNames[  4] = "vetella22";
        m_pvssNames[ 74] = "vetella23";
        m_pvssNames[  0] = "vetella24";
        m_pvssNames[ 12] = "vetella25";
        m_pvssNames[ 66] = "vetella26";
        m_pvssNames[ 22] = "vetella27";
        m_pvssNames[ 88] = "vetella28";
        m_pvssNames[ 84] = "vetella29";
        m_pvssNames[ 80] = "vetella30";
        m_pvssNames[  6] = "vetella31";
        m_pvssNames[  2] = "vetella32";
        m_pvssNames[ 68] = "vetella33";
        m_pvssNames[ 72] = "vetella34";
        m_pvssNames[ 64] = "vetella35";
        m_pvssNames[ 76] = "vetella36";
        m_pvssNames[ 14] = "vetella37";
        m_pvssNames[ 18] = "vetella38";
        m_pvssNames[ 10] = "vetella39";
        m_pvssNames[ 16] = "vetella40";
        m_pvssNames[ 20] = "vetella41";
        m_pvssNames[ 78] = "vetella42";
        m_pvssNames[103] = "vetellc01";
        m_pvssNames[ 37] = "vetellc02";
        m_pvssNames[ 41] = "vetellc03";
        m_pvssNames[ 99] = "vetellc04";
        m_pvssNames[ 39] = "vetellc05";
        m_pvssNames[101] = "vetellc06";
        m_pvssNames[ 35] = "vetellc07";
        m_pvssNames[105] = "vetellc08";
        m_pvssNames[ 71] = "vetellc09";
        m_pvssNames[  5] = "vetellc10";
        m_pvssNames[  1] = "vetellc11";
        m_pvssNames[ 67] = "vetellc12";
        m_pvssNames[  9] = "vetellc13";
        m_pvssNames[ 11] = "vetellc14";
        m_pvssNames[ 27] = "vetellc15";
        m_pvssNames[ 97] = "vetellc16";
        m_pvssNames[ 93] = "vetellc17";
        m_pvssNames[ 15] = "vetellc18";
        m_pvssNames[ 77] = "vetellc19";
        m_pvssNames[ 19] = "vetellc20";
        m_pvssNames[ 85] = "vetellc21";
        m_pvssNames[ 23] = "vetellc22";
        m_pvssNames[ 89] = "vetellc23";
        m_pvssNames[ 81] = "vetellc24";
        m_pvssNames[ 31] = "vetellc25";
        m_pvssNames[  7] = "vetellc26";
        m_pvssNames[ 73] = "vetellc27";
        m_pvssNames[ 69] = "vetellc28";
        m_pvssNames[  3] = "vetellc29";
        m_pvssNames[ 65] = "vetellc30";
        m_pvssNames[ 13] = "vetellc31";
        m_pvssNames[ 75] = "vetellc32";
        m_pvssNames[ 79] = "vetellc33";
        m_pvssNames[ 87] = "vetellc34";
        m_pvssNames[ 25] = "vetellc35";
        m_pvssNames[ 91] = "vetellc36";
        m_pvssNames[ 17] = "vetellc37";
        m_pvssNames[ 83] = "vetellc38";
        m_pvssNames[ 21] = "vetellc39";
        m_pvssNames[ 95] = "vetellc40";
        m_pvssNames[ 33] = "vetellc41";
        m_pvssNames[ 29] = "vetellc42";
        m_unknown = "UNKNOWN";
      }

      inline const std::string& pvssName( unsigned int sensorNumber ) const {
        std::map<unsigned int, std::string>::const_iterator ci = m_pvssNames.find(sensorNumber);
        if ( m_pvssNames.end() != ci ) {
          return ci->second;
        }
        return m_unknown;
      }

      inline ~PvssTell1NameMap() {;} 



    private:

      std::string m_unknown;
      std::map<unsigned int, std::string> m_pvssNames; ///< table of PVSS names

  };
}
#endif // INCLUDE_PVSSTELL1NAMEMAP_H

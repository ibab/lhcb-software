// $Id: Tower.h,v 1.17 2008-11-07 16:23:39 jucogan Exp $

#ifndef PROCESSORKERNEL_TOWER_H
#define PROCESSORKERNEL_TOWER_H     1

// STL includes
#include <map>
#include <vector>
#include <utility>  // For std::pair
#include <boost/dynamic_bitset.hpp>
#include "ProcessorKernel/Register.h"
#include "Kernel/MuonTileID.h"
#include "L0MuonKernel/MuonCandidate.h"
#include "L0MuonKernel/L0MPtLUT.h"

namespace L0Muon {

  /** @class Tower Tower.h L0MuonKernel/Tower.h

    Class representing a uniform representation of the data belonging to a PU (corresponding to a tower)
    and implementing the candidate search algorithm.

    This class is used by CoreUnit to find the muon candidates in a PU.

    - Representation of data :
      \n\n
      The logical pads which have been fired are stored in an array of @link Tower::StationMap StationMap @endlink (#m_bittable).
      The tower contains 5 StationMaps (1 per station).
      They are filled in the CoreUnit::makePads method which make use of Tower::setBit.
      \n\n
      The tower dimensions are determined by the logical pads layout
      and by the fields of interest used by the muon candidate finding algorithm.
      \n\n
      The granularity is different from a muon station to an other.
      Thus, the number of logical pads in a trigger sector depends on the station.
      In the tower, all stations are bring to the finest granularity (M3).
      Therefore, 1 hit in M1 will light 2 bits in the tower's StationMap.
      \n\n
      In addition, the tower is extended w.r.t. a trigger sector to allow to search for hits
      which would be inside the field of interest but outside the trigger sector boundaries.
      In other words, the tower takes into account the data received by a PU from it's neighbours.
      Therefore, the number of rows and columns in each station depends on the field of interest.
      \n\n
      To size up the tower, the maximum allowed values of the field of interest are used.
      They are stored in the #m_maxXFoI and #m_maxYFoI attributes of the tower
      which are initialized in the @link Tower::Tower  default constructor @endlink
      (with values given in the granularity of M3).\n\n
      In conclusion, the tower (StationMaps) dimensions are :
      - M1: 4 x 72 (rows x columns)
      - M2: 4 x 34 (rows x columns)
      - M3: 4 x 24 (rows x columns)
      - M4: 6 x 48 (rows x columns)
      - M5: 6 x 48 (rows x columns)
      .
      \n
      Finally, in a station, @link Tower::IDMap IDMap @endlink describes the mapping
      between the coordinates of a hit in the tower and the corresponding MuonTileID.
      The tower contains an array of 5 IDMap (#m_idmap) (1 per station)
      which is filled in CoreUnit::makePads using Tower::setPadIdMap.
      \n

    - Candidate search algorithm and PT computation (in Tower::processTower) :
      \n\n
       Please refer to the LHCb-2002-042 note for details on the muon candidate finding algorithm.
       \n\n
       Some parameters allow to tune the algorithm are set in CoreUnit::initialize :
       - #m_xfoi, #m_yfoi the fields of interest in X and Y are set via Tower::setFoi
       - #m_ignoreM1, #m_ignoreM2 are set via Tower::ignoreM1 and Tower::ignoreM2
       - #m_procVersion is set via Tower::procVersion
       .
       \n
       The PT is computed using either:
       - a LUT for which a pointer is given in the constructor,
       - the formulae described in LHCb-2002-042 if the pointer is null.
       is ccan be com
       \n
       The algorithm parts depending on the processor version are implemented in functions defined in ProcUtilities.h :
       - order in which the hits are looked for in M1 and M2 is determined
         in ProcUtilities::pendulumM1 and ProcUtilities::pendulumM2
       - extrapolation of M3-M2 hits in M1 is done in ProcUtilities::extrapolationM1
       - pt computation done in ProcUtilities::kine where are specified the computation parameters
         which may depends on the processor version (Z position of stations and magnet pt kick).
       - pt encoding done in ProcUtilities::encodePT where are specified the encoding parameters
         which may depends on the processor version (bin width in MeV and number of bins).

    @author  Andrei Tsaregorodtsev, Julien Cogan
    @date  12 June 2003

  */
  class Tower  {
  public:

    /// Constructor
    Tower();

    /// Destructor
    ~Tower();

    typedef std::pair<int,int> HitIndex;
    /** A StationMap holds the hits for a station.
        It's a vector where each element is a bitset with one bit per column and corresponds to a row of the tower.
    */
    typedef std::vector<boost::dynamic_bitset<> > StationMap;
    typedef std::map< HitIndex, LHCb::MuonTileID> IDMap;

    /// Reset the tower
    void reset();

    /** Set bits in tower

        @param sta : station
        @param row : row in the tower
        @param col : column in the tower
    */
    void setBit(int sta, int row, int col);

    /** Map containing pads and their MuonTileID

        @param sta : station
        @param XY  : raw and column in tower
        @param mid : MuonTileID
     */
    void setPadIdMap(int sta, std::pair<int, int> XY, LHCb::MuonTileID mid);

    /** Return the MuonTileID of a pad in tower

        @param sta : station
        @param XY  : raw and column in tower
     */
    LHCb::MuonTileID getPadIdMap(int sta, std::pair<int,int> XY);

    /// Draw bits for station
    void drawStation(int sta);

    /** Return foi for constructing the tower
        (max. values in x direction)

        @param sta  : station
    */
    int maxXFoi(int sta) const { return m_maxXFoI[sta] ; }

    /** Return foi for constructing the tower
        (max. values in y direction)

        @param sta  : station
    */
    int maxYFoi(int sta) const { return m_maxYFoI[sta] ; }

    /** Set foi for searching candidates

        @param sta  : station
        @param xfoi : foi x value
        @param yfoi : foi y value
    */
    void setFoi(int sta, int xfoi, int yfoi) {
      m_xfoi[sta] = xfoi ;
      m_yfoi[sta] = yfoi ;
    }

    /** Set the processor version to use

        @param version : processor version
    */
    void setProcVersion(int version) {
      m_procVersion= version;
    }

    /** set flag for searching without M1
        @param ignoreM1 : if true, ignoreM1 in the processing
    */
    void setIgnoreM1(bool ignoreM1){
      m_ignoreM1 = ignoreM1;
    }

    /** set flag for searching without M2
        @param ignoreM2 : if true, ignoreM2 in the processing
    */
    void setIgnoreM2(bool ignoreM2){
      m_ignoreM2 = ignoreM2;
    }

    /** set pointer to LUT
        @param m_lut : pointer to the L0MPtLUT object holding the pt LUT
    */
    void setLUTPointer(L0MPtLUT * lut){
      m_lut = lut;
    }

    /// Draw tower
    void draw();

    /// Return the tower
    StationMap getTable(int sta){ return m_bittable[sta] ; }

    /** Search candidates

        @param puID  : MuonTileID of the PU
     */
    std::vector<PMuonCandidate> processTower(LHCb::MuonTileID & puID);

    /// Cleaning clusters of seeds in a tower
    void cleanSeed(StationMap & map) ;

    /// Function to activate the debug mode
    void setDebugMode(bool debug=true) {if (debug) std::cout << "Tower:setDebugMode\n";m_debug=debug;}

  private:

    int m_maxXFoI[5]; ///< FOI Max used to set the size of the tower (X)
    int m_maxYFoI[5]; ///< FOI Max used to set the size of the tower (Y)

    int m_xfoi[5]; ///< Foi in X
    int m_yfoi[5]; ///< Foi in Y

    StationMap m_bittable[5]; ///< Array of bits in each stations
    IDMap m_idmap[5];         ///< Map relating the local coordinates and the MuonTileID of every fired pad

    int m_procVersion; ///< processor version

    bool m_ignoreM1;   ///< Ignore M1 flag
    bool m_ignoreM2;   ///< Ignore M2 flag

    bool m_debug;      ///< Debug flag

    L0MPtLUT * m_lut;  ///< Pointer to the pt Look Up Table

};

  std::ostream& operator<<(std::ostream, L0Muon::Tower& r);

}  // namespace L0Muon

#endif      // PROCESSORKERNEL_TOWER_H







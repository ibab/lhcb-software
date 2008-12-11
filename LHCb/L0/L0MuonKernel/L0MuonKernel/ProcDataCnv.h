#ifndef L0MUONKERNEL_PROCDATACNV_H
#define L0MUONKERNEL_PROCDATACNV_H 


#include "Kernel/MuonTileID.h"
#include "ProcessorKernel/TileRegister.h"
#include <boost/dynamic_bitset.hpp>
#include <string>
#include <map>
#include <vector>
#include "L0MuonKernel/ProcDataErrors.h"

namespace L0Muon {
  
  /** @class ProcDataCnv ProcDataCnv.h  L0MuonKernel/ProcDataCnv.h

      Class to interface the data in L0MuonProcData bank with the L0Muon Register.
      
      The L0MuonProcData bank content is described in the L0Muon TELL1 specifications (see EDMS 818559).

      The L0MuonProcData bank contains data from all processing boards in a quarter (stored i #m_quarter).

      It holds pointers to the TileRegister instances containing the optical link and neighbour data 
      received by the PUs of a quarter.
      - FORMATTED_OL_Q<q>_<ib>_<ipu> (alias to FORMATTED_OL_Q<q>R<r><xy>)
      - FORMATTED_NEIGH_Q<q>_<ib>_<ipu> (alias to FORMATTED_NEIGH_Q>q>R<r><xy>)
      .
      where : 
      - q(=0,...,3) is the quarter
      - ib(=0,...,11) is the board index in the quarter
      - ipu(=0,...,3) is the pu index in the quarter
      - r(=0,...,3) is the region number
      - xy are the coordinate of the PU's MuonTileID.

      These pointers are stored in 2 different kinds of containers. 
      One is an array using the board and pu indices (#m_ols and #m_neighs).
      The other is a map using the PU MuonTileID as a key (#m_olsMap and #m_neighsMap).  

      It also owns the #m_errors attribute which stores the errors related to the L0MuonProcData bank
      (either errors reported by the hardware or which occured during the decoding itself) in ProcDataErrors objects.

      It implements methods to :
      - access the informations contained in the TileRegister instances described above :
        - list of MuonTileID with a hit in the optical link data (ProcDataCnv::ols)
        - list of MuonTileID with a hit in the neighbour data (ProcDataCnv::neighs)
      - fill these registers from the data in the L0MuonProcData bank (bank decoding) (ProcDataCnv::decodeBank)
      - fill the L0MuonProcData bank (ProcDataCnv::rawBank) (to do).
      It is used by the L0MuonOutput tool in the L0/L0Muon package (belonging to Lbcom project).

      @author Julien Cogan
      @date 2007-09-03
 
  */
  class ProcDataCnv {

  private:

    static const unsigned int PB_OpticalLinks_size = 24; ///< Number of words in the opt. link field of a processing board  
    static const unsigned int PB_Neighbours_size   = 34; ///<  Number of words in the neighbour field of a processing board
    static const unsigned int PU_OpticalLinks_size_16 = 10; ///< Number of words in the opt. link field of a PU
    static const unsigned int PU_Neighbours_size_16   = 17; ///<  Number of words in the neighbour field of a PU

  public:

    /// Default Constructor
    ProcDataCnv();

    /** Constructor with the quarter index 
        
    @param quarter : identifier of the quarter attached to this converter (0 to 3)
     */
    ProcDataCnv(int quarter);

    /// Destructor
    ~ProcDataCnv();

    /** Returns the MuonTileID of the BCSU in the given board

    @param ib : index of the board in the quarter (0 to 11)
    */
    LHCb::MuonTileID mid_BCSU(int ib);

    /** Returns the MuonTileID of the given PU in the given board

    @param ib : index of the board in the quarter (0 to 11)
    @param ipu : index of the pu in the board (0 to 3)
    */
    LHCb::MuonTileID mid_PU(int ib, int ipu);

    /// Clear the registers
    void release();

    /** Returns the list of MuonTileID hit in the optical link data of the given PU
        
    @param puid : MuonTileID of the PU
    */
    std::vector<LHCb::MuonTileID> ols(LHCb::MuonTileID puid);    

    /** Returns the list of MuonTileID hit in the neighbour data of the given PU
        
    @param puid : MuonTileID of the PU
    */
    std::vector<LHCb::MuonTileID> neighs(LHCb::MuonTileID puid);

    /// Returns the list of PU's MuonTileID in the quarter attached to this register
    std::vector<LHCb::MuonTileID> pus();

    /** Decode the L0MuonProcData bank. 

        From the data in the bank, it :
        - fills the TileRegisters with the optical link data of a quarter
        - fills the TileRegisters with the neighbour data of a quarter
        - fills the ProcDataErrors errors with the error encountered in the decoding 

        Return values :
        -  0 : incomplete bank
        - -1 : bad format (in 1st part of the bank)
        - -2 : bad format (in 2nd part of the bank)
        -  1 : only 1st first part was decoded
        -  2 : the two parts were decoded
        
        @param raw : vector containing the L0MuonCtrlCand bank data
        @param version : version of the L0MuonCtrlCand bank
    */
    int decodeBank(const std::vector<unsigned int> raw, int version);
    
    /** Writes the L0MuonProcData raw bank.

    @param raw : vector to be filled with the L0MuonProcData bank data
    @param version : version of the bank to encode 
    @param mode : output mode (0: write only the header, 1: write the 1st part, 2: write both part) 
    @param compression : flag to turn on the data compression
    */
    int rawBank(std::vector<unsigned int> &raw, int version,int mode, bool compression);

    /** Returns true if the given pu in the given processing board was found in error.
        
    @param ib : index of the board in the quarter (0 to 11)
    @param ipu : index of the pu in the board (0 to 3)
    */
    bool inError(int ib, int ipu) const { return m_errors[ib].inError(ipu);}

    /** Returns the decoding error code for the given processing board.
        
    @param ib : index of the board in the quarter (0 to 11)
    */
    int  decodingError(int ib) const { return ((int(m_errors[ib].decodingError())<<1)&2);}

    /** Returns the hardware error code for the given pu in the given processing board.
        
    @param ib : index of the board in the quarter (0 to 11)
    @param ipu : index of the pu in the board (0 to 3)
    */
    int  hardwareError(int ib, int ipu) const {return m_errors[ib].hardwareError(ipu);}

    /** Returns a pointer to the ProcDataErrors error handler for the given processing board.
        
    @param ib : index of the board in the quarter (0 to 11)
    */
    const ProcDataErrors * errors(int ib) const {return &m_errors[ib];}

  private:

    /// Decode bank version v1 - Obsolete as this version has never been used 
    int decodeBank_v1(const std::vector<unsigned int> &raw);

    /** Decode bank version v2. Called by ProcDataCnv::decodeBank
        
    @param raw : vector containing the L0MuonProcData bank data
    */ 
    int decodeBank_v2(const std::vector<unsigned int> &raw);

    /// Encode bank version v1 - Obsolete as this version has never been used 
    int rawBank_v1(std::vector<unsigned int> &raw, int mode);

    /** Encode bank version v2. Called by ProcDataCnv::rawBank

         TO BE IMPLEMENTED
        
    @param raw : vector to be filled the L0MuonProcData bank data
    @param mode : output mode (0: write only the header, 1: write the 1st part, 2: write both part) 
    @param compression : flag to turn on the data compression
    */ 
    int rawBank_v2(std::vector<unsigned int> &raw, int mode, bool compression);

    /** Apply the compression algorithm on the given word and fill the given bitset

    @param word : word to be compressed
    @param bitset : bitset updated with the compressed sequence
    */
    void word_to_compressedBitset(unsigned int word, boost::dynamic_bitset<> & bitset);
    
    /** Fill the TileRegister containing the optical link data of the given processing board with the given bitset.
        
    @param iboard : index of the board in the quarter (0 to 11)
    @param bitset : bitset representing the data
     */
    int setRegisters_for_PB_OpticalLinks(int iboard, boost::dynamic_bitset<> & rawbitset );

    /** Fill the TileRegister containing the neighbour data of the given processing board with the given bitset.
        
    @param iboard : index of the board in the quarter (0 to 11)
    @param bitset : bitset representing the data
     */
    int setRegisters_for_PB_Neighbours(int iboard, boost::dynamic_bitset<> & rawbitset );
    
    /** Decompress a slice of the L0MuonProcData bank corresponding to the optical link field
        or to the neighbour field of a processing board and fill a bitset with the decompressed data.

        The uncompressed data have a fixed size (#PB_OpticalLinks_size for OL and #PB_Neighbours_size for neighbour).
        
        This method skims through the input words starting from the word and the bit at the given indices.
        It reads the data bit by bit moving on inside the input data using ProcDataCnv::next_bit.
        It stops when the uncompressed data size reaches the given size.

        It applies the decompression algorithm, i.e. it reconstructs the original 32 bits word from a key (2 or 3 bits) 
        and eventually from an encoded sequences.

        @param raw : reference to the raw data vector
        @param size : size of the bitset to fill (#PB_OpticalLinks_size for OL and #PB_Neighbours_size for neighbour)
        @param word_index : index of first word of the slice in the raw data vector 
        @param bit_index : index of the first bit of the slice in the firt word 
    */
    int compressedPBWords_to_bitset(const std::vector<unsigned int> & raw, const int size, 
                                    unsigned int & word_index, int & bit_index,
                                    boost::dynamic_bitset<> & rawbitset);

    /** Consider an uncompressed slice of raw data in L0MuonProcData bank corresponding to the optical link field
        or to the neighbour field of a processing board and fill a bitset.

        @param raw : reference to the raw data vector
        @param size : size of the bitset to fill (#PB_OpticalLinks_size for OL and #PB_Neighbours_size for neighbour)
        @param word_index : index of first word of the slice in the raw data vector 
        @param bit_index : index of the first bit of the slice in the firt word 
    */
    int notcompressedPBWords_to_bitset(const std::vector<unsigned int> & raw, const int size, 
                                       unsigned int & word_index,
                                       boost::dynamic_bitset<> & rawbitset);

    /** Returns the next bit in the raw data and increment the indices of current word and bit.
        The raw data are parsed from 1st word to last word. Inside a word, one read from lsb to msb.

        @param raw  : reference to the raw data vector
        @param iwd  : reference to the index of the current word in the raw data vector
        @param ibit : reference to the index of the current bit in the current word 
     */
    int next_bit(const std::vector<unsigned int> & raw, unsigned int &iwd, int &ibit);

    /// apply the compression in bank v1 - obsolete as this bank was never used
    boost::dynamic_bitset<> applyCompression(boost::dynamic_bitset<> bitset_to_compress);

    /// unapply the compression in bank v1 - obsolete as this bank was never used
    boost::dynamic_bitset<> unapplyCompression(boost::dynamic_bitset<> & compressed, unsigned int length);

    int m_quarter; ///< index of the quarter attached to this converter

    // Pointers to tile registers organised into arrays according to board and pu indices
    TileRegister* m_ols[12][4];     ///< Array of pointers to TileRegister containing the optical link data;
                                    ///< the first index is the board number in the quarter,
                                    ///< the second the pu index in the board
    TileRegister* m_neighs[12][4];  ///< Array of pointers to TileRegister containing the neighbour data;
                                    ///< the first index is the board number in the quarter,
                                    ///<  the second the pu index in the board

    // Pointers to tile registers, organised into a map with the puID as a key
    std::map<LHCb::MuonTileID, TileRegister*> m_olsMap;   ///< Map of pointers to TileRegister containing the optical links data;
                                                          ///< the key is the MuonTileID of the PU
    std::map<LHCb::MuonTileID, TileRegister*> m_neighsMap;///< Map of pointers to TileRegister containing the neighbour data:
                                                          ///< the key is the MuonTileID of the PU
    
    ProcDataErrors m_errors[12]; ///< Array of error handler (one per processing board)

  };
}; // namespace L0Muon
 
#endif    // L0MUONKERNEL_PROCDATACNV_H

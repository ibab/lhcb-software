// $Id: VeloRawBankDecoder.h,v 1.2 2006-02-15 15:48:21 krinnert Exp $
#ifndef VELORAWBANKDECODER_H 
#define VELORAWBANKDECODER_H 1

#include <vector>


#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiADCWord.h"

#include "VeloDAQ/VeloRawBufferWord.h"

#include "VeloDAQ/VeloClusterWord.h"

// forward declaration needed for friendship with iterators
class VeloRawBankDecoder;

/** @class VeloRawBankDecoder VeloRawBankDecoder.h
 *  
 *  Decoder for raw Velo banks
 *
 *  This class provides two ways to decode the velo raw buffer.
 *  Either you can iterate over the cluster positions only, 
 *  using the pos_iterator,
 *  or you can iterate over cluster positions bundled with
 *  the ADC values of the strips contributing to the cluster.
 *  The latter method uses more CPU and memory.  
 *  So only use it when you really need the ADCs.  
 *  Otherwise (e.g. in the trigger) stick to the first method.
 *
 *  One instance of this class can decode exactly one bank, the
 *  one provided in the constructor argument.
 *
 *  This class is intentionally as independent from the LHCb/Gaudi
 *  framework as possible (it only uses some classes in Si/SiDAQ
 *  and Velo/VeloDAQ which are in turn independent of the framework).  
 *  This is because it will be used in standalone testing of Velo DAQ. 
 *  Don't break it.
 *
 *  @see VeloClusterWord
 *  @see SiHeaderWord
 *  @see SiADCWord
 * 
 *  @author Kurt Rinnert
 *  @date   2006-02-08
 */
class VeloRawBankDecoder {
public: 

  // live and death

  /** Constructor  
   *  Takes constant pointer to beginning of data content
   *  of a raw Velo bank.  The Constructor is non trrivial,
   *  it will imediately decode the bank header bytes.  So
   *  if you don't need it, don't create it.
   */
  VeloRawBankDecoder(const buffer_word* bank) :
    m_bank(bank),
    m_header(SiHeaderWord(bank[0])),
    m_nClusters(m_header.nClusters()),
    m_posBegin(pos_iterator(0,this)),
    m_posEnd(pos_iterator(m_nClusters,this)), 
    m_posAdcBegin(posadc_iterator(0,this)),
    m_posAdcEnd(posadc_iterator(m_nClusters,this))
  { 
    ;
  } 

  ~VeloRawBankDecoder() { ; } 

  // shortcuts
  
  /** @class VeloRawBankDecoder::VeloDecodedCluster VeloRawBankDecoder.h
   *  
   *  Simple type wrapper
   *
   *  This is just a simple type wrapper for a pair of a
   *  VeloClusterWord and a vector of corresponding SiADCWords.
   *
   *  @see VeloClusterWord
   *  @see SiADCWord
   * 
   *  @author Kurt Rinnert
   *  @date   2006-02-08
   */
  typedef std::pair<VeloClusterWord,std::vector<SiADCWord> > VeloDecodedCluster;

  // decoding iterators

  /** @class VeloRawBankDecoder::pos_iterator VeloRawBankDecoder.h
   *  
   *  Decoding iterator for cluster positions only
   *
   *  This smart iterator allows you to traverse a the cluster
   *  positions in the raw bank and decode them at the same
   *  time.  Use this if you do not need the ADC counts and 
   *  speed is a concern (e.g. in the trigger).
   *
   *  @see VeloClusterWord
   * 
   *  @author Kurt Rinnert
   *  @date   2006-02-02
   */
  class pos_iterator
  {
  public:
    
    // live and death
    
    pos_iterator() { ; }
    pos_iterator(const pos_iterator& init) : 
      m_pos(init.m_pos),
      m_decoder(init.m_decoder),
      m_cluster(init.m_cluster)
    { ; } 

    ~pos_iterator() { ; }

    // operators

    /// assignment
    const pos_iterator& operator=(const pos_iterator& rhs) 
    { 
      m_pos     = rhs.m_pos; 
      m_decoder = rhs.m_decoder;
      m_cluster = rhs.m_cluster; 

      return *this;
    }

    /**  Increment
     *   The implementatio of this increment operator
     *   is non-trivial.  It reads the next cluster
     *   position word from the raw bank and decodes it.
     *
     */
    const pos_iterator& operator++() const; 

    /** Equal 
     *  True if positions match 
     */
    bool operator==(const pos_iterator& rhs) const { return m_pos == rhs.m_pos; }

    /** Not Equal 
     *  True if positions do not match 
     */
    bool operator!=(const pos_iterator& rhs) const { return m_pos != rhs.m_pos; }

    // dereferencing

    /**  dereference to reference to VeloCLusterWord instance
     *   Gives access to the cluster word.  Note that the return value
     *   is a const reference.  The object it refers to does no longer
     *   exist once the iterator goes out of scope.  So if you need to hold
     *   a copy longer than that you have to initialise an instance. 
     *   I am well aware that this is a somewhat dangerous behaviour.  However,
     *   this is performance critical code.  So please simply refrain from
     *   creating instances and be aware of the lifetime of the returned
     *   reference.
     *
     * @see VeloClusterWord
     */
    const VeloClusterWord& operator*() const { return m_cluster; } 

    /**  dereference to pointer
     *   Gives access to pointer to cluster word
     *
     * @see VeloClusterWord
     */
    const VeloClusterWord* operator->() const { return &m_cluster; } 

  private:

    /**  Construct with position in raw bank reference to decoder
     *   Only friends (i.e. the decoder class are allowed
     *   to do that).  If applicable, the position will be
     *   decoded.
     *
     * @see VeloRawBankDecoder
     */
    pos_iterator(unsigned int pos, const VeloRawBankDecoder* decoder);

    /** Decode cluster position
     *  This method decodes the cluster position from
     *  the raw bank into a VeloClusterWord.
     *
     * @see VeloClusterWord
     */
    void decode() const;

  private:
    mutable unsigned int m_pos;
    const VeloRawBankDecoder* m_decoder;
    mutable VeloClusterWord m_cluster; 

    friend class VeloRawBankDecoder;
  };
  friend class pos_iterator;

  /** @class VeloRawBankDecoder::posadc_iterator VeloRawBankDecoder.h
   *  
   *  Decoding iterator for cluster positions only
   *
   *  This smart iterator allows you to traverse a the cluster
   *  positions and ADC counts in the raw bank and decode them at the same
   *  time.  Use this only if you really need the ADC counts an speed
   *  is not a concern.
   *
   *  @see VeloDecodedCluster
   *  @see VeloClusterWord
   *  @see SiADCWord
   * 
   *  @author Kurt Rinnert
   *  @date   2006-02-08
   */
  class posadc_iterator
  {
  public:
    
    // live and death
    
    posadc_iterator() { ; }
    posadc_iterator(const posadc_iterator& init) : 
      m_pos(init.m_pos),
      m_nADC(init.m_nADC),
      m_ADC32(init.m_ADC32),
      m_offset(init.m_offset),
      m_decoder(init.m_decoder),
      m_cluster(init.m_cluster)
    { ; } 

    ~posadc_iterator() { ; }

    // operators

    /// assignment
    const posadc_iterator& operator=(const posadc_iterator& rhs) 
    { 
      m_pos     = rhs.m_pos; 
      m_nADC    = rhs.m_nADC;
      m_ADC32   = rhs.m_ADC32;
      m_offset  = rhs.m_offset; 
      m_decoder = rhs.m_decoder;
      m_cluster = rhs.m_cluster; 

      return *this;
    }

    /**  Increment
     *   The implementatio of this increment operator
     *   is non-trivial.  It reads the next cluster
     *   position word from the raw bank and decodes it.
     *
     */
    const posadc_iterator& operator++() const; 

    /** Equal 
     *  True if positions match 
     */
    bool operator==(const posadc_iterator& rhs) const { return m_pos == rhs.m_pos; }

    /** Not Equal 
     *  True if positions do not match 
     */
    bool operator!=(const posadc_iterator& rhs) const { return m_pos != rhs.m_pos; }

    // dereferencing

    /**  dereference to reference to VeloDecodedCluster instance
     *   Gives access to the decoded cluster.  Note that the return value
     *   is a const reference.  The object it refers to does no longer
     *   exist once the iterator goes out of scope.  So if you need to hold
     *   a copy longer than that you have to initialise an instance. 
     *   I am well aware that this is a somewhat dangerous behaviour.  However,
     *   this is performance critical code.  So please simply refrain from
     *   creating instances and be aware of the lifetime of the returned
     *   reference.
     * 
     * @see VeloDecodedCluster
     */
    const VeloDecodedCluster& operator*() const { return m_cluster; } 

    /**  dereference to pointer
     *   Gives access to pointer to decoded cluster
     *
     * @see VeloDecodedCluster
     */
    const VeloDecodedCluster* operator->() const { return &m_cluster; } 

  private:

    /**  Construct with position in raw bank and reference to decoder
     *   Only friends (i.e. the decoder class are allowed
     *   to do that).  If applicable, the position is decoded.  
     *
     * @see VeloRawBankDecoder
     */
    posadc_iterator(unsigned int pos, const VeloRawBankDecoder* decoder);

    /** Decode cluster positiopn
     *  This method decodes the cluster position and ADC counts
     *  from the raw bank into a VeloDecodedCluster.
     *
     * @see VeloDecodedCluster
     */
    void decode() const;

  private:
    mutable unsigned int m_pos;
    mutable unsigned int m_nADC;
    mutable unsigned int m_ADC32;
    unsigned int m_offset;
    const VeloRawBankDecoder* m_decoder;
    mutable  VeloDecodedCluster m_cluster; 

    friend class VeloRawBankDecoder;
  };
  friend class posadc_iterator;


  // accessors

  /** Access decoded bank header  
   *
   * @see SiHeaderWord
   */
  const SiHeaderWord& header() const { return m_header; }

  /** Access number of clusters  
   *  Yields the total number of clusters encoded
   *  in the raw bank.
   * 
   * @return numbert of clusters in raw bank
   */
  unsigned int nClusters() const { return m_nClusters; }

  /// start iterator for cluster positions
  const pos_iterator& posBegin() const { return m_posBegin; }

  /// end iterator for cluster positions
  const pos_iterator& posEnd() const { return m_posEnd; }

  /// start iterator for decoded clusters with ADC values
  const posadc_iterator& posAdcBegin() const { return m_posAdcBegin; }

  /// end iterator for decoded clusters with ADC values
  const posadc_iterator& posAdcEnd() const { return m_posAdcEnd; }

private:
  const buffer_word*  m_bank;
  const SiHeaderWord  m_header;
  const unsigned int m_nClusters;

  const pos_iterator    m_posBegin;
  const pos_iterator    m_posEnd;
  const posadc_iterator m_posAdcBegin;
  const posadc_iterator m_posAdcEnd;

};
#endif // VELORAWBANKDECODER_H


//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************

#ifndef MuonEvent_MuonEventBits_H
#define MuonEvent_MuonEventBits_H 1

// Include files

/** @namespace MuonEventBase 
 *
 * general bit (un)packing set and get methods 
 *
 * @author unknown
 * created Thu Jan 12 16:46:43 2006
 * 
 */


namespace MuonEventBits
{
  /// generic get method for packed word in the muon event package 
  unsigned int getMuonBits(unsigned int packedValue,
                      unsigned int mask,
                      unsigned int shift);

  /// generic set method for packed word in the muon event package
  void setMuonBits(unsigned int& origValue,
              unsigned int shift,
              unsigned int mask,
              unsigned int bitValue);

};

// -----------------------------------------------------------------------------
//   end of namespace
// -----------------------------------------------------------------------------

// Including forward declarations

inline unsigned int MuonEventBits::getMuonBits(unsigned int packedValue,
                                          unsigned int mask,
                                          unsigned int shift) 
{
  return ((packedValue&mask)>>shift); 
}

inline void MuonEventBits::setMuonBits(unsigned int& origValue,
                                  unsigned int shift,
                                  unsigned int mask,
                                  unsigned int bitValue) 
{
  unsigned int store   = origValue;
                    unsigned int result=(((bitValue<<shift)&mask)| (store&~mask));
             	      origValue=result;		
}


#endif ///MuonEvent_MuonEventBits_H

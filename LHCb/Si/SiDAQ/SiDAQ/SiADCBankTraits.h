// $Id: SiADCBankTraits.h,v 1.1 2006-02-22 17:21:15 krinnert Exp $
#ifndef SIDAQ_SIADCBANKTRAITS_H 
#define SIDAQ_SIADCBANKTRAITS_H 1


namespace SiDAQ {

  /** @struct adc_only_bank_tag SiADCBankTraits.h SiDAQ/SiADCBankTraits.h
   *
   *  Flags raw adc bank without neighbour sum byte.
   *
   *  This class is used implement a traits pattern needed to
   *  distinguish ST and Velo raw ADC banks at compile time.
   *
   *  @author Kurt Rinnert
   *  @date   2006-02-21
   */
  struct adc_only_bank_tag {};

  /** @struct adc_neighboursum_bank_tag SiADCBankTraits.h SiDAQ/SiADCBankTraits.h
   *
   *  Flags raw adc bank with neighbour sum byte.
   *
   *  This class is used implement a traits pattern needed to
   *  distinguish ST and Velo raw ADC banks at compile time.
   *
   *  @author Kurt Rinnert
   *  @date   2006-02-21
   */
  struct adc_neighboursum_bank_tag {};

}
#endif // SIDAQ_SIADCBANKTRAITS_H

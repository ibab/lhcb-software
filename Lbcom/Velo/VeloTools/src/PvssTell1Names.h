// $Id: PvssTell1Names.h,v 1.1 2009-09-08 16:44:19 krinnert Exp $
#ifndef PVSSTELL1NAMES_H 
#define PVSSTELL1NAMES_H 1

#include <map>
#include <vector>

#include "GaudiAlg/GaudiTool.h"
#include "Tell1Kernel/IPvssTell1Names.h" 
#include "Tell1Kernel/PvssTell1NameMap.h" 


namespace Velo
{
  /** @class PvssTell1Names PvssTell1Names.h
   *
   *  Implementation of PvssTell1Map interface.
   *
   *  Provides access to PVSS TELL1 names from sensor
   *  numbers.
   *
   *  @author Kurt Rinnert
   *  @date   2009-09-08
   */
  class PvssTell1Names : public GaudiTool, virtual public Velo::IPvssTell1Names {

    public: 
      /// Standard constructor
      PvssTell1Names( const std::string& type, 
          const std::string& name,
          const IInterface* parent);

      virtual ~PvssTell1Names( ); ///< Destructor

      virtual StatusCode initialize(); 
      virtual StatusCode finalize(); 

      virtual const std::string& pvssName( unsigned int sensorNumber ); ///< retrive PVSS TELL1 name

    private:

      // time stamps
      Velo::PvssTell1NameMap m_pvssNames; 

  };
}
#endif // PVSSTELL1NAMES_H 

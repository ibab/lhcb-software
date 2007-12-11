#ifndef VeloTELL1Event_VeloTELL1FData_H
#define VeloTELL1Event_VeloTELL1FData_H 1

// Include files
#include "GaudiKernel/KeyedObject.h"
#include "GaudiKernel/KeyedContainer.h"
#include "GaudiKernel/boost_allocator.h"
#include "GaudiKernel/SerializeSTL.h"
#include <ostream>

// Forward declarations

namespace VeloTELL1
{
  // Namespace for locations in TDS
  namespace VeloTELL1FDataLocation {
    static const std::string& Default = "Raw/Velo/FNoise";
    static const std::string& FCMSNoise = "Raw/Velo/CMSNoise";
  }  

  /** @class VeloTELL1FData VeloTELL1FData.h
   *
   * The VeloTELL1FData is keyed data object, with sensor number as a key, and is
   * meant to store non-zero suppressed data (2304 ADC samples, 2048 pedestals or
   * 256 Headers). Ojects stored are of type float, key of the object is source
   * id of the TELL1
   *
   * @author Tomasz Szumlak
   * created Mon Nov 12 14:07:31 2007
   * 
   */

  class VeloTELL1FData: public KeyedObject<int>
  {
  public:

    /// Const iterator
    typedef std::vector<float>::const_iterator cFIT;
    /// Pair of const iterators
    typedef std::pair<cFIT, cFIT> ALinkPairF;
  
    /// Configuration constants depending on stored data type
    enum NZSDataType{ VeloFull=18,
                      VeloPedestal=30,
                      WriteHeaders=40
      };
    /// Different bank content
    enum ALinkContent{ HEADERS=4,
                       FPGA_ALINKS=16,
                       ADC_PER_ALINK=32,
                       SENSOR_ALINKS=64
      };
    /// Sensor types
    enum SensType{ PHI_SENSOR=1,
                   R_SENSOR=2,
                   PILE_UP=3
      };
  
    /// constructor with key
    VeloTELL1FData(const int numberOfTELL1,
                   const int dataType): KeyedObject<int>(numberOfTELL1),
                                        m_decodedData(),
                                        m_dataType(dataType),
                                        m_isReordered(false),
                                        m_sensorType(0) {}  
    /// Default Constructor
    VeloTELL1FData(): m_decodedData(),
                      m_dataType(0),
                      m_isReordered(),
                      m_sensorType(0) {}  
    /// Default Destructor
    virtual ~VeloTELL1FData() {}  
    /// overloaded subscripting operator
    ALinkPairF operator[](const unsigned int ALinkNumber) const;  
    /// Returns true if R type
    bool isR() const;  
    /// Returns true if Pile-Up type
    bool isPileUp() const;  
    /// Returns true if Phi type
    bool isPhi() const;  
    /// returns value of the charge deposited in a given channel
    float channelADC(const unsigned int channel) const;
    /// returns adc value for a given strip - valid after reordering
    float stripADC(const unsigned int strip) const;  
    /// returns vector with all decoded samples
    std::vector<float>& data();
    /// fill the vector with adc Velo data, use restricted within TELL1 algorithm
    void addALink(std::vector<float>& inVec);  
    /// Update  Vector with decoded data
    void setDecodedData(const std::vector<float>& value);  
    /// Retrieve const  Name of last process
    bool isReordered() const;  
    /// Update  Name of last process
    void setIsReordered(bool value);  
    /// Update  Sensor Type
    void setSensorType(unsigned int value);
    /// get ALink number using channel number
    unsigned int channel2ALink(const unsigned int channel) const;
  #ifndef _WIN32
    /// operator new
    static void* operator new ( size_t size )
    {
      return ( sizeof(VeloTELL1FData) == size ? 
               boost::singleton_pool<VeloTELL1FData, sizeof(VeloTELL1FData)>::malloc() :
               ::operator new(size) );
    }
  
    /// placement operator new
    /// it is needed by libstdc++ 3.2.3 (e.g. in std::vector)
    /// it is not needed in libstdc++ >= 3.4
    static void* operator new ( size_t size, void* pObj )
    {
      return ::operator new (size,pObj);
    }
  
    /// operator delete
    static void operator delete ( void* p )
    {
      boost::singleton_pool<VeloTELL1FData, sizeof(VeloTELL1FData)>::is_from(p) ?
      boost::singleton_pool<VeloTELL1FData, sizeof(VeloTELL1FData)>::free(p) :
      ::operator delete(p);
    }
  
    /// placement operator delete
    /// not sure if really needed, but it does not harm
    static void operator delete ( void* p, void* pObj )
    {
      ::operator delete (p, pObj);
    }
  #endif
  protected:

  private:

    /// Strips
    enum strips{ INNER_DUMMY_1=21,
                 INNER_DUMMY_2=22,
                 OUTER_DUMMY_1=42,
                 OUTER_DUMMY_2=43,
                 DUMMY_CHANNELS=64,
                 INNER_DUMMY=85,
                 INNER_STRIPS_1=170,
                 INNER_STRIPS_2=171,
                 OUTER_STRIPS_1=341,
                 OUTER_STRIPS_2=342,
                 INNER_STRIPS=683,
                 PPFPGA_CHANNELS=512,
                 SENSOR_STRIPS=2048,
                 ALL_STRIPS=2304
      };
  
    std::vector<float> m_decodedData; ///< Vector with decoded data
    int                     m_dataType;    ///< Type of decoded data to be stored
    bool                    m_isReordered; ///< Name of last process
    unsigned int            m_sensorType;  ///< Sensor Type
  
  }; // class VeloTELL1FData

  /// Definition of Keyed Container for VeloTELL1FData
  typedef KeyedContainer<VeloTELL1FData, Containers::HashMap> VeloTELL1FDatas;
  
} // namespace VeloTELL1;
//--
inline void VeloTELL1::VeloTELL1FData::setDecodedData(const std::vector<float>& value) 
{
  m_decodedData = value;
}
//--
inline bool VeloTELL1::VeloTELL1FData::isReordered() const 
{
  return m_isReordered;
}
//--
inline void VeloTELL1::VeloTELL1FData::setIsReordered(bool value) 
{
  m_isReordered = value;
}
//--
inline void VeloTELL1::VeloTELL1FData::setSensorType(unsigned int value) 
{
  m_sensorType = value;
}
//--
inline VeloTELL1::VeloTELL1FData::ALinkPairF VeloTELL1::VeloTELL1FData::operator[](const unsigned int ALinkNumber) const 
{
      int ALinkBegin=0, ALinkEnd=0;
      // this only makes sense before reordering
      if(!m_isReordered){
        // ALink must be in range (0 - 64)
        if(ALinkNumber<SENSOR_ALINKS){
          // return iterators according to stored data format
          if(m_dataType==WriteHeaders){
            ALinkBegin=ALinkNumber*HEADERS;
            ALinkEnd=ALinkBegin+HEADERS;
          }else if(m_dataType==VeloPedestal){
            ALinkBegin=ALinkNumber*ADC_PER_ALINK;
            ALinkEnd=ALinkBegin+ADC_PER_ALINK;
          }else if(m_dataType==VeloFull){
           unsigned int processor=ALinkNumber/FPGA_ALINKS;
           ALinkBegin=(ALinkNumber+2*processor)*ADC_PER_ALINK;
           ALinkEnd=ALinkBegin+ADC_PER_ALINK;
          }
        }
      }
      // if reordering is done two identical iterators will be returned
      ALinkPairF aPair;
      aPair.first=m_decodedData.begin()+ALinkBegin;
      aPair.second=m_decodedData.begin()+ALinkEnd;
      //
      return ( aPair );     
}
//--
inline bool VeloTELL1::VeloTELL1FData::isR() const 
{
      if(m_isReordered&&m_sensorType==R_SENSOR){
        return ( true );
      }else{
        return ( false );
      }     
}
//--
inline bool VeloTELL1::VeloTELL1FData::isPileUp() const 
{
      if(m_isReordered&&m_sensorType==PILE_UP){
        return ( true );
      }else{
        return ( false );
      }     
}
//--
inline bool VeloTELL1::VeloTELL1FData::isPhi() const 
{
      if(m_isReordered&&m_sensorType==PHI_SENSOR){
        return ( true );
      }else{
        return ( false );
      }    
}
//--
inline float VeloTELL1::VeloTELL1FData::channelADC(const unsigned int channel) const 
{
       float adc=0;
       // call it only before reordering
       if(!m_isReordered&&m_dataType==VeloFull){
         if(channel<SENSOR_STRIPS){
           // data vector contains also dummy strips - need to translate
           unsigned int processor=channel/PPFPGA_CHANNELS;
           unsigned int index=channel+DUMMY_CHANNELS*processor;
           adc=m_decodedData[index];
         }
       }
       return ( adc );      
}
//--
inline float VeloTELL1::VeloTELL1FData::stripADC(const unsigned int strip) const 
{
       float adc=0;
       if(m_isReordered&&strip<SENSOR_STRIPS&&m_dataType==VeloFull){
         if(m_sensorType==R_SENSOR||m_sensorType==PILE_UP){
           unsigned int processor=strip/PPFPGA_CHANNELS;
           unsigned int index=strip+DUMMY_CHANNELS*processor;
           adc=m_decodedData[index];
         }else if(m_sensorType==PHI_SENSOR){
           if(strip<INNER_STRIPS){
             if(strip<INNER_STRIPS_1){
               adc=m_decodedData[strip];
             }else if(strip<INNER_STRIPS_1+INNER_STRIPS_2){
               unsigned int index=strip+INNER_DUMMY_1;
               adc=m_decodedData[index];
             }else if(strip<INNER_STRIPS_1+2*INNER_STRIPS_2){
               unsigned int index=strip+INNER_DUMMY_1+INNER_DUMMY_2;
               adc=m_decodedData[index];
             }else if(strip<INNER_STRIPS_1+3*INNER_STRIPS_2){
               unsigned int index=strip+INNER_DUMMY_1+2*INNER_DUMMY_2;
               adc=m_decodedData[index];
             }
          }else{
             if(strip<INNER_STRIPS+OUTER_STRIPS_1){
               unsigned int index=strip+INNER_DUMMY; 
               adc=m_decodedData[index];
             }else if(strip<INNER_STRIPS+OUTER_STRIPS_1+OUTER_STRIPS_2){
               unsigned int index=strip+INNER_DUMMY+OUTER_DUMMY_1;
               adc=m_decodedData[index];
             }else if(strip<INNER_STRIPS+2*OUTER_STRIPS_1+OUTER_STRIPS_2){
               unsigned int index=strip+INNER_DUMMY+OUTER_DUMMY_1+OUTER_DUMMY_2;
               adc=m_decodedData[index];
             }else if(strip<INNER_STRIPS+3*OUTER_STRIPS_1+OUTER_STRIPS_2){
               unsigned int index=strip+INNER_DUMMY+OUTER_DUMMY_1+2*OUTER_DUMMY_2;
               adc=m_decodedData[index];
             }
           }
         }
       }
       return ( adc );     
}
//--
inline std::vector<float>& VeloTELL1::VeloTELL1FData::data() 
{
      return ( m_decodedData );    
}
//--
inline void VeloTELL1::VeloTELL1FData::addALink(std::vector<float>& inVec) 
{
       std::vector<float>::iterator datIt;
       datIt=inVec.begin();
       for( ; datIt!=inVec.end(); datIt++){
         m_decodedData.push_back(*datIt);
       }
       assert(m_decodedData.size()<=SENSOR_STRIPS);     
}
//--
inline unsigned int VeloTELL1::VeloTELL1FData::channel2ALink(const unsigned int channel) const
{
  unsigned int aLink=0;
  if(!m_isReordered&&channel<SENSOR_STRIPS){
    aLink=channel/ADC_PER_ALINK;
  }
  return ( aLink );
}
#endif ///VeloTELL1Event_VeloTELL1FData_H

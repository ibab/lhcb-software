/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// ===========================================================================
#ifndef          GIGA_GIGASTREAM_H
#define          GIGA_GIGASTREAM_H  1 
/// ===========================================================================
/// from STL
#include <string>
#include <vector>
/// base class from Gaudi 
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataSelector.h" 
#include "GaudiKernel/DataStoreItem.h"
/// forward declarations 
template <class ALGORITHM> class AlgFactory; 
class IDataProviderSvc;
class IDataManagerSvc;
class IServiceLocator;
class IConversionSvc; 

/** @class GiGaStream  GiGaStream.h 
 *
 *  simple utility to move data to and from GiGa
 *
 *  @author Vanya Belyaev
 */


class GiGaStream: public Algorithm
{
  ///  
  friend class AlgFactory<GiGaStream>;
  ///
 public:
  ///
  typedef std::vector<DataStoreItem>  Items; 
  typedef std::vector<std::string>    Names; 
  ///
 protected:
  
  /// constructor 
  GiGaStream( const std::string& StreamName    , 
              ISvcLocator*       SericeLocator ) ;
  /// virtual destructor 
  virtual ~GiGaStream(){}; 

 public: 

  /// Standard methods from Algorithm interface 
  virtual StatusCode initialize() ; 
  virtual StatusCode execute   () ; 
  virtual StatusCode finalize  () ; 

 protected: 
  
  /// Load objects pointed by Item  and put it into Selector 
  StatusCode LoadObject( const DataStoreItem& Item , 
                         IDataSelector* Selector );
  
  /// Load content of directory and put it into Selector 
  StatusCode LoadObject( IDataDirectory* Object  , 
                         const int Level , 
                         IDataSelector* Selector );
  /// Miscellaneous function to simplify the typing 
  StatusCode Error( const std::string& message , 
                    const StatusCode& status  = StatusCode::FAILURE ); 
  
 private: 

  bool                        m_ExecuteOnce            ; 
  bool                        m_Execute                ;  

  std::string                 m_NameOfConversionSvc    ; 
  IConversionSvc*             m_ConversionSvc          ; 
  
  std::string                 m_NameOfDataProviderSvc  ; 
  IDataProviderSvc*           m_DataProviderSvc        ;

  std::string                 m_NameOfDataManagerSvc   ; 
  IDataManagerSvc*            m_DataManagerSvc         ;
  
  Names                       m_NamesOfStreamItems     ;
  IDataSelector               m_DataSelector           ;  
  Items                       m_StreamItems            ;

  bool m_FillGiGaStream ;  ///< flag to distinguish GiGa input and output 

};

/// ===========================================================================
#endif    ///< GIGA_GIGASTREAM_H
/// ===========================================================================







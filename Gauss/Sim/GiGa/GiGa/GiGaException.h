#ifndef        __GIGA_GIGAEXCEPTION_GIGAEXCEPTION_H__
#define        __GIGA_GIGAEXCEPTION_GIGAEXCEPTION_H__ 1 

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *   base classes         (from STL)                                       * ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

#include <string> 
#include <memory>

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *   base classes         (from Gaudi framework)                           * ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/GaudiException.h" 
#include "GaudiKernel/Stat.h" 

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *************************************************************************** ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
///////////////////////////////////////////////////////////////////////////////////
///                                                                             ///
/// *************************************************************************** ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *            Geant4 Interface for Gaudi  Applications                     * ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *            Gaudi  Interface for Geant4 Applications                     * ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *************************************************************************** ///
///                                                                             ///
///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *************************************************************************** ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///                                                                             ///
///    class GiGaException : a simple definition of exception class to be used  ///      
///                          inside inmplementation of GiGa Service             ///
///                                                                             ///
///    Author: Vanya Belyaev                                                    ///
///                                                                             ///
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////



class GiGaException : public GaudiException 
{  
 
 public:

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *           constructor                                                 * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///  
  GiGaException( const std::string& message                          ,          ///
             const StatusCode & sc       = StatusCode::FAILURE   )          ///
    : GaudiException( message , "*GiGaException*" , sc ) {};                    ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *           constructor                                                 * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///  
  GiGaException(  const std::string   &  message                            ,   ///
                  const GaudiException&  Exception                          ,   ///
                  const StatusCode    &  sc           = StatusCode::FAILURE )   ///
    : GaudiException( message , "*GiGaException*" , sc , Exception ) {};        ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////
  
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *           (virtual) destructor                                        * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  virtual ~GiGaException(){};                                                   /// 
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////
  

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *            clone method                                               * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  virtual inline GaudiException* clone() const                                  ///
    { return  new(std::nothrow) GiGaException(*this); };                        ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////

};

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *************************************************************************** ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
///////////////////////////////////////////////////////////////////////////////////
///                                                                             ///
/// *************************************************************************** ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *            Geant4 Interface for Gaudi  Applications                     * ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *            Gaudi  Interface for Geant4 Applications                     * ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *************************************************************************** ///
///                                                                             ///
///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *************************************************************************** ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *  Several useful MACROS                                                  * ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////
#define ___GIGA_TRY___                   try

#define ___GIGA_THROW_GIGA____(_tag,_method,_exception) \
         { std::string _str(_tag); \
        _str += " GiGaException is catched "; \
        if( "" != _method && " " != _method ) { _str += " from " ; _str += _method ; } \
        throw GiGaException( _str, _exception ); } 

#define ___GIGA_THROW_GAUDI___(_tag,_method,_exception) \
         { std::string _str(_tag); \
        _str += " GaudiException with tag="; \
        _str += _exception.tag(); \
        _str += " is catched "; \
        if( "" != _method && " " != _method ) { _str += " from " ; _str += _method ; } \
        throw GiGaException( _str, _exception ); } 

#define ___GIGA_THROW_STD_____(_tag,_method,_exception) \
         { std::string _str(_tag); \
        _str += " std::exception with what="; \
        _str += _exception.what(); \
        _str += " is catched "; \
        if( "" != _method && " " != _method ) { _str += " from " ; _str += _method ; } \
        throw GiGaException( _str ); } 

#define ___GIGA_THROW_ALL_____(_tag,_method) \
         { std::string _str(_tag); \
        _str += " UNKNOWN exception "; \
        _str += " is catched "; \
        if( "" != _method && " " != _method ) { _str += " from " ; _str += _method ; } \
        throw GiGaException( _str ); } 


#define ___GIGA_PRINT_GIGA____(_tag,_method,_ms,_ss,_ex)   \
         { std::string _str(_tag); \
        _str += " GiGaException is catched "; \
        if( "" != _method && " " != _method ) { _str += " from " ; _str += _method ; } \
        MsgStream _log1( _ms , _tag ); \
        MsgStream _log2( _ms , _ex.tag() ); \
        _log1 << MSG::ERROR << _str << " : " << endreq; \
        _log2 << MSG::ERROR << _ex  << endreq; \
        Stat _stat( _ss , _ex.tag() ); }             

#define ___GIGA_PRINT_GAUDI___(_tag,_method,_ms,_ss,_ex)   \
         { std::string _str(_tag); \
        _str += " GaudiException with tag="; \
        _str += _ex.tag(); \
        _str += "is catched "; \
        if( "" != _method && " " != _method ) { _str += " from " ; _str += _method ; } \
        MsgStream _log1( _ms , _tag ); \
        MsgStream _log2( _ms , _ex.tag() ); \
        _log1 << MSG::ERROR << _str << " : " << endreq; \
        _log2 << MSG::ERROR << _ex  << endreq; \
        Stat _stat( _ss , _ex.tag() ); }             

#define ___GIGA_PRINT_STD_____(_tag,_method,_ms,_ss,_ex)   \
         { std::string _str(_tag); \
        _str += " std::exception with what="; \
        _str += _ex.what(); \
        _str += "is catched "; \
        if( "" != _method && " " != _method ) { _str += " from " ; _str += _method ; } \
        MsgStream _log1( _ms , _tag ); \
        MsgStream _log2( _ms , _ex.what() ); \
        _log1 << MSG::ERROR << _str << " : " << endreq; \
        _log2 << MSG::ERROR << "*std::exception* what=" << _ex.what()  << endreq; \
        Stat _stat( _ss , "*std::exception*" ); }             

#define ___GIGA_PRINT_ALL_____(_tag,_method,_ms,_ss)   \
         { std::string _str(_tag); \
        _str += " UNKNOWN exception "; \
        _str += "is catched "; \
        if( "" != _method && " " != _method ) { _str += " from " ; _str += _method ; } \
        MsgStream _log1( _ms , _tag ); \
        MsgStream _log2( _ms , "*UNKNOWN exception*" ); \
        _log1 << MSG::ERROR << _str << " : " << endreq; \
        _log2 << MSG::ERROR << "*UNKNOWN exception*"  << endreq; \
        Stat _stat( _ss , "*UNKNOWN exception*" ); }             

#define ___GIGA_CATCH_GIGA____(_body)     catch( const GiGaException  & _ex ){ _body; }

#define ___GIGA_CATCH_GAUDI___(_body)     catch( const GaudiException & _ex ){ _body; } 

#define ___GIGA_CATCH_STD_____(_body)     catch( const std::exception & _ex ){ _body; } 

#define ___GIGA_CATCH_ALL_____(_body)     catch(...){ _body; } 


#define ___GIGA_CATCH_AND_THROW_GIGA____(_tag,_method) \
        catch( const GiGaException  & _ex ) { ___GIGA_THROW_GIGA____(_tag,_method,_ex) ; }

#define ___GIGA_CATCH_AND_THROW_GAUDI___(_tag,_method) \
        catch( const GaudiException & _ex ) { ___GIGA_THROW_GAUDI___(_tag,_method,_ex) ; }

#define ___GIGA_CATCH_AND_THROW_STD_____(_tag,_method) \
        catch( const std::exception & _ex ) { ___GIGA_THROW_STD_____(_tag,_method,_ex) ; } 

#define ___GIGA_CATCH_AND_THROW_ALL_____(_tag,_method) \
        catch(...)                          { ___GIGA_THROW_ALL_____(_tag,_method)     ; }


#define ___GIGA_CATCH_AND_PRINT_GIGA____(_tag,_method,_ms,_ss) \
        catch( const GiGaException  & _ex ) { ___GIGA_PRINT_GIGA____(_tag,_method,_ms,_ss,_ex) ; }

#define ___GIGA_CATCH_AND_PRINT_GAUDI___(_tag,_method,_ms,_ss) \
        catch( const GaudiException & _ex ) { ___GIGA_PRINT_GAUDI___(_tag,_method,_ms,_ss,_ex) ; }

#define ___GIGA_CATCH_AND_PRINT_STD_____(_tag,_method,_ms,_ss) \
        catch( const std::exception & _ex ) { ___GIGA_PRINT_STD_____(_tag,_method,_ms,_ss,_ex) ; }

#define ___GIGA_CATCH_AND_PRINT_ALL_____(_tag,_method,_ms,_ss) \
        catch(...)                          { ___GIGA_PRINT_ALL_____(_tag,_method,_ms,_ss)     ; }          


#define ___GIGA_CATCH_PRINT_AND_RETURN_GIGA____(_tag,_method,_ms,_ss,_sc) \
        catch( const GiGaException  & _ex ) { ___GIGA_PRINT_GIGA____(_tag,_method,_ms,_ss,_ex) ; return _sc ; }

#define ___GIGA_CATCH_PRINT_AND_RETURN_GAUDI___(_tag,_method,_ms,_ss,_sc) \
        catch( const GaudiException & _ex ) { ___GIGA_PRINT_GAUDI___(_tag,_method,_ms,_ss,_ex) ; return _sc ; }

#define ___GIGA_CATCH_PRINT_AND_RETURN_STD_____(_tag,_method,_ms,_ss,_sc) \
        catch( const std::exception & _ex ) { ___GIGA_PRINT_STD_____(_tag,_method,_ms,_ss,_ex) ; return _sc ; }

#define ___GIGA_CATCH_PRINT_AND_RETURN_ALL_____(_tag,_method,_ms,_ss,_sc) \
        catch(...)                          { ___GIGA_PRINT_ALL_____(_tag,_method,_ms,_ss)     ; return _sc ; }


#define ___GIGA_CATCH___(_body) \
        ___GIGA_CATCH_GIGA____(_body) \
        ___GIGA_CATCH_GAUDI___(_body) \
        ___GIGA_CATCH_STD_____(_body) \
        ___GIGA_CATCH_ALL_____(_body) 

#define ___GIGA_CATCH_AND_RETURN___(_ret) \
        ___GIGA_CATCH____( return _ret )

#define ___GIGA_CATCH_AND_THROW___(_tag,_method) \
        ___GIGA_CATCH_AND_THROW_GIGA____(_tag,_method) \
        ___GIGA_CATCH_AND_THROW_GAUDI___(_tag,_method) \
        ___GIGA_CATCH_AND_THROW_STD_____(_tag,_method) \
        ___GIGA_CATCH_AND_THROW_ALL_____(_tag,_method)

#define ___GIGA_CATCH_AND_PRINT___(_tag,_method,_ms,_ss) \
        ___GIGA_CATCH_AND_PRINT_GIGA____(_tag,_method,_ms,_ss) \
        ___GIGA_CATCH_AND_PRINT_GAUDI___(_tag,_method,_ms,_ss) \
        ___GIGA_CATCH_AND_PRINT_STD_____(_tag,_method,_ms,_ss) \
        ___GIGA_CATCH_AND_PRINT_ALL_____(_tag,_method,_ms,_ss) 

#define ___GIGA_CATCH_PRINT_AND_RETURN___(_tag,_method,_ms,_ss,_sc) \
        ___GIGA_CATCH_PRINT_AND_RETURN_GIGA____(_tag,_method,_ms,_ss,_sc) \
        ___GIGA_CATCH_PRINT_AND_RETURN_GAUDI___(_tag,_method,_ms,_ss,_sc) \
        ___GIGA_CATCH_PRINT_AND_RETURN_STD_____(_tag,_method,_ms,_ss,_sc) \
        ___GIGA_CATCH_PRINT_AND_RETURN_ALL_____(_tag,_method,_ms,_ss,_sc) 


///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *************************************************************************** /// 
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
///////////////////////////////////////////////////////////////////////////////////
///                                                                             ///
/// *************************************************************************** ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *            Geant4 Interface for Gaudi  Applications                     * ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *            Gaudi  Interface for Geant4 Applications                     * ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *************************************************************************** ///
///                                                                             ///
///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *************************************************************************** ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////


#endif    //   __GIGA_GIGAEXCEPTION_GIGAEXCEPTION_H__













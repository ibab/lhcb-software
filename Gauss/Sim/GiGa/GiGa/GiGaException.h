// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/08/12 15:42:43  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.4  2001/07/23 13:11:42  ibelyaev
/// the package restructurisation(II)
///
/// Revision 1.3  2001/07/15 20:53:53  ibelyaev
/// package restructurisation
///
// ============================================================================
#ifndef GIGA_GIGAEXCEPTION_H 
#define GIGA_GIGAEXCEPTION_H 1
// ============================================================================

// Include files
// GaudiKernel
#include "GaudiKernel/GaudiException.h"

/** @class GiGaException  GiGaException.h GiGa/GiGaException.h
 *  
 *
 *  @author Ivan Belyaev
 *  @date   29/06/2001
 */

class GiGaException : public GaudiException
{
public:
  
  /**    constructor
         @param message    message to be associated with the exception 
         @param sc         status code to be associated with teh exception
  */
  GiGaException( const std::string    & message                          ,
		 const StatusCode     & sc       = StatusCode::FAILURE   ) ;
  
  /**    constructor
         @param message    message to be associated with the exception 
         @param Exception  "previous" exception 
         @param sc         status code to be associated with teh exception
  */
  GiGaException(  const std::string   &  message                         ,   
                  const GaudiException&  Exception                       ,   
                  const StatusCode    &  sc        = StatusCode::FAILURE ) ;  

  /// (virtual) destructor                                               
  virtual ~GiGaException() throw();
  
  /// clone method (virtual constructor)  
  virtual GaudiException* clone() const;
  
};

// ============================================================================
///  set of useful macros
// ============================================================================

#define ___GIGA_TRY___                   try

#define ___GIGA_THROW_GIGA____(_tag,_method,_exception) \
         { std::string _str(_tag); \
        _str += " GiGaException is catched "; \
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
        throw GiGaException( _str, _exception ); } 

#define ___GIGA_THROW_GAUDI___(_tag,_method,_exception) \
         { std::string _str(_tag); \
        _str += " GaudiException with tag="; \
        _str += _exception.tag(); \
        _str += " is catched "; \
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
        throw GiGaException( _str, _exception ); } 

#define ___GIGA_THROW_STD_____(_tag,_method,_exception) \
         { std::string _str(_tag); \
        _str += " std::exception with what="; \
        _str += _exception.what(); \
        _str += " is catched "; \
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
        throw GiGaException( _str ); } 

#define ___GIGA_THROW_ALL_____(_tag,_method) \
         { std::string _str(_tag); \
        _str += " UNKNOWN exception "; \
        _str += " is catched "; \
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
        throw GiGaException( _str ); } 


#define ___GIGA_PRINT_GIGA____(_tag,_method,_ms,_ss,_ex)   \
         { std::string _str(_tag); \
        _str += " GiGaException is catched "; \
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
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
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
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
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
        MsgStream _log1( _ms , _tag ); \
        MsgStream _log2( _ms , _ex.what() ); \
        _log1 << MSG::ERROR << _str << " : " << endreq; \
        _log2 << MSG::ERROR << "*std::exception* what=" \
       << _ex.what()  << endreq; \
        Stat _stat( _ss , "*std::exception*" ); }             

#define ___GIGA_PRINT_ALL_____(_tag,_method,_ms,_ss)   \
         { std::string _str(_tag); \
        _str += " UNKNOWN exception "; \
        _str += "is catched "; \
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
        MsgStream _log1( _ms , _tag ); \
        MsgStream _log2( _ms , "*UNKNOWN exception*" ); \
        _log1 << MSG::ERROR << _str << " : " << endreq; \
        _log2 << MSG::ERROR << "*UNKNOWN exception*"  << endreq; \
        Stat _stat( _ss , "*UNKNOWN exception*" ); }             

#define ___GIGA_CATCH_GIGA____(_body) catch( const GiGaException  & _ex ) \
                                           { _body; }

#define ___GIGA_CATCH_GAUDI___(_body) catch( const GaudiException & _ex ) \
                                           { _body; } 

#define ___GIGA_CATCH_STD_____(_body) catch( const std::exception & _ex ) \
                                           { _body; } 

#define ___GIGA_CATCH_ALL_____(_body) catch(...){ _body; } 


#define ___GIGA_CATCH_AND_THROW_GIGA____(_tag,_method) \
        catch( const GiGaException  & _ex ) \
        { ___GIGA_THROW_GIGA____(_tag,_method,_ex) ; }

#define ___GIGA_CATCH_AND_THROW_GAUDI___(_tag,_method) \
        catch( const GaudiException & _ex ) \
        { ___GIGA_THROW_GAUDI___(_tag,_method,_ex) ; }

#define ___GIGA_CATCH_AND_THROW_STD_____(_tag,_method) \
        catch( const std::exception & _ex ) \
        { ___GIGA_THROW_STD_____(_tag,_method,_ex) ; } 

#define ___GIGA_CATCH_AND_THROW_ALL_____(_tag,_method) \
        catch(...) \
        { ___GIGA_THROW_ALL_____(_tag,_method)     ; }


#define ___GIGA_CATCH_AND_PRINT_GIGA____(_tag,_method,_ms,_ss) \
        catch( const GiGaException  & _ex ) \
        { ___GIGA_PRINT_GIGA____(_tag,_method,_ms,_ss,_ex) ; }

#define ___GIGA_CATCH_AND_PRINT_GAUDI___(_tag,_method,_ms,_ss) \
        catch( const GaudiException & _ex ) \
        { ___GIGA_PRINT_GAUDI___(_tag,_method,_ms,_ss,_ex) ; }

#define ___GIGA_CATCH_AND_PRINT_STD_____(_tag,_method,_ms,_ss) \
        catch( const std::exception & _ex ) \
        { ___GIGA_PRINT_STD_____(_tag,_method,_ms,_ss,_ex) ; }

#define ___GIGA_CATCH_AND_PRINT_ALL_____(_tag,_method,_ms,_ss) \
        catch(...) \
        { ___GIGA_PRINT_ALL_____(_tag,_method,_ms,_ss)     ; }          


#define ___GIGA_CATCH_PRINT_AND_RETURN_GIGA____(_tag,_method,_ms,_ss,_sc) \
        catch( const GiGaException  & _ex ) \
        { ___GIGA_PRINT_GIGA____(_tag,_method,_ms,_ss,_ex) ; return _sc ; }

#define ___GIGA_CATCH_PRINT_AND_RETURN_GAUDI___(_tag,_method,_ms,_ss,_sc) \
        catch( const GaudiException & _ex ) \
        { ___GIGA_PRINT_GAUDI___(_tag,_method,_ms,_ss,_ex) ; return _sc ; }

#define ___GIGA_CATCH_PRINT_AND_RETURN_STD_____(_tag,_method,_ms,_ss,_sc) \
        catch( const std::exception & _ex ) \
        { ___GIGA_PRINT_STD_____(_tag,_method,_ms,_ss,_ex) ; return _sc ; }

#define ___GIGA_CATCH_PRINT_AND_RETURN_ALL_____(_tag,_method,_ms,_ss,_sc) \
        catch(...) \
        { ___GIGA_PRINT_ALL_____(_tag,_method,_ms,_ss)     ; return _sc ; }


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

// ============================================================================
///  set of useful macros
// ============================================================================

#define ___GIGA_TRY___                   try

#define ___GIGA_THROW_GIGA____(_tag,_method,_exception) \
         { std::string _str(_tag); \
        _str += " GiGaException is catched "; \
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
        throw GiGaException( _str, _exception ); } 

#define ___GIGA_THROW_GAUDI___(_tag,_method,_exception) \
         { std::string _str(_tag); \
        _str += " GaudiException with tag="; \
        _str += _exception.tag(); \
        _str += " is catched "; \
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
        throw GiGaException( _str, _exception ); } 

#define ___GIGA_THROW_STD_____(_tag,_method,_exception) \
         { std::string _str(_tag); \
        _str += " std::exception with what="; \
        _str += _exception.what(); \
        _str += " is catched "; \
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
        throw GiGaException( _str ); } 

#define ___GIGA_THROW_ALL_____(_tag,_method) \
         { std::string _str(_tag); \
        _str += " UNKNOWN exception "; \
        _str += " is catched "; \
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
        throw GiGaException( _str ); } 


#define ___GIGA_PRINT_GIGA____(_tag,_method,_ms,_ss,_ex)   \
         { std::string _str(_tag); \
        _str += " GiGaException is catched "; \
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
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
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
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
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
        MsgStream _log1( _ms , _tag ); \
        MsgStream _log2( _ms , _ex.what() ); \
        _log1 << MSG::ERROR << _str << " : " << endreq; \
        _log2 << MSG::ERROR << "*std::exception* what=" \
       << _ex.what()  << endreq; \
        Stat _stat( _ss , "*std::exception*" ); }             

#define ___GIGA_PRINT_ALL_____(_tag,_method,_ms,_ss)   \
         { std::string _str(_tag); \
        _str += " UNKNOWN exception "; \
        _str += "is catched "; \
        if( "" != _method && " " != _method ) \
        { _str += " from " ; _str += _method ; } \
        MsgStream _log1( _ms , _tag ); \
        MsgStream _log2( _ms , "*UNKNOWN exception*" ); \
        _log1 << MSG::ERROR << _str << " : " << endreq; \
        _log2 << MSG::ERROR << "*UNKNOWN exception*"  << endreq; \
        Stat _stat( _ss , "*UNKNOWN exception*" ); }             

#define ___GIGA_CATCH_GIGA____(_body) catch( const GiGaException  & _ex ) \
                                           { _body; }

#define ___GIGA_CATCH_GAUDI___(_body) catch( const GaudiException & _ex ) \
                                           { _body; } 

#define ___GIGA_CATCH_STD_____(_body) catch( const std::exception & _ex ) \
                                           { _body; } 

#define ___GIGA_CATCH_ALL_____(_body) catch(...){ _body; } 


#define ___GIGA_CATCH_AND_THROW_GIGA____(_tag,_method) \
        catch( const GiGaException  & _ex ) \
        { ___GIGA_THROW_GIGA____(_tag,_method,_ex) ; }

#define ___GIGA_CATCH_AND_THROW_GAUDI___(_tag,_method) \
        catch( const GaudiException & _ex ) \
        { ___GIGA_THROW_GAUDI___(_tag,_method,_ex) ; }

#define ___GIGA_CATCH_AND_THROW_STD_____(_tag,_method) \
        catch( const std::exception & _ex ) \
        { ___GIGA_THROW_STD_____(_tag,_method,_ex) ; } 

#define ___GIGA_CATCH_AND_THROW_ALL_____(_tag,_method) \
        catch(...) \
        { ___GIGA_THROW_ALL_____(_tag,_method)     ; }


#define ___GIGA_CATCH_AND_PRINT_GIGA____(_tag,_method,_ms,_ss) \
        catch( const GiGaException  & _ex ) \
        { ___GIGA_PRINT_GIGA____(_tag,_method,_ms,_ss,_ex) ; }

#define ___GIGA_CATCH_AND_PRINT_GAUDI___(_tag,_method,_ms,_ss) \
        catch( const GaudiException & _ex ) \
        { ___GIGA_PRINT_GAUDI___(_tag,_method,_ms,_ss,_ex) ; }

#define ___GIGA_CATCH_AND_PRINT_STD_____(_tag,_method,_ms,_ss) \
        catch( const std::exception & _ex ) \
        { ___GIGA_PRINT_STD_____(_tag,_method,_ms,_ss,_ex) ; }

#define ___GIGA_CATCH_AND_PRINT_ALL_____(_tag,_method,_ms,_ss) \
        catch(...) \
        { ___GIGA_PRINT_ALL_____(_tag,_method,_ms,_ss)     ; }          


#define ___GIGA_CATCH_PRINT_AND_RETURN_GIGA____(_tag,_method,_ms,_ss,_sc) \
        catch( const GiGaException  & _ex ) \
        { ___GIGA_PRINT_GIGA____(_tag,_method,_ms,_ss,_ex) ; return _sc ; }

#define ___GIGA_CATCH_PRINT_AND_RETURN_GAUDI___(_tag,_method,_ms,_ss,_sc) \
        catch( const GaudiException & _ex ) \
        { ___GIGA_PRINT_GAUDI___(_tag,_method,_ms,_ss,_ex) ; return _sc ; }

#define ___GIGA_CATCH_PRINT_AND_RETURN_STD_____(_tag,_method,_ms,_ss,_sc) \
        catch( const std::exception & _ex ) \
        { ___GIGA_PRINT_STD_____(_tag,_method,_ms,_ss,_ex) ; return _sc ; }

#define ___GIGA_CATCH_PRINT_AND_RETURN_ALL_____(_tag,_method,_ms,_ss,_sc) \
        catch(...) \
        { ___GIGA_PRINT_ALL_____(_tag,_method,_ms,_ss)     ; return _sc ; }


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


// ============================================================================
#endif // GIGA_GIGAEXCEPTION_H
// ============================================================================

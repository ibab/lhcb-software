/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlExprParser.cpp,v 1.1 2001-02-05 12:45:53 ranjard Exp $

// Include files
#include "DetDesc/XmlExprParser.h"
#include "GaudiKernel/MsgStream.h"
#include "DetDesc/XmlCnvException.h"

#include <cctype>
#include <cstring>

#include <string>

// Error messages of the parser
char *m_e[] = {
  "Syntax Error",
  "Unbalanced Parentheses",
  "No expression Present",
  "Division by zero",
  "Units not used inside the expression"
};

// Default Constructor
XmlExprParser::XmlExprParser( IMessageSvc* msgsvc )
: m_expression((char*)0), m_hasExponent(false), m_msgSvc(msgsvc), m_checkUnits(false) 
{
}

// Parser entry point.
double XmlExprParser::eval( char *exp, bool check ) {

  MsgStream log( m_msgSvc, "ExprParser" );
  
  double result;
  
  m_checkUnits = check;

  std::string expr    = exp;
  std::string pattern = "_clhep_unit_";
  unsigned long pos = 0;
  bool units = false;
  
  while( (pos = expr.find(pattern.c_str(), 0)) != std::string::npos )
  {
    expr  = expr.replace( pos, pattern.size(), "" );
    units = true;
  }

  m_expression = (char*)expr.c_str();
  getToken();

  if( m_checkUnits )
  {
    if( !units )
    {
      //serror(NOUNITS);
      std::string msg = m_e[NOUNITS];
      msg += " >>"; msg += m_token; msg += "<<";
      StatusCode sc; sc.setCode( EXPRESSION_ERROR );
      throw XmlCnvException(msg.c_str(),sc);
    }
  }

  if( !*m_token ) {
    serror( NOEXP );             // no expression present
    return 0.0;
  }

  evalExp2( result );

  if( *m_token ) {
    serror(SERROR); // last token must be null
  }

  return result;
}

// Add or subtract two terms.
void XmlExprParser::evalExp2( double &result ) {

  register char op;
  double temp;

  evalExp3( result );

  while( (op = *m_token) == '+' || op == '-' ) {
    
    getToken();
    evalExp3( temp );

    switch(op) {
      case '-':
        result = result - temp;
        break;
      case '+':
        result = result + temp;
        break;
    }
  }
}

// Multiply or divide two factors.
void XmlExprParser::evalExp3( double &result ) {

  register char op;
  double temp;

  evalExp4(result);
  
  while( (op = *m_token) == '*' || op == '/' || op == '%' ) {

    getToken();
    evalExp4( temp );

    switch( op ) {
      case '*':
        result = result * temp;
        break;
      case '/':
        if( !temp ) {
          serror(DIVZERO); // division by zero attempted
        } else      {
          result = result / temp;
        }
        break;
      case '%':
        result = (int) result % (int) temp;
        break;
    }
  }
}

// Process an exponent.
void XmlExprParser::evalExp4( double &result ) {

  double temp, ex;
  register int t;

  evalExp5( result );
  
  if(*m_token== '^') {

    getToken();
    evalExp4( temp );
    ex = result;

    if( temp==0.0 ) {
      result = 1.0;
      return;
    }

    for( t=(int)temp-1; t>0; --t ) {
      result = result * (double)ex;
    }
  }
}

// Evaluate a unary + or -.
void XmlExprParser::evalExp5( double &result ) {

  register char  op;

  op = 0;
  if( (m_tokenType == OPERATOR) && *m_token=='+' || *m_token == '-' ) {
    op = *m_token;
    getToken();
  }

  evalExp6( result );

  if( op=='-' ) result = -result;
}

// Process a parenthesized expression.
void XmlExprParser::evalExp6( double &result ) {
  
  if( (*m_token == '(') ) {

    getToken();
    evalExp2( result );
    
    if( *m_token != ')' ) {
      serror( PARENS );
    }

    getToken();
  } else                  {
    atom(result);
  }
}

// Get the value of a number
void XmlExprParser::atom( double &result ) {

  switch( m_tokenType ) {
    case NUMBER:
      result = atof( m_token );
      getToken();
      return;
    default:
      serror( SERROR );
  }
}

// Display a syntax error
void XmlExprParser::serror( errorsT error ) {
  MsgStream log( m_msgSvc, "ExprParser" );
  log << MSG::ERROR << m_e[error] << " >>" << m_token << "<<" << endreq;
}

// Obtain the next token
void XmlExprParser::getToken() {

  register char *temp;

  m_hasExponent = false;
  m_tokenType = UNDEFTOK;
  temp        = m_token;
  *temp       = '\0';

  if( !*m_expression ) return; // at end of expression

  while( isspace(*m_expression) )                                           {
    ++m_expression; // skip over white space
  }

  if( strchr( "+-*/%^=()", *m_expression ) )                                {
    m_tokenType = OPERATOR;
    // advance to next char
    *temp++ = *m_expression++;
  } else if( isdigit(*m_expression) )                                       {

    /*
    while( !isDelimiter( *m_expression ) ) {
      *temp++ = *m_expression++;
    }
    */

    bool signDone = false;
    bool isError  = false;

    while( true )                                                           {
      
      char c = *m_expression;
      
      if( strchr( " +-/*%^=()", c ) || c==9 || c=='\r' || c=='\t' || c==0 ) {
        
        if( ( c == '+' || c == '-') && m_hasExponent )                      {
          
          if( !signDone )                                             {
            signDone = true;
          } else                                                            {
            // Oops exponent can't have more then one sign +/-
            //break;
            isError = true;
          }

          *temp++ = *m_expression++;
          continue;
        }
        
        m_tokenType = NUMBER;
        break;
        
      } else if( c == 'e' || c == 'E' )                                     {
        
        if( !m_hasExponent )                                                {
          // No exponent found yet so we assume we got just one now
          m_hasExponent  = true;
        } else                                                              {
          // Oops, this can't be an exponent
          //break;
          isError = true;
        }
      } else if( isdigit( c ) || c == '.' )                                 {
        // OK, we accept digits and decimal point
        ;
      } else                                                                {
        // Oops, loks like not a digit
        //break;
        isError = true;
      }
      
      *temp++ = *m_expression++;
    }

    if( isError ) {
      m_tokenType = UNDEFTOK;
    }
  }

  *temp = '\0';
}

// Return true if c is a delimiter
bool XmlExprParser::isDelimiter( char c ) {

  if( strchr( " +-/*%^=()", c ) || c==9 || c=='\r' || c=='\t' || c==0 ) {

    if( ( c == '+' || c == '-') && m_hasExponent )                      {
      m_hasExponent = false;
      return false;
    }

    return true;

  } else if( c == 'e' || c == 'E' )                                     {

    if( !m_hasExponent )                                                {
      m_hasExponent = true;
    } else                                                              {
      m_hasExponent = false;
    }
  }

  return false;
}


/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/XmlExprParser.h,v 1.1.1.1 2000-11-23 13:44:44 ranjard Exp $
#ifndef DETDESC_XMLCNVSVC_XMLEXPRPARSER_H
#define DETDESC_XMLCNVSVC_XMLEXPRPARSER_H 1

#include <string>

// Forward declarations
class IMessageSvc;

//
// ClassName: XmlExprParser
//
// Description:  This class implements the recursive descent parser of numerical expressions
//               that do not use variables.
//
// Author: Radovan Chytracek ( Modification and integration inside Gaudi XML converters )
//         Original sources from the book: C/C++ Annotated Archives, Osborne McGraw-Hill,
//                                         1999, Art Friedman, Lars Klander, Mark Michaelis
//                                         and Herb Schildt
//

class XmlExprParser
{

  // Typedefs and local class declarations
  
  // Input token types
  enum typesT
  {
    UNDEFTOK,        // Undefined token
    OPERATOR,        // Operator token
    NUMBER           // Number token
  };
  
  // Parser error types
  enum errorsT
  {
    SERROR,          // Syntax error
    PARENS,          // Parenthesis error
    NOEXP,           // No expression error
    DIVZERO,         // Division by zero error
    NOUNITS
  };

public:

  // Constructor
  XmlExprParser( IMessageSvc* msgsvc );

  // Default destructor
  ~XmlExprParser() {
  }

  // Parser entry point
  double eval( char *exp, bool check = true );
  double eval( const char *exp, bool check = true );
  double eval( std::string exp, bool check = true );

private:

  // Add or subtract two terms
  void evalExp2( double &result );

  // Multiply or divide two factors
  void evalExp3( double &result );

  // Process an exponent
  void evalExp4( double &result );

  // Evaluate a unary + or -
  void evalExp5( double &result );

  // Process a parenthesized expression
  void evalExp6( double &result );
  
  // Get the value of a number
  void atom( double &result );
  
  // Obtain the next token
  void getToken();
  
  // Display a syntax error
  void serror( errorsT error );
  
  // Return true if c is a delimiter
  bool isDelimiter(char c);

private:

  char        *m_expression;     // Points to the expression to be parsed
  char        m_token[80];       // Holds current token
  typesT      m_tokenType;       // Holds token's type
  bool        m_hasExponent;     // Flag that (perhaps) exponent was found in the number

  IMessageSvc* m_msgSvc;         // The Gaudi message service, must be initialized
                                 // before passed into XmlExprParser
  bool        m_checkUnits;      // Inspect input expression for '_clhep_units' string
                                 // Recommended inside XML converters

};

// Parser entry point.
inline double XmlExprParser::eval( const char *exp, bool check ) {
  return( eval( (char*)exp, check ) );
}

inline double XmlExprParser::eval( std::string exp, bool check ) {
  return( eval( exp.c_str(), check ) );
}

#endif // DETDESC_XMLCNVSVC_XMLEXPRPARSER_H

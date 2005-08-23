// $Id: Parser.h,v 1.1.1.1 2005-08-23 10:00:41 marcocle Exp $
#ifndef PARSER_H 
#define PARSER_H 1

// Include files
#include <stdexcept>
#include <string>

/** @class ParserError Parser.h
 *  
 *  Exception class used by @class Parser.
 *
 *  @author Marco Clemencic
 *  @date   2005-06-30
 */
class ParserError: public std::runtime_error {
public:
  ParserError(const std::string &msg): std::runtime_error(msg){}
};

/** @class Parser Parser.h
 *  
 *  Simple class implementing a tokenizer used by @class DimCondCommand.
 *
 *  @author Marco Clemencic
 *  @date   2005-06-30
 */
class Parser 
{
public:
  /// Default constructor
  Parser():
    _white_spaces(" \t\n"),
    _esc_char('\\'),
    _quot_marker('"'),
    _end_marker(';')
  {
    _escapable_chars  = _esc_char;
    _escapable_chars += _quot_marker;
    _escapable_chars += _end_marker;
  }
  
  /// Parse the chars between begin and end, or the first occurence of the separation mark (';'),
  /// and put the found tokens into the tokens container.
  /// Returns the iterator to the position of the first char after the separation mark or end (i.e.
  /// where to start next parsing on the same string)
  template<class container>
  std::string::const_iterator parse(std::string::const_iterator begin,
                                    std::string::const_iterator end,
                                    container &tokens) const;
  /// Same as above, but easier to use for a single statement.
  template<class container>
  inline std::string::const_iterator parse(const std::string &input, container &tokens) const
  {
    return parse(input.begin(),input.end(),tokens);
  }

  /// Method used to put a '\' (back slash) before the special chars, like '"' or ';'.
  /// It is needed to prepare strings to be sent to the command.
  inline std::string escape(const std::string &input) const;

private:
  std::string _white_spaces;
  char _esc_char;
  std::string _escapable_chars;
  char _quot_marker;
  char _end_marker;
    
};

template<class container>
std::string::const_iterator Parser::parse(std::string::const_iterator begin,
                                               std::string::const_iterator end,
                                               container &tokens) const
{
  // clear container
  tokens.clear();
  
  enum Status
    {
      SKIP,   // when dealing with white spaces (or begin)
      TOKEN,  // parsing normal token
      QTOKEN, // parsing quoted token
    };
  
  
  Status current = SKIP;
  std::string last_token;

  // loop over input
  std::string::const_iterator c = begin;
  while ( c != end && *c != _end_marker ) {
    
    switch ( current ){

    case SKIP:
      if ( _white_spaces.find(*c) != std::string::npos ) {
        // is a white space: go on skipping
      } else if ( *c == _quot_marker ) {
        // start quoted token
        current = QTOKEN;
      } else {
        // start normal token
        current = TOKEN;
        last_token = *c;
      }
      break;

    case TOKEN:
      if ( _white_spaces.find(*c) != std::string::npos ) {
        // stop token
        current = SKIP; // ignore
        tokens.push_back(last_token);
        last_token.clear();
      } else if ( *c == _quot_marker ) {
        // like in shells: concatenate with a quoted token
        current = QTOKEN;
      } else {
        // append the char
        last_token += *c;
      }
      break;
      
    case QTOKEN:
      if ( *c == _esc_char ) {
        // escaped char
        std::string::const_iterator next = c+1;
        if ( next != end ){
          if ( _escapable_chars.find(*next) != std::string::npos ) {
            // if the next is a special char put it (increase counter)
            last_token += *++c;
          } else {
            // if not, keep the back-slash
            last_token += *c;
          }
        } else {
          throw ParserError("Unterminated quotation!");
        }
      } else if ( *c == _quot_marker ) {
        // close quoted token (the real closure is done by TOKEN)
        current = TOKEN;
      } else {
        last_token += *c;
      }
    }
    ++c;
  }
  if ( current == TOKEN ) { // the last token closed the string
    tokens.push_back(last_token);
  } else if ( current == QTOKEN ) {
    throw ParserError("Unterminated quotation!");
  }
  // return the position of end of parsing ('end', or _end_mark+1)
  return ( c != end && *c == _end_marker)?++c:c;
}

inline std::string Parser::escape(const std::string &input) const {
  std::string output;
  output.reserve(2*input.size());
  for ( std::string::const_iterator c = input.begin(); c != input.end(); ++c ){
    if ( _escapable_chars.find(*c) != std::string::npos ) {
      output += _esc_char;
    }
    output += *c;
  }
  return output;
}

#endif

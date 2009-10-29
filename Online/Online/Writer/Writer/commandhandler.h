#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H
#include "Writer/writers.h"
#include <list>

// the valid commands
#define START_WRITER "start"
#define STOP_WRITER "stop"
#define OPEN_FILE "openfile"
#define CLOSE_FILE "closefile"
#define DEBUG "debug"
#define LOG "log"

/** Represents a command and its parameters.
 *  name - name of the command, e.g. open
 *  param1 - first parameter
 *  param2 - second parameter
 *  param3 - third parameter
 *  param4 - fourth parameter
 *  more parameters 5-8 (this is not very good way to do it)
 *  writers - the list with the Writers to work with
 */
class Command {
    public:
        std::string name;
        std::string param1;
        std::string param2;
        std::string param3;
        std::string param4;
        std::string param5;
	std::string param6;
	std::string param7;
	std::string param8;
	
	Writers* writers;

        Command(std::string n, std::string p1, std::string p2, std::string p3, std::string p4, std::string p5, std::string p6, std::string p7, std::string p8 ) 
               : name(n), param1(p1), param2(p2), param3(p3), param4(p4), param5(p5), param6(p6), param7(p7), param8(p8)  {
        }
};


/** The abstract class CommandHandler provides the interface
 *  which allows to handle a command. If an error occurs
 *  the method handleCommand will return False, otherwise True.
 *  If there has been an error, the error message can be 
 *  retrieved by getErrorMessage.
 */
class CommandHandler {
    protected:
        std::string* errorMessage; /// string of the error message
        FILE* fp; /// The logfile
    public:
        std::string command; /// the name of the command
        CommandHandler(const char*, FILE*);
        virtual bool handleCommand(Command *)=0;
        virtual struct Command* parse(char*);
        virtual std::string* getErrorMessage() const;
        void setErrorMessage(std::string*);
        virtual ~CommandHandler();

        CommandHandler(const CommandHandler& other) : command(other.command) {
            this->errorMessage = new std::string(*other.errorMessage);
        };

        CommandHandler& operator=(const CommandHandler& other) {
            this->errorMessage = new std::string(*other.errorMessage);
            return *this;
        };
        
        bool operator==(const CommandHandler& other) const {
            return (this->command == (other.command));
        };

        bool operator!=(const CommandHandler& other) const {
            return !(this->command == (other.command));
        };
        
        bool operator<(const CommandHandler& other) const {
            return (this->command.length() < other.command.length());
        };

        bool operator>(const CommandHandler& other) const {
            return (this->command.length() > other.command.length());
        };
};

/** Concrete class to handle the command which registers
 *  a WriterNET with its process id
 */
class StartWriterCommandHandler: public CommandHandler {
    public:
        StartWriterCommandHandler(FILE* fp) : CommandHandler(START_WRITER, fp) {
        };
        virtual bool handleCommand(Command*);
};

/** Concrete class to handle the command to unregister
 *  a WriterNET identified by its process id
 */
class StopWriterCommandHandler: public CommandHandler {
    public:
        StopWriterCommandHandler(FILE* fp) : CommandHandler(STOP_WRITER, fp) {
        };
        virtual bool handleCommand(Command*);
};

// file specific
/** Concrete class to handle the command to open a
 *  file for e specific WriterNet
 */
class OpenFileCommandHandler: public CommandHandler {
    public:
        OpenFileCommandHandler(FILE* fp) : CommandHandler(OPEN_FILE, fp) {
        };
        virtual bool handleCommand(Command*);
};

/** Concrete class to handle the command to close a
 *  file for e specific WriterNet
 */
class CloseFileCommandHandler: public CommandHandler {
    public:
        CloseFileCommandHandler(FILE* fp) : CommandHandler(CLOSE_FILE, fp) {
        };
        virtual bool handleCommand(Command*);
};

/** Concrete class to handle the comand to create or
 *  update a key/value pair for a file. A key/value pair
 *  represents a logging value for a file
 */
class LogFilePropertyCommandHandler: public CommandHandler {
    public:
        LogFilePropertyCommandHandler(FILE* fp) : CommandHandler(LOG, fp) {
        };
        virtual bool handleCommand(Command*);
};

// general
/** Concrete class to handle the command to debug 
 *  and log the status
 */
class DebugCommandHandler: public CommandHandler {
    public:
        DebugCommandHandler(FILE* fp) : CommandHandler(DEBUG, fp) {
        };
        virtual bool handleCommand(Command*);
};

#endif


#ifndef _WIN32

#include "Writer/commandhandler.h"
#include "Writer/writers.h"
#include <stdlib.h>
#include <string.h>
#include "Writer/defs.h"

using namespace std;

/** Constructor, which reserves some money for the error message */
CommandHandler::CommandHandler(char* cmd, FILE* logFilePointer) : command(cmd) {
    this->errorMessage = new string("No error occurred");
    this->fp = logFilePointer;
}

/** Transform a command string to a command struct which can be
 *  processed by the concrete command handlers
 */
Command* CommandHandler::parse(char *buf) {
    string name, p1, p2, p3, p4;
    // tokenize the command string which is like
    // command/param1 or
    // command/param1/param2 or
    // command/param1/param2/name=value
    char* cp = NULL;
    if((cp= strtok(buf, DELIMITER_S)) == NULL) {
        this->setErrorMessage( new string("Error: can not tokenize") );
        return NULL;
    } else {
        name += string(cp);
    }
    if((cp = strtok(NULL, DELIMITER_S)) == NULL) {
        this->setErrorMessage( new string("Error: no parameter found\n") );
        return NULL;
    } else {
        p1 += string(cp);
    }
    if((cp = strtok(NULL, DELIMITER_S)) == NULL) {
        this->setErrorMessage( new string("Info: no second parameter found\n") );
    } else {
        p2 += string(cp);
    }
    if((cp = strtok(NULL, DELIMITER_S)) == NULL) {
        this->setErrorMessage( new string("Info: no third parameter found\n") );
    } else {
        // split the name=value
        char * name = NULL;
        char * value = NULL;
        if((name = strtok(cp, "=")) == NULL) {
            this->setErrorMessage( new string("Error: invalid name=value pair") );
            return NULL;
        } 
        if((value = strtok(NULL, "=")) == NULL) {
            this->setErrorMessage( new string("Error: invalid name=value pair") );
            return NULL;
        }
        p3 += (name);
        p4 += (value);
    }
    // check for valid command
    if(name == string(START_WRITER) ||
       name == string(STOP_WRITER) ||
       name == string(OPEN_FILE) ||
       name == string(CLOSE_FILE) ||
       name == string(DEBUG) ||
       name == string(LOG) ) {
       return new Command(name, p1, p2, p3, p4); // valid command
    } else {
        this->setErrorMessage( new string("Command not known") );
        return NULL;
    }
}

/** Sets the error message */
void CommandHandler::setErrorMessage(string* errorMessage) {
    if(this->errorMessage != NULL) {
        delete this->errorMessage;
    }
    this->errorMessage = errorMessage;
}

/** Get the error Message */
string* CommandHandler::getErrorMessage() const {
    return this->errorMessage;
}

/** Destructor which frees the space reserved
 *  for the error message */
CommandHandler::~CommandHandler() {
    delete this->errorMessage;
}
// writer specific
/** creates a new WriterNetObject */
bool StartWriterCommandHandler::handleCommand(Command* theCommand) {
    int writerPID = 0;
    if((writerPID = atoi(theCommand->param1.c_str())) == 0) {
        this->setErrorMessage(new string("Error: no valid processID\n") );
        return false;
    }
    theCommand->writers->addWriter(writerPID);
    return true;
}

/** Removes a WriterNetObject */
bool StopWriterCommandHandler::handleCommand(Command* theCommand) {
    int writerPID = 0;
    if((writerPID = atoi(theCommand->param1.c_str())) == 0) {
        this->setErrorMessage(new string("Error: no valid processID\n") );
        return false;
    }
    theCommand->writers->deleteWriter(writerPID);
    return true;
}

// file specific
bool OpenFileCommandHandler::handleCommand(Command* theCommand) {
    int writerPID = 0;
    if((writerPID = atoi(theCommand->param1.c_str())) == 0) {
        this->setErrorMessage(new string("Error: no valid processID\n") );
        return false;
    }
    if(theCommand->param2 == "") {
        this->setErrorMessage( new string("Error: no valid file name\n") );
        return false;
    }
    theCommand->writers->addFile( writerPID, theCommand->param2.c_str() );
    return true;
}

bool CloseFileCommandHandler::handleCommand(Command* theCommand) {
    int writerPID = 0;
    if((writerPID = atoi(theCommand->param1.c_str())) == 0) {
        this->setErrorMessage( new string("Error: no valid processID\n") );
        return false;
    }
    if(theCommand->param2 == "") {
        this->setErrorMessage(new string("Error: no valid file name\n") );
        return false;
    }
    theCommand->writers->removeFile(writerPID, theCommand->param2.c_str());
    return true;
}

bool LogFilePropertyCommandHandler::handleCommand(Command* theCommand) {
    int writerPID = 0;
    if((writerPID = atoi(theCommand->param1.c_str())) == 0) {
        this->setErrorMessage( new string("Error: no valid processID\n") );
        return false;
    }
    theCommand->writers->addKeyValuePair(writerPID, 
                                         theCommand->param2.c_str(),
                                         theCommand->param3.c_str(),
                                         theCommand->param4.c_str()
                                        );
    return true;
}

// general
bool DebugCommandHandler::handleCommand(Command* theCommand) {
    theCommand->writers->debug(this->fp);
    return true;
}

#endif

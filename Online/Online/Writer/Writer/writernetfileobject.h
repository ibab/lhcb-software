#ifndef WRITERNETFILEOBJECT_H
#define WRITERNETFILEOBJECT_H
#include <string>
#include <map>

using namespace std;

typedef map<string, string> KeyValuePairContainer;
typedef KeyValuePairContainer::iterator KeyValuePairIterator;

/** \brief Represents a file belonging to a writer process. Manages the
 *  key/value pairs used for monitoring this files. */
class WriterNetFileObject {
    private:
        unsigned int keyValuePairsCount;
    public:
        int writerPID; /** process id of the writer */
        string* fileName; /** name of the file */
        KeyValuePairContainer * keyValuePairs; /** std:map collection of 
                                                *  the key/value paris
                                                * belonging to this file */
                                                
        /** Constructor: gets the processID ot the writr
         *  and the id of the file.
         */
        WriterNetFileObject(int processID, const char* fn);

        /** Copy constructor, used for the container */
        WriterNetFileObject(const WriterNetFileObject& );
        
        /** operator== for comparison in container **/
        bool operator==(const WriterNetFileObject&) const;
        
        /** operator!= for comparison in container **/
        bool operator!=(const WriterNetFileObject&) const;
        
        /** operatori< for comparison in container **/
        bool operator<(const WriterNetFileObject&) const;
        
        /** operator< for comparison in container **/
        bool operator>(const WriterNetFileObject&) const;

        /** Adds a key/value pair for this file. If the key already exists,
         *  the value will we replaced by this value.
         */
        void addKeyValue(const char* key, const char* value);

        /** Debugs. Prints all key value pairs */
        void debug(FILE*);

        /** Destructor */
        ~WriterNetFileObject();
};
#endif


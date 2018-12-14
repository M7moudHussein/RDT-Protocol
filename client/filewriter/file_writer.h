//
// Created by salma on 12/13/18.
//

#ifndef RDT_PROTOCOL_FILE_WRITER_H
#define RDT_PROTOCOL_FILE_WRITER_H


#include <string>
#include <iostream>
#include <fstream>

class file_writer {

public:
    static void open(std::string file_name);

    static void close(std::string file_name);

    static void write(std::string data);  // writes data to same opened file on intervals.

private:
    static std::ofstream file_stream;
};


#endif //RDT_PROTOCOL_FILE_WRITER_H

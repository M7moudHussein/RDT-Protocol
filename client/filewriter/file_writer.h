//
// Created by salma on 12/13/18.
//

#ifndef RDT_PROTOCOL_FILE_WRITER_H
#define RDT_PROTOCOL_FILE_WRITER_H


#include <string>
#include <iostream>
#include <fstream>

namespace file_writer {

    std::ofstream file_stream;

    void open(std::string file_name);

    void close(std::string file_name);

    void write(std::string data);  // writes data to same opened file on intervals.

};


#endif //RDT_PROTOCOL_FILE_WRITER_H

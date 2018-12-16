//
// Created by salma on 12/13/18.
//

#include "file_writer.h"

std::ofstream file_writer::file_stream;

void file_writer::open(std::string file_name) {
    file_writer::file_stream.open(file_name);
}

void file_writer::close() {
    file_writer::file_stream.close();
}

void file_writer::write(std::string data) {
    file_writer::file_stream << data;
}
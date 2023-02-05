#ifndef COMMAND_LINE_INPUTS_H
#define COMMAND_LINE_INPUTS_H

#include <algorithm>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <iterator>

namespace cli {
namespace po = boost::program_options;
po::variables_map read(int ac, char* av[]);
}  // namespace cli
void SetParaviewPath(std::string path);


#endif  // COMMAND_LINE_INPUTS_H
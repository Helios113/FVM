#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <msg.h>
#include <crypto.h>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>

#include <conduit.hpp>
#include "conduit_blueprint.hpp"
#include "conduit_relay.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace interp {
bool Run(po::variables_map& vm, conduit::Node& node);
}
namespace {
bool Interpret(std::string& fp, conduit::Node& node);
bool ValidateDataBase(fs::path db_path);
void FillChildren(conduit::Node& parent, std::vector<std::string>& paths,
                   std::string path);
void ExtractValues(conduit::Node& base, conduit::Node& target,
                    std::vector<std::string>& paths, std::string path);
bool CheckValidity(conduit::Node& input_data,
                   std::vector<std::string>& verification_paths,
                   std::vector<std::string>& required_paths);
}  // namespace

#endif  // INTERPRETER_H
#include <interpreter.h>
#ifdef VAR
#define default_path VAR
#endif

namespace interp {
conduit::Node def_schema;
/** Brief description which ends at this dot. Details follow
 *  here.
 */
bool Run(po::variables_map& vm, conduit::Node& node) {
    // check cli args
    if (!vm.count("input-file") && !vm.count("help") &&
        !vm.count("set-paraview")) {
        msg::Msg("No input files were given", msg::MsgType::Error, true);
        return false;
    }
    if (vm.count("input-file")) {
        // Read each input file path and send to interpreter
        std::string fp = vm["input-file"].as<std::string>();
        msg::Msg(fp, msg::MsgType::Info, true);

        return Interpret(fp, node);
    }
    return false;
}
}  // namespace interp

namespace {
bool Interpret(std::string& fp, conduit::Node& input_data) {
    conduit::Node default_data;

    fs::path bin_path(
        default_path); /*!< Detailed description after the member */

    if (!ValidateDataBase((bin_path / "database"))) {
        return false;
    }

    fs::path def_file_path("database/defaults.yaml");

    conduit::relay::io::load((bin_path / def_file_path).string(),
                             interp::def_schema);

    conduit::relay::io::load(fp, input_data);
    std::vector<std::string> verification_paths = {};
    std::vector<std::string> required_paths = {};

    ExtractValues(interp::def_schema, default_data, required_paths, "");

    FillChildren(input_data, verification_paths, "");

    bool valid = CheckValidity(input_data, verification_paths, required_paths);

    if (valid) {
        std::string ek_path = input_data["structure_file"].as_string();
        conduit::relay::io::load_merged(ek_path, input_data);

        for (int i = 0; i < input_data["ek_files"].number_of_children(); i++) {
            ek_path = input_data["ek_files"][i].as_string();
            conduit::relay::io::load_merged(ek_path, input_data);
        }

        input_data.update(default_data);

        // load material files
        conduit::NodeIterator itr = input_data["mat_table"].children();
        while (itr.has_next()) {
            conduit::Node& cld = itr.next();
            conduit::relay::io::load_merged(
                (bin_path / "database" / (cld.as_string() + ".yaml")).string(),
                input_data);
        }
    }

    return valid;
}
bool ValidateDataBase(fs::path db_path) {
    if (!fs::exists((db_path / "hash.json"))) {
        msg::Msg("Hash file is missing", msg::MsgType::Error, true);
        return false;
    }
    conduit::Node hash_data;
    conduit::relay::io::load((db_path / "hash.json").string(), hash_data);
    for (auto& entry :
         boost::make_iterator_range(fs::directory_iterator(db_path), {})) {
        if (entry.path().filename().string() == "hash.json") continue;
        if (hash_data.has_path(entry.path().filename().string()) == 0) {
            msg::Msg("Unhashed file in database", msg::MsgType::Error, true);
            msg::Msg(entry.path().filename().string(), msg::MsgType::Error,
                     false);
            return false;
        }
        if (!crypto::Safe(
                entry.path().string(),
                hash_data[entry.path().filename().string()].as_string())) {
            msg::Msg("Modified file in database", msg::MsgType::Error, true);
            msg::Msg(entry.path().filename().string(), msg::MsgType::Error,
                     false);
            return false;
        }
    }
    return true;
}
void FillChildren(conduit::Node& parent, std::vector<std::string>& paths,
                  std::string path) {
    conduit::NodeIterator itr = parent.children();
    while (itr.has_next()) {
        conduit::Node& cld = itr.next();
        if (cld.number_of_children() > 0 && !cld.dtype().is_list()) {
            // we are not at a leaf so recurse deeper
            FillChildren(cld, paths,
                         (path.empty()) ? cld.name() : path + "/" + cld.name());
            continue;
        }
        paths.push_back((path.empty()) ? cld.name() : path + "/" + cld.name());
    }
}

void ExtractValues(conduit::Node& base, conduit::Node& target,
                   std::vector<std::string>& paths, std::string path) {
    conduit::NodeIterator itr = base.children();
    while (itr.has_next()) {
        conduit::Node& cld = itr.next();
        if (cld.number_of_children() == 0) {
            // we are not at a leaf so recurse deeper
            if (cld.name() == "value") {
                target[path].set(cld);
            } else if (cld.name() == "required") {
                paths.push_back(path);
            } else if (cld.name() != "range" && cld.name() != "dtype") {
                target[(path.empty()) ? cld.name() : path + "/" + cld.name()]
                    .set(cld);
            }
        } else {
            ExtractValues(
                cld, target, paths,
                (path.empty()) ? cld.name() : path + "/" + cld.name());
        }
    }
}

bool CheckValidity(conduit::Node& input_data,
                   std::vector<std::string>& verification_paths,
                   std::vector<std::string>& required_paths) {
    bool valid = true;
    std::vector<std::string> unkown_keys = {};
    std::vector<std::string> out_of_range_values = {};
    std::vector<std::string> missing_keys = {};
    std::vector<std::string> wrong_dtype = {};

    for (std::string& path : verification_paths) {
        if (interp::def_schema.has_path(path) == 0) {
            unkown_keys.push_back(path);
        } else {
            if (interp::def_schema.has_path(path + "/range") &&
                input_data[path].dtype().is_number()) {
                int n = input_data[path].dtype().number_of_elements();
                double val[n];
                if (n > 1) {
                    if (input_data[path].dtype().is_integer()) {
                        input_data[path].to_double_array(input_data[path]);
                    }
                    auto tmp = input_data[path].as_double_array();
                    for (int i = 0; i < n; i++) {
                        val[i] = tmp[i];
                    }
                } else if (n == 1) {
                    val[0] = (input_data[path].dtype().is_integer())
                                 ? double(input_data[path].as_int64())
                                 : input_data[path].as_double();
                }
                for (int i = 0; i < n; i++) {
                    if (interp::def_schema[path + "/range"]
                            .dtype()
                            .is_integer()) {
                        interp::def_schema[path + "/range"].to_double_array(
                            interp::def_schema[path + "/range"]);
                    }
                    auto rng =
                        interp::def_schema[path + "/range"].as_double_array();
                    if (val[i] < rng[0] || val[i] > rng[1]) {
                        out_of_range_values.push_back(path);
                    }
                }
            }
            if (interp::def_schema.has_path(path + "/dtype")) {
                if (interp::def_schema[path + "/dtype"].as_string() ==
                        "number" &&
                    !input_data[path].dtype().is_number()) {
                    wrong_dtype.push_back(path);
                }
                if (interp::def_schema[path + "/dtype"].as_string() ==
                        "string" &&
                    !input_data[path].dtype().is_string()) {
                    wrong_dtype.push_back(path);
                }
            }
        }
    }
    for (std::string& path : required_paths) {
        if (input_data.has_path(path) == 0) {
            missing_keys.push_back(path);
        }
    }

    if (unkown_keys.size() > 0) {
        valid = false;
        msg::Msg("Keys that don't match:", msg::MsgType::Error, true);
        msg::Msg(unkown_keys, msg::MsgType::Error, false);
    }
    if (out_of_range_values.size() > 0) {
        valid = false;
        msg::Msg("Values which are out of range:", msg::MsgType::Error, true);

        std::string info = "";
        BOOST_FOREACH (std::string a, out_of_range_values) {
            info += a;
            info += " ";
            info += input_data[a].to_string() + " " +
                    interp::def_schema[a + "/range"].to_string() + "\n";
        }
        info.pop_back();
        msg::Msg(info, msg::MsgType::Error, false);
    }
    if (missing_keys.size() > 0) {
        valid = false;
        msg::Msg("Missing required parameters:", msg::MsgType::Error, true);
        msg::Msg(missing_keys, msg::MsgType::Error, false);
    }
    if (wrong_dtype.size() > 0) {
        valid = false;
        msg::Msg("Wrong type of value given:", msg::MsgType::Error, true);

        std::string info = "";

        BOOST_FOREACH (std::string a, wrong_dtype) {
            info += a;
            info += " must be ";
            info += interp::def_schema[a + "/dtype"].to_string() + "\n";
        }
        info.pop_back();
        msg::Msg(info, msg::MsgType::Error, false);
    }
    return valid;
}
}  // namespace

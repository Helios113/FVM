#include <command_line_input.h>
#include <msg.h>

namespace {
template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    copy(v.begin(), v.end(), std::ostream_iterator<T>(os, " "));
    return os;
}
}  // namespace
namespace cli {
namespace po = boost::program_options;
po::variables_map read(int ac, char* av[]) {
    po::variables_map vm;
    try {
        int opt;
        po::options_description desc("Allowed options");
        desc.add_options()("help,h", "produce help message")(
            "math-debug,d", "enable math-debug")(
            "input-file,i", po::value<std::string>(), "input file")(
            "output-file,o", po::value<std::string>(), "output file")(
            "visualise,v", po::value<std::string>(), "paraview pipeline")(
            "set-paraview,s", po::value<std::string>(),
            "set the paraview path");

        po::positional_options_description p;
        p.add("input-file", -1);

        po::store(
            po::command_line_parser(ac, av).options(desc).positional(p).run(),
            vm);
        po::notify(vm);

        if (vm.count("help")) {
            msg::Msg("Usage: options_description [options]", msg::MsgType::Info,
                     true);
            std::stringstream a;
            a << desc;
            msg::Msg(a.str(), msg::MsgType::Info, false);

        } else {
            if (vm.count("set-paraview")) {
                msg::Msg("Paraview path set to:", msg::MsgType::Info, true);
                msg::Msg(vm["set-paraview"].as<std::string>(), msg::MsgType::Info, false);
                SetParaviewPath(vm["set-paraview"].as<std::string>());
            }
            if (vm.count("input-file")) {
                msg::Msg("Input path is: " + vm["input-file"].as<std::string>(),
                         msg::MsgType::Info, true);
            }

            if (vm.count("output-file")) {
                msg::Msg(
                    "Output path is: " + vm["output-file"].as<std::string>(),
                    msg::MsgType::Info, true);
            }
            if (vm.count("verbose")) {
                msg::Msg("Verbosity enabled", msg::MsgType::Info, true);
            }
            if (vm.count("math-debug")) {
                msg::Msg("Math debug enabled", msg::MsgType::Info, true);
            }
        }
        return vm;
    } catch (std::exception& e) {
        msg::Msg(e.what(), msg::MsgType::Error, true);
        return vm;
    }
}
}  // namespace cli

void SetParaviewPath(std::string path) {
    try {
        std::ofstream pwp;
        pwp.open("pwp");
        pwp << path;
        pwp.close();

    } catch (std::exception& e) {
        msg::Msg(e.what(), msg::MsgType::Error, true);
    }
}
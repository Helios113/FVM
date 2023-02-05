#include <command_line_input.h>
#include <grid.h>
#include <interpreter.h>
#include <msg.h>
#include <poisson.h>
#include <type_defs.h>
#include <vis.h>

#include <fstream>

#include "conduit.hpp"
#include "conduit_blueprint.hpp"
namespace po = boost::program_options;
int main(int argc, char* argv[]) {
    conduit::Node node;
    po::variables_map vm = cli::read(argc, argv);

    if (!interp::Run(vm, node)) {
        return 0;
    }
    msg::Msg("Loaded files successfully", msg::MsgType::Happy, true);

    grid::Grid a(node);

    if (vm.count("visualise")) {
        vis::Initialise(vm["visualise"].as<std::string>());
        vis::Execute(a);
    }
    // poisson::PoissonCoeffs(a);
    if (vm.count("visualise")) {
        vis::Finalise();
    }
    return 0;
}

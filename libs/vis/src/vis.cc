#include <vis.h>

void vis::Initialise(std::string file_path) {
    conduit_cpp::Node node;
    std::string pw_path = "";
    try {
        std::ifstream pwp;
        pwp.open("pwp");
        std::getline(pwp, pw_path);

    } catch (std::exception& e) {
        msg::Msg(e.what(), msg::MsgType::Error, true);
    }

    node["catalyst/scripts/script"] = "/home/preslav/NESS_NEW/ness/savedata.py";
    node["catalyst_load/implementation"] = "paraview";
    node["catalyst_load/search_paths/paraview"] =
        "/home/preslav/Documents/ParaView-5.11.0-RC2-MPI-Linux-Python3.9-x86_64/lib/catalyst";

    catalyst_status err = catalyst_initialize(conduit_cpp::c_node(&node));
    if (err != catalyst_status_ok) {
        msg::Msg("Failed to initialize Catalyst: ", msg::MsgType::Error, true);
        msg::Msg(err, msg::MsgType::Error, false);
    }
}

void vis::Execute(grid::Grid& grid) {
    conduit_cpp::Node exec_params;

    // add time/cycle information
    auto state = exec_params["catalyst/state"];
    state["timestep"].set(1);
    state["time"].set(1.0);

    // // Add channels.
    // // We only have 1 channel here. Let's name it 'grid'.
    auto channel = exec_params["catalyst/channels/grid"];

    // // Since this example is using Conduit Mesh Blueprint to define the mesh,
    // // we set the channel's type to "mesh".
    channel["type"].set("mesh");

    // // now create the mesh.
    auto mesh = channel["data"];

    // // start with coordsets (of course, the sequence is not important, just
    // make
    // // it easier to think in this order).

    mesh["coordsets/coords/type"].set_string("rectilinear");
    mesh["coordsets/coords/values/x"].set(grid.g_x);
    mesh["coordsets/coords/values/y"].set(grid.g_y);
    mesh["coordsets/coords/values/z"].set(grid.g_z);

    mesh["topologies/grid/coordset"].set("coords");
    mesh["topologies/grid/type"].set("rectilinear");

    mesh["topologies/grid/elements/origin/i"].set(grid.g_x[0]);
    mesh["topologies/grid/elements/origin/j"].set(grid.g_y[0]);
    mesh["topologies/grid/elements/origin/k"].set(grid.g_z[0]);
    mesh["topologies/grid/elements/origin"].print();

    auto fields = mesh["fields"];

    BOOST_FOREACH (auto& a, grid.vertex_scalar_fields) {
        AddScalerField(fields, a.first, "vertex", a.second);
    }
    BOOST_FOREACH (auto& a, grid.cell_scalar_fields) {
        AddScalerField(fields, a.first, "element", a.second);
    }
    BOOST_FOREACH (auto& a, grid.vertex_vector_fields) {
        AddVectorField(fields, a.first, "vertex", a.second);
    }
    BOOST_FOREACH (auto& a, grid.cell_vector_fields) {
        AddVectorField(fields, a.first, "element", a.second);
    }
    BOOST_FOREACH (auto& a, grid.vertex_tensor_fields) {
        AddTensorField(fields, a.first, "vertex", a.second,
                       a.second.shape()[3]);
    }
    BOOST_FOREACH (auto& a, grid.cell_tensor_fields) {
        AddTensorField(fields, a.first, "element", a.second,
                       a.second.shape()[3]);
    }
    catalyst_status err = catalyst_execute(conduit_cpp::c_node(&exec_params));
    if (err != catalyst_status_ok) {
        std::cerr << "Failed to execute Catalyst: " << err << std::endl;
    }
}

void vis::Finalise() {
    conduit_cpp::Node node;
    catalyst_status err = catalyst_finalize(conduit_cpp::c_node(&node));
    if (err != catalyst_status_ok) {
        std::cerr << "Failed to finalize Catalyst: " << err << std::endl;
    }
}

template <typename T>
static void AddScalerField(conduit_cpp::Node& node, const std::string& name,
                           const char* association, const T& field) {
    node[name + "/association"].set(association);
    node[name + "/topology"].set("grid");
    node[name + "/volume_dependent"].set("false");
    node[name + "/values"].set(field.data(), field.num_elements());
}

template <typename T>
static void AddVectorField(conduit_cpp::Node& node, const std::string& name,
                           const char* association, const T& field) {
    node[name + "/association"].set(association);
    node[name + "/topology"].set("grid");
    node[name + "/volume_dependent"].set("false");
    node[name + "/values/x"].set(field.data(), field.num_elements() / 3,
                                 /*offset=*/0, 3*sizeof(double));
    node[name + "/values/y"].set(field.data(), field.num_elements() / 3,
                                 /*offset=*/1*sizeof(double), 3*sizeof(double));
    node[name + "/values/z"].set(field.data(), field.num_elements() / 3,
                                 /*offset=*/2*sizeof(double), 3*sizeof(double));
}

template <typename T>
static void AddTensorField(conduit_cpp::Node& node, const std::string& name,
                           const char* association, const T& field,
                           int tensor_dim) {
    node[name + "/association"].set(association);
    node[name + "/topology"].set("grid");
    node[name + "/volume_dependent"].set("false");
    for (int i = 0; i < tensor_dim; i++) {
        node[name + "/values/" + std::to_string(i)].set(
            field.data(), field.num_elements() / tensor_dim,
            /*offset=*/i * sizeof(double),
            /*stride=*/tensor_dim * sizeof(double));
    }
}
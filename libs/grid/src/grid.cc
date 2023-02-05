#include <grid.h>

grid::Grid::Grid(conduit::Node& node) {
    SetDims(node["Nx"].as_int(), node["Ny"].as_int(), node["Nz"].as_int());
    SetGrid(node["coordsets/coords/values"]);
    SetFields(node["fields"]);
    SetMaterials(node);
    GeneratePoissonCoeffs();
}
void grid::Grid::SetDims(int x, int y, int z) {
    dims[0] = x;
    dims[1] = y;
    dims[2] = z;
}
int grid::Grid::GetDim(int i) { return dims[i]; }

void grid::Grid::SetGrid(conduit::Node& node) {
    auto tmp = node["x"].as_double_array();
    for (int i = 0; i < dims[0]; i++) {
        g_x.push_back(tmp[i]);
    }
    tmp = node["y"].as_double_array();
    for (int i = 0; i < dims[1]; i++) {
        g_y.push_back(tmp[i]);
    }
    tmp = node["z"].as_double_array();
    for (int i = 0; i < dims[2]; i++) {
        g_z.push_back(tmp[i]);
    }

    SetPoints();
    GenerateCellProps();
}

void grid::Grid::SetPoints() {
    // Maybe memorry leak?
    // Actually definetly this is a mem leak. However it is only used once, so
    // should be fine
    p_x = new scalar_field{boost::extents[dims[0]][dims[1]][dims[2]]};
    p_y = new scalar_field{boost::extents[dims[0]][dims[1]][dims[2]]};
    p_z = new scalar_field{boost::extents[dims[0]][dims[1]][dims[2]]};
    for (int i = 0; i < dims[0]; i++) {
        for (int j = 0; j < dims[1]; j++) {
            for (int k = 0; k < dims[2]; k++) {
                (*p_x)[i][j][k] = g_x[i];
                (*p_y)[i][j][k] = g_y[j];
                (*p_z)[i][j][k] = g_z[k];
            }
        }
    }
}

void grid::Grid::SetFields(conduit::Node& node) {
    conduit::NodeIterator itr = node.children();
    while (itr.has_next()) {
        conduit::Node& cld = itr.next();
        std::string name = cld.name();
        auto tmp = cld["values"].as_double_array();
        if (cld["association"].as_string() == "vertex") {
            vertex_scalar_fields.insert(
                {name,
                 scalar_field{boost::extents[dims[0]][dims[1]][dims[2]]}});
            for (int i = 0; i < dims[0]; i++) {
                for (int j = 0; j < dims[1]; j++) {
                    for (int k = 0; k < dims[2]; k++) {
                        vertex_scalar_fields[name][i][j][k] =
                            tmp[i * dims[1] * dims[2] + j * dims[2] + k];
                    }
                }
            }
        }
        if (cld["association"].as_string() == "element") {
            cell_scalar_fields.insert(
                {name,
                 scalar_field{
                     boost::extents[dims[0] - 1][dims[1] - 1][dims[2] - 1]}});
            for (int i = 0; i < dims[0] - 1; i++) {
                for (int j = 0; j < dims[1] - 1; j++) {
                    for (int k = 0; k < dims[2] - 1; k++) {
                        cell_scalar_fields[name][i][j][k] =
                            tmp[i * (dims[1] - 1) * (dims[2] - 1) +
                                j * (dims[2] - 1) + k];
                    }
                }
            }
        }
    }
}

void grid::Grid::GenerateCellProps() {
    cell_vector_fields.insert(
        {"cell_dims",
         vector_field{
             boost::extents[dims[0] - 1][dims[1] - 1][dims[2] - 1][3]}});
    cell_vector_fields.insert(
        {"area",
         vector_field{
             boost::extents[dims[0] - 1][dims[1] - 1][dims[2] - 1][3]}});

    cell_scalar_fields.insert(
        {"vol",
         scalar_field{boost::extents[dims[0] - 1][dims[1] - 1][dims[2] - 1]}});

    cell_scalar_fields.insert(
        {"centre_sum",
         scalar_field{boost::extents[dims[0] - 1][dims[1] - 1][dims[2] - 1]}});

    auto& areas = cell_vector_fields["area"];
    auto& cell_dims = cell_vector_fields["cell_dims"];
    auto& vol = cell_scalar_fields["vol"];
    auto& centre_sum = cell_scalar_fields["centre_sum"];

    // optimisation for abs
    bool brb = -1;  // branch reducing bool

    for (int i = 0; i < dims[0] - 1; i++) {
        for (int j = 0; j < dims[1] - 1; j++) {
            for (int k = 0; k < dims[2] - 1; k++) {
                // x dist
                cell_dims[i][j][k][0] =
                    ((*p_x)[i + 1][j][k] - (*p_x)[i][j][k]) * 1e-7;
                brb = cell_dims[i][j][k][0] > 0;  // check if we are pos (1)
                cell_dims[i][j][k][0] *= brb;
                // y dist
                cell_dims[i][j][k][1] =
                    ((*p_y)[i][j + 1][k] - (*p_y)[i][j][k]) * 1e-7;
                brb = cell_dims[i][j][k][1] > 0;  // check if we are pos (1)
                cell_dims[i][j][k][1] *= brb;
                // z dist
                cell_dims[i][j][k][2] =
                    ((*p_z)[i][j][k + 1] - (*p_z)[i][j][k]) * 1e-7;
                brb = cell_dims[i][j][k][2] > 0;  // check if we are pos (1)
                cell_dims[i][j][k][2] *= brb;

                vol[i][j][k] = cell_dims[i][j][k][0] * cell_dims[i][j][k][1] *
                               cell_dims[i][j][k][2] * 0.125;
                // x face area
                areas[i][j][k][0] =
                    cell_dims[i][j][k][1] * cell_dims[i][j][k][2] * 0.25;
                // y face area
                areas[i][j][k][1] =
                    cell_dims[i][j][k][0] * cell_dims[i][j][k][2] * 0.25;
                // z face area
                areas[i][j][k][2] =
                    cell_dims[i][j][k][0] * cell_dims[i][j][k][1] * 0.25;

                centre_sum[i][j][k] =
                    areas[i][j][k][0] / cell_dims[i][j][k][0] +
                    areas[i][j][k][1] / cell_dims[i][j][k][1] +
                    areas[i][j][k][2] / cell_dims[i][j][k][2];
            }
        }
    }
}

void grid::Grid::GeneratePoissonCoeffs() {
    vertex_tensor_fields.insert(
        {"poisson_coeffs",
         tensor_field{boost::extents[dims[0]][dims[1]][dims[2]][8]}});
    auto& poisson_coeffs = vertex_tensor_fields["poisson_coeffs"];
    auto& areas = cell_vector_fields["area"];
    auto& cell_dims = cell_vector_fields["cell_dims"];
    auto& centre_sum = cell_scalar_fields["centre_sum"];
    auto& vol = cell_scalar_fields["vol"];
    auto& mat = cell_scalar_fields["mat"];
    short a, b, c;
    for (int i = 0; i < dims[0] - 1; i++) {
        for (int j = 0; j < dims[1] - 1; j++) {
            for (int k = 0; k < dims[2] - 1; k++) {
                for (int cnt = 0; cnt < 8; cnt++) {
                    a = (1 & (cnt >> (2)));
                    b = (1 & (cnt >> (1)));
                    c = (1 & (cnt >> (0)));
                    poisson_coeffs[i + a][j + b][k + c][a] +=
                        mat_field[mat[i][j][k]]["permittivity"] *
                        areas[i][j][k][0] / cell_dims[i][j][k][0];
                    poisson_coeffs[i + a][j + b][k + c][2 + b] +=
                        mat_field[mat[i][j][k]]["permittivity"] *
                        areas[i][j][k][1] / cell_dims[i][j][k][1];
                    poisson_coeffs[i + a][j + b][k + c][4 + c] +=
                        mat_field[mat[i][j][k]]["permittivity"] *
                        areas[i][j][k][2] / cell_dims[i][j][k][2];
                    poisson_coeffs[i + a][j + b][k + c][6] -=
                        mat_field[mat[i][j][k]]["permittivity"] *
                        centre_sum[i][j][k];
                    poisson_coeffs[i + a][j + b][k + c][7] += vol[i][j][k];
                }
            }
        }
    }
}

// Generate a list of alternating indecies to do red/black
// Try expansion
//   array_type::array_view<3>::type myview =
//   myarray[ boost::indices[range(0,2)][range(1,3)][range(0,4,2)] ]; striding
//   easy peasy

void grid::Grid::SetMaterials(conduit::Node& node) {
    auto& mat_table = node["mat_table"];
    auto& mats = node["mat"];
    mat_field.resize(boost::extents[mat_table.number_of_children()]);

    conduit::NodeIterator itr = mat_table.children();
    while (itr.has_next()) {
        conduit::Node& cld = itr.next();
        std::unordered_map<std::string, dynamic_double> m;
        conduit::NodeIterator itr2 = mats[cld.as_string()].children();
        while (itr2.has_next()) {
            conduit::Node& cld2 = itr2.next();
            if (cld2.dtype().number_of_elements() != 1) {
                std::vector<double> a;
                cld2.to_double_array(cld2);
                auto b = cld2.as_double_array();
                for (int i = 0; i < cld.dtype().number_of_elements(); i++) {
                    a.push_back(b[i]);
                }
                m.insert({cld2.name(), dynamic_double(a)});
            } else {
                m.insert({cld2.name(), dynamic_double(cld2.to_double())});
            }
        }
        mat_field[stoi(cld.name())] = m;
    }
}
#ifndef GRID_H
#define GRID_H
#include <stdio.h>
#include <msg.h>
#include <type_defs.h>

#include <boost/foreach.hpp>

#include "conduit.hpp"
namespace grid {

class Grid {
   private:
    // Point locations
    scalar_field* p_x;
    scalar_field* p_y;
    scalar_field* p_z;

    // Geometry
    void SetPoints();
    void GenerateCellProps();
    void GeneratePoissonCoeffs();

   public:
    Grid(conduit::Node& node);

    // Scalar fields
    std::unordered_map<std::string, scalar_field> vertex_scalar_fields;
    std::unordered_map<std::string, scalar_field> cell_scalar_fields;

    // Vector fields
    std::unordered_map<std::string, vector_field> vertex_vector_fields;
    std::unordered_map<std::string, vector_field> cell_vector_fields;

    // Vector fields
    std::unordered_map<std::string, tensor_field> vertex_tensor_fields;
    std::unordered_map<std::string, tensor_field> cell_tensor_fields;

    complex_field mat_field{boost::extents[0]};

    // Grid locations
    std::vector<double> g_x;
    std::vector<double> g_y;
    std::vector<double> g_z;
    int dims[3];
    void SetDims(int x, int y, int z);
    int GetDim(int i);

    void SetGrid(conduit::Node& node);
    void SetFields(conduit::Node& node);
    void SetMaterials(conduit::Node& node);

    double* GetXPointsStart() { return p_x->data(); }
    int GetXPointsNumber() { return p_x->num_elements(); }
    double* GetYPointsArray() { return p_y->data(); }
    double* GetZPointsArray() { return p_z->data(); }
};

}  // namespace grid

#endif  // MSG_H

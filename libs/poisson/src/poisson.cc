#include <poisson.h>

void PoissonCoeffs(grid::Grid& grid) {
    grid.cell_vector_fields.insert(
        {"coeffs",
         vector_field{boost::extents[grid.dims[0] - 1][grid.dims[1] - 1]
                                    [grid.dims[2] - 1][7]}});
    // What is the relations between cells and vertices?

    // Best way to store data

    for (int i = 0; i < grid.dims[0]; i++) {
        for (int j = 0; j < grid.dims[1]; j++) {
            for (int k = 0; k < grid.dims[2]; k++) {
                // grid.cell_vector_fields["coeffs"][i][j][k][0] += ;
            }
        }
    }
}
#ifndef VIS_H
#define VIS_H
#include <grid.h>
#include <msg.h>

#include <catalyst.hpp>
#include <conduit_cpp_to_c.hpp>

#include <boost/foreach.hpp>
#include <fstream>
#include <string> 
namespace vis {
void Initialise(std::string file_path);
void Execute(grid::Grid& grid);
void Finalise();
}  // namespace vis

template <typename T>
static void AddScalerField(conduit_cpp::Node& node, const std::string& name,
               const char* association, const T& field);

template <typename T>
static void AddVectorField(conduit_cpp::Node& node, const std::string& name,
                       const char* association, const T& field);
template <typename T>
static void AddTensorField(conduit_cpp::Node& node, const std::string& name,
                      const char* association, const T& field, int tensor_dim);
#endif
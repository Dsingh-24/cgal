#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/read_xyz_points.h>
#include <CGAL/Point_with_normal_3.h>
#include <CGAL/property_map.h>

#include <CGAL/Shape_detection_3.h>

#include <iostream>
#include <fstream>

// Type declarations
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef CGAL::Point_with_normal_3<Kernel>                   Point_with_normal;
typedef std::vector<Point_with_normal>                      Pwn_vector;
typedef CGAL::Identity_property_map<Point_with_normal>      Point_map;
typedef CGAL::Normal_of_point_with_normal_pmap<Kernel>      Normal_map;

// In Efficient_RANSAC_traits the basic types, i.e., Point and Vector types
// as well as iterator type and property maps, are defined.
typedef CGAL::Shape_detection_3::Efficient_RANSAC_traits
  <Kernel, Pwn_vector, Point_map, Normal_map>   Traits;
typedef CGAL::Shape_detection_3::Efficient_RANSAC<Traits>   Efficient_ransac;
typedef CGAL::Shape_detection_3::Plane<Traits>              Plane;

int main() 
{
  // Points with normals.
  Pwn_vector points;

  // Loads point set from a file. 
  // read_xyz_points_and_normals takes an OutputIterator for storing the points
  // and a property map to store the normal vector with each point.
  std::ifstream stream("cube.pwn");

  if (!stream || 
    !CGAL::read_xyz_points_and_normals(stream,
      std::back_inserter(points),
      Normal_map()))
  {
      std::cerr << "Error: cannot read file cube.pwn" << std::endl;
      return EXIT_FAILURE;
  }

  // Instantiates shape detection engine.
  Efficient_ransac ransac;

  // Provides the input data.
  ransac.set_input_data(points);

  // Registers planar shapes via template method.
  ransac.add_shape_factory<Plane>();

  // Detects registered shapes with default parameters.
  ransac.detect();

  // Prints number of detected shapes.
  std::cout << ransac.shapes().size() << " shapes detected." << std::endl;

  return EXIT_SUCCESS;
}

/** @file dijkstra_theta.cpp
 *
 * An example application that calculates the shortest paths on a constructed Theta graph 
 * by calling the Dijkstra's algorithm from BGL.
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <algorithm>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Theta_graph_2.h>

using namespace boost;

// select the kernel type
typedef CGAL::Exact_predicates_inexact_constructions_kernel   Kernel;
typedef Kernel::Point_2                   Point_2;
typedef Kernel::Direction_2               Direction_2;

/** define the struct for edge property */
struct Edge_property {
	/** record the Euclidean length of the edge */
	double euclidean_length;
};
// define the theta graph to use the selected kernel, to be undirected,
// and to use Edge_property as the edge property
typedef CGAL::Theta_graph_2<Kernel, boost::undirectedS, Edge_property>      T;
// obtain the graph type by boost::adjacency_list 
typedef T::Graph                                              Graph;

int main(int argc, char ** argv) {

	if (argc != 3) {
		std::cout << "Usage: " << argv[0] << " <no. of cones> <input filename>" << std::endl;
        return 1;
    }

	unsigned long k = atol(argv[1]);
	if (k<2) {
		std::cout << "The number of cones should be larger than 1!" << std::endl;
		return 1;
	}

	// open the file containing the vertex list
	std::ifstream inf(argv[2]);
	if (!inf) {
		std::cout << "Cannot open file " << argv[1] << "!" << std::endl;
		return 1;
	}

	// iterators for reading the vertex list file
	std::istream_iterator< Point_2 > input_begin( inf );
	std::istream_iterator< Point_2 > input_end;

	// construct the theta graph on the vertex list
	T t(k, input_begin, input_end, Direction_2(1,0));

	// copy the boost::adjacency_list object of the constructed graph from t
	// copy is used here because we need to modify the edge property when calling the dijkstra's algorithm.
	Graph g = t.graph();

	// select a source vertex for dijkstra's algorithm
	graph_traits<Graph>::vertex_descriptor v0;
	v0 = vertex(0, g);
	std::cout << "The source vertex is: " << g[v0] << std::endl;

	// get the vertex index map
	property_map<Graph, vertex_index_t>::type indexmap = get(vertex_index, g);
	std::cout << "The index of source vertex is: " << get(indexmap, v0) << std::endl;

	// calculating edge length in Euclidean metric and store them in the edge property
	graph_traits<Graph>::edge_iterator ei, ei_end;  
	for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
		graph_traits<Graph>::edge_descriptor e = *ei;
		graph_traits<Graph>::vertex_descriptor  u = source(e, g);
		graph_traits<Graph>::vertex_descriptor  v = target(e, g);
		const Point_2& pu = g[u];
		const Point_2& pv = g[v];

		double dist = CGAL::sqrt( CGAL::to_double((pu.x()-pv.x())*(pu.x()-pv.x()) + (pu.y()-pv.y())*(pu.y()-pv.y())) );
		g[e].euclidean_length = dist;
		std::cout << "Edge (" << g[u] << ", " << g[v] << "): " << dist << std::endl;
	}

    // calculating the distances from v0 to other vertices
	unsigned int n = num_vertices(g);
	// vector for storing the results
	std::vector<double> distances(n);
	// Calling the Dijkstra's algorithm implementation from boost.
	// For details, pls read the documentation from BGL
    dijkstra_shortest_paths(g,
                            v0,
                            weight_map(get(&Edge_property::euclidean_length, g)).
                            distance_map(make_iterator_property_map(distances.begin(), indexmap))
						    );

	std::cout << "distances are:" << std::endl;
	for (unsigned int i=0; i < n; ++i) {
	    std::cout << "distances[" << i << "] = " << distances[i] << ", (x,y)=" << g[vertex(i, g)];
		std::cout << " at Vertex " << i << std::endl;
	}

    return 0;
}

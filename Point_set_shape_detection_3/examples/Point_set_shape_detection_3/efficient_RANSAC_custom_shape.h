#ifndef MY_PLANE_SHAPE_H
#define MY_PLANE_SHAPE_H

#include <CGAL/Efficient_RANSAC.h>

/*
My_Plane derives from Shape_base. The plane is represented by
its normal vector and distance to the origin.
*/
template <class Traits>
class My_Plane : public CGAL::Shape_detection_3::Shape_base<Traits> {
public:
  typedef typename Traits::Geom_traits::FT FT;///< number type.
  typedef typename Traits::Geom_traits::Point_3 Point;///< point type.

public:
  My_Plane() : Shape_base<Traits>() {}

  //  Computes squared Euclidean distance from query point to the shape.
  virtual FT squared_distance(const Point &p) const {
    const FT sd = (p - m_point_on_primitive) * m_normal;
    return sd * sd;
  }

protected:
  // Constructs shape based on minimal set of samples from the input data.    
  virtual void create_shape(const std::vector<std::size_t> &indices) {
    const Point p1 = this->point(indices[0]);
    const Point p2 = this->point(indices[1]);
    const Point p3 = this->point(indices[2]);

    m_normal = CGAL::cross_product(p1 - p2, p1 - p3);

    m_normal = m_normal * (1.0 / sqrt(m_normal.squared_length()));
    m_d = -(p1[0] * m_normal[0] + p1[1] * m_normal[1] + p1[2] * m_normal[2]);

    m_is_valid = true;
  }

  // Computes squared Euclidean distance from a set of points.
  virtual void squared_distance(const std::vector<std::size_t> &indices,
                                std::vector<FT> &dists) {
      for (std::size_t i = 0; i < indices.size(); i++) {
        const FT sd = (this->point(indices[i])
          - m_point_on_primitive) * m_normal;
        dists[i] = sd * sd;
      }
  }

  /*
  Computes the normal deviation between shape and
  a set of points with normals.
  */
  virtual void cos_to_normal(const std::vector<std::size_t> &indices,
                             std::vector<FT> &angles) const {
      for (std::size_t i = 0; i < indices.size(); i++)
        angles[i] = abs(this->normal(indices[i]) * m_normal);
  }

  // Returns the number of required samples for construction.
  virtual std::size_t minimum_sample_size() const {
    return 3;
  }

  // Returns a string with shape parameters.
  virtual std::string info() const {
    std::stringstream sstr;
    sstr << "Type: plane (" << m_normal.x() << ", " 
      << m_normal.y() << ", " << m_normal.z() << ")x - " <<
      m_d << " = 0" << " #Pts: " << this->m_indices.size();

    return sstr.str();
  }

private:
  Point m_point_on_primitive;
  Vector m_normal;
  FT m_d;
};
#endif

#ifndef MPM_PARTICLEBASE_H_
#define MPM_PARTICLEBASE_H_

#include <array>
#include <limits>
#include <memory>
#include <vector>

#include "cell.h"
#include "data_types.h"
#include "function_base.h"
#include "hdf5_particle.h"
#include "material.h"

namespace mpm {

// Forward declaration of Material
template <unsigned Tdim>
class Material;

//! Particle phases
enum ParticlePhase : unsigned int { Solid = 0, Liquid = 1, Gas = 2 };

//! ParticleBase class
//! \brief Base class that stores the information about particleBases
//! \details ParticleBase class: id_ and coordinates.
//! \tparam Tdim Dimension
template <unsigned Tdim>
class ParticleBase {
 public:
  //! Define a vector of size dimension
  using VectorDim = Eigen::Matrix<double, Tdim, 1>;

  //! Constructor with id and coordinates
  //! \param[in] id Particle id
  //! \param[in] coord coordinates of the particle
  ParticleBase(Index id, const VectorDim& coord);

  //! Constructor with id, coordinates and status
  //! \param[in] id Particle id
  //! \param[in] coord coordinates of the particle
  //! \param[in] status Particle status (active / inactive)
  ParticleBase(Index id, const VectorDim& coord, bool status);

  //! Destructor
  virtual ~ParticleBase(){};

  //! Delete copy constructor
  ParticleBase(const ParticleBase<Tdim>&) = delete;

  //! Delete assignement operator
  ParticleBase& operator=(const ParticleBase<Tdim>&) = delete;

  //! Initialise particle HDF5 data
  //! \param[in] particle HDF5 data of particle
  //! \retval status Status of reading HDF5 particle
  virtual bool initialise_particle(const HDF5Particle& particle) = 0;

  //! Initialise particle HDF5 data and material
  //! \param[in] particle HDF5 data of particle
  //! \param[in] material Material associated with the particle
  //! \retval status Status of reading HDF5 particle
  virtual bool initialise_particle(
      const HDF5Particle& particle,
      const std::shared_ptr<Material<Tdim>>& material) = 0;

  //! Assign material history variables
  //! \param[in] state_vars State variables
  //! \param[in] material Material associated with the particle
  //! \retval status Status of cloning HDF5 particle
  virtual bool assign_material_state_vars(
      const mpm::dense_map& state_vars,
      const std::shared_ptr<mpm::Material<Tdim>>& material) = 0;

  //! Retrun particle data as HDF5
  //! \retval particle HDF5 data of the particle
  virtual HDF5Particle hdf5() const = 0;

  //! Return id of the particleBase
  Index id() const { return id_; }

  //! Assign coordinates
  //! \param[in] coord Assign coord as coordinates of the particleBase
  void assign_coordinates(const VectorDim& coord) { coordinates_ = coord; }

  //! Return coordinates
  //! \retval coordinates_ return coordinates of the particleBase
  VectorDim coordinates() const { return coordinates_; }

  //! Compute reference coordinates in a cell
  virtual bool compute_reference_location() = 0;

  //! Return reference location
  virtual VectorDim reference_location() const = 0;

  //! Assign cell
  virtual bool assign_cell(const std::shared_ptr<Cell<Tdim>>& cellptr) = 0;

  //! Assign cell and xi
  virtual bool assign_cell_xi(const std::shared_ptr<Cell<Tdim>>& cellptr,
                              const Eigen::Matrix<double, Tdim, 1>& xi) = 0;

  //! Assign cell id
  virtual bool assign_cell_id(Index id) = 0;

  //! Return cell id
  virtual Index cell_id() const = 0;

  //! Return cell ptr
  virtual std::shared_ptr<Cell<Tdim>> cell() const = 0;

  //! Return cell ptr status
  virtual bool cell_ptr() const = 0;

  //! Remove cell
  virtual void remove_cell() = 0;

  //! Compute shape functions
  virtual void compute_shapefn() noexcept = 0;

  //! Assign volume
  virtual bool assign_volume(double volume) = 0;

  //! Return volume
  virtual double volume() const = 0;

  //! Return size of particle in natural coordinates
  virtual VectorDim natural_size() const = 0;

  //! Assign boolean property at the particle
  //! \param[in] property Name of the property to assign
  //! \param[in] boolean Property boolean (true/false) of the particles in a
  //! cell
  void assign_boolean_property(mpm::properties::Boolean property,
                               bool boolean) noexcept;

  //! Return boolean property
  //! \param[in] property Name of the property to update
  //! \retval boolean property at particle
  bool boolean_property(mpm::properties::Boolean property) const;

  //! Update scalar property at the particle
  //! \param[in] property Name of the property to update
  //! \param[in] update A boolean to update (true) or assign (false)
  //! \param[in] value Property value from the particles in a cell
  void update_scalar_property(mpm::properties::Scalar property, bool update,
                              double value) noexcept;

  //! Return scalar property
  //! \param[in] property Name of the property to return
  //! \retval scalar property at particle
  double scalar_property(mpm::properties::Scalar property) const;

  //! Map scalar property to the nodes
  //! \param[in] property Name of the property to update
  //! \param[in] update A boolean to update (true) or assign (false)
  //! \param[in] phase Index corresponding to the phase
  void map_scalar_property_to_nodes(mpm::properties::Scalar property,
                                    bool update, unsigned phase) noexcept;

  //! Map an arbitrary scalar value to nodal scalar property
  //! \param[in] property Name of the property to update
  //! \param[in] update A boolean to update (true) or assign (false)
  //! \param[in] phase Index corresponding to the phase
  //! \param[in] value Scalar value to be mapped from particle to node
  void map_scalar_property_to_nodes(mpm::properties::Scalar property,
                                    bool update, unsigned phase,
                                    double value) noexcept;

  //! Return an interpolation of scalar property in particle from nodes
  //! \param[in] property Name of the property to update
  //! \param[in] phase Index corresponding to the phase
  //! \retval interpolated scalar property at particle
  double interpolate_scalar_property_from_nodes(
      mpm::properties::Scalar property, unsigned phase) const;

  //! Update vector property at the particle
  //! \param[in] property Name of the property to update
  //! \param[in] update A boolean to update (true) or assign (false)
  //! \param[in] value Property value from the particles in a cell
  void update_vector_property(
      mpm::properties::Vector property, bool update,
      const Eigen::Matrix<double, Tdim, 1>& value) noexcept;

  //! Return vector property
  //! \param[in] property Name of the property to return
  //! \retval vector property at particle
  Eigen::Matrix<double, Tdim, 1> vector_property(
      mpm::properties::Vector property) const;

  //! Map vector property to the nodes
  //! \param[in] property Name of the property to update
  //! \param[in] update A boolean to update (true) or assign (false)
  //! \param[in] phase Index corresponding to the phase
  void map_vector_property_to_nodes(mpm::properties::Vector property,
                                    bool update, unsigned phase) noexcept;

  //! Map an arbitrary vector value to nodal vector property
  //! \param[in] property Name of the property to update
  //! \param[in] update A boolean to update (true) or assign (false)
  //! \param[in] phase Index corresponding to the phase
  //! \param[in] value Vector value to be mapped from particle to node
  void map_vector_property_to_nodes(
      mpm::properties::Vector property, bool update, unsigned phase,
      const Eigen::Matrix<double, Tdim, 1>& value) noexcept;

  //! Return an interpolation of vector property in particle from nodes
  //! \param[in] property Name of the property to update
  //! \param[in] phase Index corresponding to the phase
  //! \retval interpolated vector property at particle
  Eigen::Matrix<double, Tdim, 1> interpolate_vector_property_from_nodes(
      mpm::properties::Vector property, unsigned phase) const;

  //! Return mass density
  virtual double mass_density() const = 0;

  //! Map multimaterial properties to nodes
  virtual void map_multimaterial_mass_momentum_to_nodes() noexcept = 0;

  //! Map multimaterial displacements to nodes
  virtual void map_multimaterial_displacements_to_nodes() noexcept = 0;

  //! Map multimaterial domain gradients to nodes
  virtual void map_multimaterial_domain_gradients_to_nodes() noexcept = 0;

  //! Assign material
  virtual bool assign_material(
      const std::shared_ptr<Material<Tdim>>& material) = 0;

  //! Return material
  virtual std::shared_ptr<Material<Tdim>> material() const = 0;

  //! Return material id
  unsigned material_id() const { return material_id_; }

  //! Assign status
  void assign_status(bool status) { status_ = status; }

  //! Status
  bool status() const { return status_; }

  //! Initialise properties
  virtual void initialise() = 0;

  //! Assign mass
  virtual void assign_mass(double mass) = 0;

  //! Return mass
  virtual double mass() const = 0;

  //! Assign pressure
  virtual void assign_pressure(double pressure) = 0;

  //! Return pressure
  virtual double pressure() const = 0;

  //! Compute strain
  virtual void compute_strain(double dt) noexcept = 0;

  //! Strain
  virtual Eigen::Matrix<double, 6, 1> strain() const = 0;

  //! Strain rate
  virtual Eigen::Matrix<double, 6, 1> strain_rate() const = 0;

  //! Volumetric strain of centroid
  virtual double volumetric_strain_centroid() const = 0;

  //! dvolumetric strain
  virtual double dvolumetric_strain() const = 0;

  //! Initial stress
  virtual void initial_stress(const Eigen::Matrix<double, 6, 1>&) = 0;

  //! Compute stress
  virtual void compute_stress() noexcept = 0;

  //! Return stress
  virtual Eigen::Matrix<double, 6, 1> stress() const = 0;

  //! Map internal force
  virtual void map_internal_force() noexcept = 0;

  //! Assign velocity
  virtual void assign_velocity(const VectorDim& velocity) = 0;

  //! Return velocity
  virtual VectorDim velocity() const = 0;

  //! Return displacement of the particle
  virtual VectorDim displacement() const = 0;

  //! Assign traction
  virtual bool assign_traction(unsigned direction, double traction) = 0;

  //! Return traction
  virtual VectorDim traction() const = 0;

  //! Compute updated position
  virtual void compute_updated_position(
      double dt, bool velocity_update = false) noexcept = 0;

  //! Return a state variable
  virtual void assign_state_variable(const std::string& var, double value) = 0;

  //! Return a state variable
  virtual double state_variable(const std::string& var) const = 0;

  //! Return tensor data of particles
  //! \param[in] property Property string
  //! \retval vecdata Tensor data of particle property
  virtual Eigen::VectorXd tensor_data(const std::string& property) = 0;

  //! Apply particle velocity constraints
  //! \param[in] dir Direction of particle velocity constraint
  //! \param[in] velocity Applied particle velocity constraint
  virtual void apply_particle_velocity_constraints(unsigned dir,
                                                   double velocity) = 0;

  //! Assign material id of this particle to nodes
  virtual void append_material_id_to_nodes() const = 0;

  //! Return the number of neighbour particles
  virtual unsigned nneighbours() const = 0;

  //! Assign neighbour particles
  //! \param[in] neighbours set of id of the neighbouring particles
  //! \retval insertion_status Return the successful addition of a node
  virtual void assign_neighbours(const std::vector<mpm::Index>& neighbours) = 0;

  //! Return neighbour ids
  virtual std::vector<mpm::Index> neighbours() const = 0;

 protected:
  //! particleBase id
  Index id_{std::numeric_limits<Index>::max()};
  //! coordinates
  VectorDim coordinates_;
  //! Cell id
  Index cell_id_{std::numeric_limits<Index>::max()};
  //! Status
  bool status_{true};
  //! Reference coordinates (in a cell)
  Eigen::Matrix<double, Tdim, 1> xi_;
  //! Cell
  std::shared_ptr<Cell<Tdim>> cell_;
  //! Vector of nodal pointers
  std::vector<std::shared_ptr<NodeBase<Tdim>>> nodes_;
  //! Material
  std::shared_ptr<Material<Tdim>> material_;
  //! Unsigned material id
  unsigned material_id_{std::numeric_limits<unsigned>::max()};
  //! Material state history variables
  mpm::dense_map state_variables_;
  //! Vector of particle neighbour ids
  std::vector<mpm::Index> neighbours_;
  //! Shape functions
  Eigen::VectorXd shapefn_;
  //! Boolean properties
  fc::vector_map<mpm::properties::Boolean, bool> boolean_properties_;
  //! Scalar properties
  fc::vector_map<mpm::properties::Scalar, double> scalar_properties_;
  //! Vector properties
  fc::vector_map<mpm::properties::Vector, Eigen::Matrix<double, Tdim, 1>>
      vector_properties_;
};  // ParticleBase class
}  // namespace mpm

#include "particle_base.tcc"

#endif  // MPM_PARTICLEBASE_H__

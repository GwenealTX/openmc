#ifndef OPENMC_WMP_H
#define OPENMC_WMP_H

#include "hdf5.h"
#include "xtensor/xtensor.hpp"

#include <array>
#include <complex>
#include <string>
#include <tuple>

namespace openmc {

//========================================================================
// Constants
//========================================================================

// Constants that determine which value to access
constexpr int MP_EA {0}; // Pole
constexpr int MP_RS {1}; // Residue scattering
constexpr int MP_RA {2}; // Residue absorption
constexpr int MP_RF {3}; // Residue fission

// Polynomial fit indices
constexpr int FIT_S {0}; // Scattering
constexpr int FIT_A {1}; // Absorption
constexpr int FIT_F {2}; // Fission

// Multipole HDF5 file version
constexpr std::array<int, 2> WMP_VERSION {1, 1};

//========================================================================
// Windowed multipole data
//========================================================================

class WindowedMultipole {
public:
  // Types
  struct WindowInfo {
    int index_start; // Index of starting pole
    int index_end; // Index of ending pole
    bool broaden_poly; // Whether to broaden polynomial curvefit
  };

  // Constructors, destructors
  WindowedMultipole(hid_t group);

  // Methods

  //! \brief Evaluate the windowed multipole equations for cross sections in the
  //! resolved resonance regions
  //!
  //! \param E Incident neutron energy in [eV]
  //! \param sqrtkT Square root of temperature times Boltzmann constant
  //! \return Tuple of elastic scattering, absorption, and fission cross sections in [b]
  std::tuple<double, double, double> evaluate(double E, double sqrtkT);

  //! \brief Evaluates the windowed multipole equations for the derivative of
  //! cross sections in the resolved resonance regions with respect to
  //! temperature.
  //!
  //! \param E Incident neutron energy in [eV]
  //! \param sqrtkT Square root of temperature times Boltzmann constant
  //! \return Tuple of derivatives of elastic scattering, absorption, and
  //!         fission cross sections in [b/K]
  std::tuple<double, double, double> evaluate_deriv(double E, double sqrtkT);

  // Data members
  std::string name_; //!< Name of nuclide
  double E_min_; //!< Minimum energy in [eV]
  double E_max_; //!< Maximum energy in [eV]
  double sqrt_awr_; //!< Square root of atomic weight ratio
  double inv_spacing_; //!< 1 / spacing in sqrt(E) space
  int fit_order_; //!< Order of the fit
  bool fissionable_; //!< Is the nuclide fissionable?
  std::vector<WindowInfo> window_info_; // Information about a window
  xt::xtensor<double, 3> curvefit_; // Curve fit coefficients (window, poly order, reaction)
  xt::xtensor<std::complex<double>, 2> data_; //!< Poles and residues
};

//========================================================================
// Non-member functions
//========================================================================

//! Check to make sure WMP library data version matches
//!
//! \param[in] file  HDF5 file object
void check_wmp_version(hid_t file);

//! \brief Checks for the existence of a multipole library in the directory and
//! loads it
//!
//! \param[in] i_nuclide  Index in global nuclides array
void read_multipole_data(int i_nuclide);

} // namespace openmc

#endif // OPENMC_WMP_H

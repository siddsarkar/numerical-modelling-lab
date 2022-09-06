#include <iostream>
#include <numeric>
#include <valarray>
#include <vector>

#define ND -1

using namespace std;

void plot_variogram(vector<int> &h, vector<float> &semi_variance,
                    string label) {
  // number of fits, number of points per fit
  size_t const n_fits = 10;
  size_t const n_points_per_fit = 10;

  // model ID and number of model parameters
  int const model_id = GAUSS_1D;
  size_t const n_model_parameters = 4;

  // initial parameters
  vector<REAL> initial_parameters(n_fits * n_model_parameters);

  // data
  vector<REAL> data(n_points_per_fit * n_fits);

  // tolerance
  REAL const tolerance = 0.001f;

  // maximum number of iterations
  int const max_number_iterations = 10;

  // estimator ID
  int const estimator_id = LSE;

  // parameters to fit (all of them)
  vector<int> parameters_to_fit = {
      accumulate(h.begin(), h.end(), 0.0) / h.size(),
      accumulate(semi_variance.begin(), semi_variance.end(), 0.0) /
          semi_variance.size(),
      0,
  };

  // output parameters
  vector<REAL> output_parameters(n_fits * n_model_parameters);
  vector<int> output_states(n_fits);
  vector<REAL> output_chi_square(n_fits);
  vector<int> output_number_iterations(n_fits);

  /***************************** call to gpufit  ****************************/

  int const status =
      gpufit(n_fits, n_points_per_fit, data.data(), 0, model_id,
             initial_parameters.data(), tolerance, max_number_iterations,
             parameters_to_fit.data(), estimator_id, 0, 0,
             output_parameters.data(), output_states.data(),
             output_chi_square.data(), output_number_iterations.data());

  /****************************** status check  *****************************/

  if (status != ReturnState::OK) {
    throw runtime_error(gpufit_get_last_error());
  }
}

vector<double> horizontal(vector<vector<double>> data, int lag) {
  int n = data.size();
  int m = data[0].size();
  double s = 0;
  double p = 0;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m - lag; j++) {
      if (data[i][j] != ND && data[i][j + lag] != ND) {
        s += pow(data[i][j] - data[i][j + lag], 2);
        p += 1;
      }
    }
  }
  return {s / p};
}

vector<double> vertical(vector<vector<double>> data, int lag) {
  int n = data.size();
  int m = data[0].size();
  double s = 0;
  double p = 0;
  for (int i = 0; i < n - lag; i++) {
    for (int j = 0; j < m; j++) {
      if (data[i][j] != ND && data[i + lag][j] != ND) {
        s += pow(data[i][j] - data[i + lag][j], 2);
        p += 1;
      }
    }
  }
  return {s / p};
}

vector<double> inclined(vector<vector<double>> data, int lag) {
  int n = data.size();
  int m = data[0].size();
  double s = 0;
  double p = 0;
  for (int i = 0; i < n - lag; i++) {
    for (int j = 0; j < m - lag; j++) {
      if (data[i][j] != ND && data[i + lag][j + lag] != ND) {
        s += pow(data[i][j] - data[i + lag][j + lag], 2);
        p += 1;
      }
    }
  }
  return {s / p};
}

// def semivariogram_plotting(semivariance):
//"""
// Plot the semivariogram.
//"""
// p0 = [np.mean(h), np.mean(semivariance), 0]
// cof , cov = curve_fit(models.gaussian, h, semivariance, p0)
// print("range: %.2f   sill: %.f   nugget: %.2f" % (cof[0], cof[1], cof[2]))
// xi =np.linspace(h[0], h[-1])
// yi = [models.gaussian(h, *cof) for h in xi]
// plt.plot(h, semivariance, 'og')
// plt.plot(xi, yi, '-b')
// plt.show()
//  convert above to c++ code using gpufit and pbPlots

int main() {
  vector<vector<double>> data = {{44, ND, 40, 42, 40, 39, 37, 36, ND},
                                 {42, ND, 43, 42, 39, 39, 41, 40, 36},
                                 {37, 37, 37, 35, 38, 37, 37, 33, 34},
                                 {35, 38, ND, 35, 37, 36, 36, 35, ND},
                                 {36, 35, 36, 35, 39, 33, 32, 29, 28},
                                 {38, 37, 35, ND, 30, ND, 29, 30, 32}};

  vector<int> h = {100, 200, 300, 400, 500, 600};

  vector<double> semi_variance_horizontal;
  for (int i = 1; i < 6; i++) {
    semi_variance_horizontal.push_back(horizontal(data, i)[0]);
  }

  vector<double> semi_variance_vertical;
  for (int i = 1; i < 6; i++) {
    semi_variance_vertical.push_back(vertical(data, i)[0]);
  }

  vector<double> semi_variance_inclined;
  for (int i = 1; i < 6; i++) {
    semi_variance_inclined.push_back(inclined(data, i)[0]);
  }

  plot_variogram(h, semi_variance_horizontal, "Horizontal");

  return 0;
}

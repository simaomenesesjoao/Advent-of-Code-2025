#include <iostream>
#include <Eigen/Dense>
#include <optional>

constexpr double tol = 1e-7;
constexpr unsigned int DEBUG = 3;

bool is_equal(double x, double y);
bool is_equal(const Eigen::VectorXd& x, const Eigen::VectorXd& y);

struct Constraint {
    std::optional<double> smallest;
    std::vector<double> row;
    std::optional<double> largest;
};

using Constraints = std::vector<Constraint>;

struct Solution {
    Eigen::VectorXd coordinates;
    double cost;
    bool exists;
};

std::optional<Eigen::MatrixXd> solve_lp(const Eigen::MatrixXd& A, unsigned int num_constraints, unsigned int cost_row);

Solution optimize(const Constraints& constraints, const Eigen::VectorXd& cost);
Eigen::MatrixXd add_slack(const Constraints& constraints, const Eigen::VectorXd& cost);
Eigen::MatrixXd add_artificial(const Eigen::MatrixXd& A);
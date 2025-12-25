#include "utils/lp.h"

bool is_equal(double x, double y){
    return std::abs(x-y) < tol;
}

bool is_equal(const Eigen::VectorXd& x, const Eigen::VectorXd& y){
    return is_equal((x-y).norm(), 0);
}

Eigen::MatrixXd add_slack(const Constraints& constraints, const Eigen::VectorXd& cost){

    if(DEBUG > 1){
        std::cout << "LP: Getting number of variables\n";
    }

    unsigned int num_vars = cost.size();
    unsigned int num_constraints = 0;
    for(const auto& [lowest, row, highest]: constraints){
        if(highest){
            num_constraints++;
        }
        if(lowest){
            num_constraints++;
        }
    }

    if(DEBUG > 1){
        std::cout << "LP: " << num_vars << " " << num_constraints << "\n";
    }
    
    Eigen::MatrixXd A(num_constraints+1, num_constraints + num_vars + 1);

    if(DEBUG > 1){
        std::cout << "LP: Setting matrix\n";
    }

    unsigned int constraint = 0;
    unsigned int row_index = 0;
    for(unsigned int i=0; i<constraints.size(); i++){

        if(DEBUG > 1){
            std::cout << "Row index: " << i << "\n";
        }

        const auto& [lowest, row, highest] = constraints.at(i);

        if(highest){

            if(DEBUG > 1){
                std::cout << "Highest\n";
            }
            
            for(unsigned col_index=0; col_index<num_vars; col_index++){
                A(row_index, col_index) = row.at(col_index);
            }

            // Slack variable
            A(row_index, num_vars + row_index) = 1.0;

            // RHS
            A(row_index, num_vars + num_constraints) = *highest;

            row_index++;
        }

        if(lowest){

            if(DEBUG > 1){
                std::cout << "lowest\n";
            }
            
            for(unsigned col_index=0; col_index<num_vars; col_index++){
                A(row_index, col_index) = -row.at(col_index);
            }

            // Slack variable
            A(row_index, num_vars + row_index) = 1.0;


            if(DEBUG > 1){
                std::cout << "LP: RHS\n";
            }
            
            // RHS
            A(row_index, num_vars + num_constraints) = -*lowest;

            row_index++;
        }
    }


    for(unsigned int i=0; i<num_vars; i++){
        A(num_constraints, i) = cost(i);
    }

    if(DEBUG > 2){
        std::cout << "Matrix with slack variables:\n" << A << "\n";
    }
    
    return A;
}

std::optional<Eigen::MatrixXd> solve_lp(const Eigen::MatrixXd& A, unsigned int num_constraints, unsigned int cost_row){

    if(DEBUG > 1){
        std::cout << "Entering solve_lp for matrix\n" << A << "\n";
    }

    auto B = A;
    unsigned int rows = B.rows();
    unsigned int cols = B.cols();
    unsigned int num_variables = cols - 1;

    unsigned int i = 40;
    while(--i > 0){

        if(DEBUG > 1){
            std::cout << "LP: New pivot iteration\n";
        }        

        // Find largest non-negative element in the cost function
        double highest = -1.0;
        int max_col = -1;
        for(int col=0; col<num_variables; col++){
            double b = B(cost_row, col);
            if(b > 0 && b > highest){
                highest = b;
                max_col = col;
            }
        }

        if(DEBUG > 1){
            std::cout << "LP: Cost row: " << B.row(cost_row) << " | largest: " << highest << " | col index: " << max_col << " \n";
        }

        // Algorithm complete
        if(max_col == -1){

            if(DEBUG > 1){
                std::cout << "LP: Cannot improve cost more\n";
            }

            break;
        }

        // Find the first restriction violated from increasing this variable
        int max_row = -1;
        double lowest_ratio = 0.0;
        for(int row=0; row<num_constraints; row++){
            double b = B(row, max_col);
            double c = B(row, num_variables);

            if(b < tol){
                continue;
            }

            if(c/b < lowest_ratio || lowest_ratio == 0.0){
                lowest_ratio = c/b;
                max_row = row;
            }
        }
        
        if(max_row == -1){
            if(DEBUG > 1){
                std::cout << "LP: Pivot row could not be found.\n";
            }
            return std::nullopt;

        }

        if(DEBUG > 1){
            std::cout << "LP: Pivot row: " << B.row(max_row) << "\n";
        }

        // Update the matrix
        B.row(max_row) /= B(max_row, max_col);
        for(unsigned int j=0; j<rows; j++){
            if(j == max_row){
                continue;
            }
            B.row(j) -= B(j, max_col)*B.row(max_row);
        }


        if(DEBUG > 1){
            std::cout << "LP: New matrix:\n";
            std::cout << B << "\n";
        }


    }
    
    // No solution found
    if(i <= 0){
        if(DEBUG > 1){
            std::cout << "Timeout reached.\n";
        }
        return std::nullopt;
    }

    return B;
}

Eigen::MatrixXd remove_artificial(const Eigen::MatrixXd& A, const unsigned int num_vars, const unsigned int num_constraints){

    Eigen::MatrixXd B = Eigen::MatrixXd::Zero(num_constraints+1, num_vars + num_constraints+1);

    // Constraints + objective row
    B.block(0, 0, num_constraints+1, num_vars + num_constraints) = A.block(0, 0, num_constraints+1, num_vars + num_constraints);

    B.block(0, num_vars + num_constraints, num_constraints+1,1) = A.block(0,num_vars + num_constraints*2,num_constraints+1,1);

    return B;

}

Eigen::MatrixXd add_artificial(const Eigen::MatrixXd& A){
    // Given A*x = b and x > 0 such that x contains slack variables, add artificial
    // variables and minimize their sum

    const unsigned int cols = A.cols()-1;
    const unsigned int rows = A.rows()-1;

    const Eigen::VectorXd cost = A.row(rows);    

    Eigen::MatrixXd B = Eigen::MatrixXd::Zero(rows+2, rows+cols+1);

    B.block(0,0,rows,cols) = A.block(0,0,rows, cols); // constraints
    B.block(0,cols+rows,rows,1) = A.block(0,cols,rows,1); // b

    for(unsigned int row=0; row<rows; row++){
        if(B(row, rows+cols) < 0) {
            B.row(row) *= -1;
        }
    }
    
    B.block(rows, 0, 1, cols) = A.block(rows, 0, 1, cols);
    B(rows+1, cols+rows) = A(rows,cols);

    for(unsigned int row=0; row<rows; row++){
        B.row(rows+1) += B.row(row);
        B(row,row+cols) += 1.0; // artificial variables
    }
    
    if(DEBUG > 2){
        std::cout << "Matrix with artificial variables:\n" << B << "\n";
    }

    return B;
}

Eigen::VectorXd get_sol_coordinates(const Eigen::MatrixXd& A){
    const unsigned int num_constraints = A.rows() - 1;
    const unsigned int num_vars = A.cols() - 1;

    Eigen::VectorXd coordinates(num_vars);
    
    for(unsigned int col=0; col<num_vars; col++){

        unsigned int num_non_zero = 0;
        unsigned int one_row = 0;
        for(unsigned int row=0; row<num_constraints; row++){
            double a = A(row, col);
            if(is_equal(a, 1.0)){
                one_row = row;
            }
            if(!is_equal(a, 0.0)){
                num_non_zero++;
            }
        }
        if(num_non_zero == 1){
            coordinates(col) = A(one_row, num_vars);
        }
    }
    return coordinates;
}

Solution optimize(const Constraints& constraints, const Eigen::VectorXd& cost){

    // The constraints are inequalities. 
    // They need to be converted to equalities through slack variables A*x = b
    const Eigen::MatrixXd A = add_slack(constraints, cost);

    // At this point, it is not yet known whether the LP is feasible, so we
    // add artificial variables and minimize (Phase 1 LP)
    const Eigen::MatrixXd B = add_artificial(A);

    const unsigned int num_constraints = B.rows() - 2;
    const unsigned int num_vars = cost.size();
    const unsigned int cost_row = B.rows() - 1;
    const auto maybe_C = solve_lp(B, num_constraints, cost_row);
    
    if(!maybe_C){
        return {{}, {}, false};
    }
    
    const auto& C = *maybe_C;

    if(std::abs(C(C.rows()-1, C.cols()-1)) > tol){
        return {{}, {}, false};
    }

    const auto CC = remove_artificial(C, num_vars, num_constraints);

    const auto maybe_D = solve_lp(CC, num_constraints, cost_row-1);
    if(!maybe_D){
        return {{}, {}, false};
    }
    const auto& D = *maybe_D;

    const auto coordinates = get_sol_coordinates(D);
    Eigen::VectorXd orig_coordinates(num_vars);
    orig_coordinates = coordinates.segment(0,num_vars);

    if(DEBUG > 1){
        std::cout << "coordinates: " << coordinates.transpose() << "\n";
    }    

    return {orig_coordinates, -D(D.rows()-1, D.cols()-1), true};    
}


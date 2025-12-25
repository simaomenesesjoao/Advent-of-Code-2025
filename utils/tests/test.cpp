#include <iostream>
#include "utils/lp.h"
#include <Eigen/Dense>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Maximize in a square", "[simplex]") {
    SECTION("Given a square region"){
        /* Region:
         ___  
        |   |  1 <= x <= 4
        |   |  2 <= y <= 5.5
        |___|  
        */

        unsigned int cols = 2;

        Constraints constraints;
        constraints.push_back({1.0, {1.0, 0.0}, 4.0});
        constraints.push_back({2.0, {0.0, 1.0}, 5.5});

        SECTION("when the cost increases as x+y"){

            Eigen::VectorXd cost(cols);
            cost << 1.0, 1.0;
            
            Solution sol = optimize(constraints, cost);

            Eigen::VectorXd solution(cols);
            solution << 4.0, 5.5;

            REQUIRE(sol.exists);
            REQUIRE(is_equal(sol.cost, 9.5));
            REQUIRE(is_equal(sol.coordinates, solution));
        }

        SECTION("when the cost increases as -x+y"){

            Eigen::VectorXd cost(cols);
            cost << -1.0, 1.0;
            
            Solution sol = optimize(constraints, cost);

            Eigen::VectorXd solution(cols);
            solution << 1.0, 5.5;

            REQUIRE(sol.exists);
            REQUIRE(is_equal(sol.cost, 4.5));
            REQUIRE(is_equal(sol.coordinates, solution));
        }

        SECTION("when the cost only depends on y"){

            Eigen::VectorXd cost(cols);
            cost << 0.0, 1.0;
            
            Solution sol = optimize(constraints, cost);

            REQUIRE(sol.exists);
            REQUIRE(is_equal(sol.cost, 5.5));
        }
    }
}


TEST_CASE("Impossible square", "[simplex]") {
    SECTION("Given an impossible region"){
        // Region: 5 <= x+y <= 4

        unsigned int cols = 2;

        Constraints constraints;
        constraints.push_back({5.0, {1.0, 0.0}, 4.0});
        

        SECTION("when the cost increases as x+y"){

            Eigen::VectorXd cost(cols);
            cost << 1.0, 1.0;
            
            Solution sol = optimize(constraints, cost);

            REQUIRE(!sol.exists);
        }
    }
}

TEST_CASE("Unbounded LP", "[simplex]") {
    SECTION("Given an unbounded region"){

        unsigned int cols = 2;

        Constraints constraints;
        constraints.push_back({1.0, {0.0, 1.0}, std::nullopt});
        constraints.push_back({1.0, {1.0, 0.0}, std::nullopt});

        SECTION("when the cost vector points in the unbounded direction"){
            Eigen::VectorXd cost(cols);
            cost << 1.0, 1.0;

            Solution sol = optimize(constraints, cost);

            REQUIRE(!sol.exists);
        }


        SECTION("when the cost vector points in the bounded direction"){
            Eigen::VectorXd cost(cols);
            cost << -1.0, -1.0;

            Solution sol = optimize(constraints, cost);

            Eigen::VectorXd solution(cols);
            solution << 1.0, 1.0;

            REQUIRE(sol.exists);
            REQUIRE(is_equal(sol.cost, -2.0));
            REQUIRE(is_equal(sol.coordinates, solution));
        }
       
    }
}

TEST_CASE("Minimization in a trapezoid", "[simplex]") {

    /* Region:
       |\     3 <= x+y <= 5
       | \    x >= 1
       |  \   y >= 1
       \___\
    Objective: minimize 2x + y 
    */

    unsigned int cols = 2;

    Constraints constraints;
    constraints.push_back({3.0, {1.0, 1.0}, 5});
    constraints.push_back({1.0, {1.0, 0.0}, std::nullopt});
    constraints.push_back({1.0, {0.0, 1.0}, std::nullopt});

    Eigen::VectorXd cost(cols);
    cost << -2.0, -1.0;
    
    Solution sol = optimize(constraints, cost);

    Eigen::VectorXd solution(cols);
    solution << 1.0, 2.0;
    
    REQUIRE(sol.exists);
    REQUIRE(is_equal(sol.cost, -4.0));
    REQUIRE(is_equal(sol.coordinates, solution));
}

TEST_CASE("Minimization in a polygon", "[simplex]") {
    /* Region:
        1 <= x+y
        2x-y <= 1
        3y <= 2

        Objective: maximize 6x + 3y
    */

    SECTION("Given an unbounded region"){
        unsigned int cols = 2;

        Constraints constraints;
        constraints.push_back({1.0, {1.0, 1.0}, std::nullopt});
        constraints.push_back({std::nullopt, {2.0, -1.0}, 1.0});
        constraints.push_back({std::nullopt, {0.0, 3.0}, 2.0});


        SECTION("when the cost function is 6x + 3y"){

            Eigen::VectorXd cost(cols);
            cost << 6.0, 3.0;
            Solution sol = optimize(constraints, cost);

            Eigen::VectorXd solution(cols);
            solution << 5.0/6.0, 2.0/3.0;

            REQUIRE(sol.exists);
            REQUIRE(is_equal(sol.cost, 7));
            REQUIRE(is_equal(sol.coordinates, solution));
        }
    }
}

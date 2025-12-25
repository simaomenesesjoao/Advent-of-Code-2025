#include <optional>
#include <string>
#include <vector>
#include <iostream>
#include "utils/utils.h"
#include <Eigen/Dense>

using Button = std::vector<int>;

struct Machine {
    std::vector<bool> indicator_lights;
    std::vector<Button> buttons;
    std::vector<int> joltages;

    friend std::ostream& operator<<(std::ostream& stream, const Machine& machine) {
        stream << "Machine:\n";
        stream << "- Indicator lights: " << machine.indicator_lights << "\n";
        stream << "- Buttons: ";
        for(const auto& button: machine.buttons){
            stream << button << " ";
        }
        stream << "\n- Joltages: " << machine.joltages;
        return stream;
    }
};

struct ParseOutput {
    
    std::vector<Machine> machines;

    friend std::ostream& operator<<(std::ostream& stream, const ParseOutput& output) {
        for(const auto& machine: output.machines){
            stream << machine << "\n";
        }
        return stream;
    }
};

ParseOutput parse(const std::vector<std::string>& lines){

    std::vector<Machine> machines;

    for(const auto& line: lines){
        std::vector<std::string> items = utils::split(line, ' ');

        Machine machine;

        // Get the indicator lights
        std::string lights_str = items.at(0);
        for(char c: lights_str){
            if(c == '.'){
                machine.indicator_lights.push_back(false);
            } else if(c == '#'){
                machine.indicator_lights.push_back(true);
            }
        }
        
        // Get the buttons
        for(const auto& item: items){
            if(item[0] == '{' || item[0] == '['){
                continue;
            }

            unsigned int N = item.size();
            std::vector<std::string> numbers = utils::split(item.substr(1, N-2), ',');
            
            Button button;
            for(const auto& number: numbers){
                button.push_back(std::stoi(number));
            }
            machine.buttons.push_back(button);
        }


        // Get the joltagess
        for(const auto& item: items){
            if(item[0] != '{'){
                continue;
            }


            unsigned int N = item.size();
            std::vector<std::string> numbers = utils::split(item.substr(1, N-2), ',');

            for(const auto& number: numbers){
                machine.joltages.push_back(std::stoi(number));
            }
        }


        machines.push_back(machine);
    }
    return {machines};
}

int g(const std::vector<bool>& state, const std::vector<Button>& buttons, const std::vector<bool>& lights_expected){

    unsigned int num_presses = 0;

    std::vector<bool> lights(lights_expected.size(), false);
    
    for(unsigned int i=0; i<state.size(); i++){
        const auto& active = state.at(i);

        if(active){
            num_presses++;
            for(const int digit: buttons.at(i)){
                lights[digit] = !lights[digit];
            }
        }
    }
    
    // std::cout << "Final state: " << state << " -> " << lights << " with " << num_presses << " presses ";

    for(unsigned int i=0; i<lights_expected.size(); i++){
        if(lights.at(i) != lights_expected.at(i)){
            // std::cout << " X\n";
            return 999999;
        }
    }

    return num_presses;
}

int f(const std::vector<bool>& state, 
      const std::vector<Button>& buttons, 
      int n, 
      const std::vector<bool>& lights_expected){

    if(n == -1){
        return g(state, buttons, lights_expected);
    }

    std::vector<bool> state1 = state;
    std::vector<bool> state2 = state;

    state1[n] = true;
    state2[n] = false;

    return std::min(f(state1, buttons, n-1, lights_expected), f(state2, buttons, n-1, lights_expected));
}

L first_part(const std::vector<Machine>& machines) {

    L count = 0;
    for(const auto& machine: machines){
        unsigned int N = machine.buttons.size();

        std::vector<bool> state(N, false);
        count += f(state, machine.buttons, N-1, machine.indicator_lights);

    }
    
    return count;
}
using V = Eigen::VectorXd;

V remove_projection(const V& a, const V& b){
    return a - a.dot(b)*b/b.squaredNorm();
}

std::optional<std::pair<int, double>> next_intersection(const V& x, const V& v, const std::vector<V>& matrix, const std::vector<int>& joltages){

    std::cout << "Computing next intersection\n";
    double tol = 1e-7;
    // Outcomes: can find smallest t>0

    int min_i = -1;
    double min_t = 1e10;
    for(int i=0; i<joltages.size(); i++){
        const auto& u = matrix.at(i);

        if(std::abs(u.dot(v)) < tol){
            std::cout << "same plane\n";
            continue;
        }
        
        std::cout << "j,u,x,v: " <<  joltages.at(i) << " | " << u.transpose() << " | " << x.transpose() << " | " << v.transpose() << "\n";


        double dif = joltages.at(i) - u.dot(x);
        double den = u.dot(v);
        double t = dif/den;

        std::cout << "dif: " << dif << " den: " << den << " t: " << t <<  "\n";

        if(t < min_t && t > -tol){
            min_t = t;
            min_i = i;
        }
    }

    // No solution found - impossible
    if(min_i == -1){
        std::cout << "none found\n";
        return std::nullopt;
    }

    return std::pair<int, double>{min_i, min_t};
}

void print_current_joltages(const V& x_in, const std::vector<V>& matrix, const std::vector<int>& joltages){

    std::cout << "Current joltages:\n";
    for(unsigned int i=0; i<joltages.size(); i++){
        const auto& row = matrix.at(i);
        double joltage = 1.0*joltages.at(i);

        double A = x_in.dot(row);
        std::cout << row.transpose() << " -> " << A << " (" << joltage << ")\n";
    }    
}

V enforce_inequality(const V& x_in, const std::vector<V>& matrix, const std::vector<int>& joltages){

    std::cout << "Enforcing inequality:\n";
    
    unsigned int num_joltages = joltages.size();
    auto x = x_in;

    for(unsigned int i=0; i<num_joltages; i++){
        const auto& u = matrix.at(i);
        const double A = joltages.at(i);

        std::cout << "x,u,A: " << x.transpose() << " | " << u.transpose() << " | " << A << "\n";

        double num = A - u.dot(x);
        double den = u.dot(u);
        double t = num/den;
        if(t<0){
            continue;
        }

        x = x + u*t;
        // std::cout << "n/d: " << num << "/" << den << " t:" << t << " | " << x.transpose() << "\n";

    }

    return x;
}


L second_part(const std::vector<Machine>& machines) {

    L count = 0;
    for(const auto& machine: machines){
        auto joltages = machine.joltages;

        unsigned int num_buttons = machine.buttons.size();
        unsigned int num_joltages = joltages.size();
        std::vector<Eigen::VectorXd> matrix(num_joltages, Eigen::VectorXd::Zero(num_buttons));
        
        for(unsigned int i=0; i<num_buttons; i++){
            const auto& button = machine.buttons.at(i);
            for(int digit: button){
                matrix[digit][i] = 1;
            }
        }

        // xi > 0
        for(unsigned i=0; i<num_buttons; i++){
            Eigen::VectorXd unit = Eigen::VectorXd::Zero(num_buttons);
            unit[i] = 1.0;
            matrix.push_back(unit);
            joltages.push_back(0);
        }

        std::cout << "matrix:\n";
        for(const auto& row: matrix){
            for(const auto& cell: row){
                std::cout << cell << " ";
            }
            std::cout << "\n";
        }

        // The restrictions are expressed as u.x = A, and we want to minimize
        // the sum of coordinates in x. To minimize, we can impose that A <= u.x and
        // run a minimization algorithm. But first we need to make sure that this condition
        // holds for every restriction

        // Quero encontrar t tal que x + u*t esteja na zona A <= u.x
        // A <= u.(x + u*t) 
        // A <= u.x + u.u * t
        // A - u.x <= u.u * t

        Eigen::VectorXd x = Eigen::VectorXd::Zero(num_buttons);

        x = enforce_inequality(x, matrix, joltages);


        // Neste ponto sei que x satisfaz todas as desigualdades

        print_current_joltages(x, matrix, joltages);


        Eigen::VectorXd v = Eigen::VectorXd::Zero(num_buttons);
        Eigen::VectorXd cost = -Eigen::VectorXd::Ones(num_buttons);
        // v(0) = -1.0;
        v = cost;
        
        std::cout << "x,v: " << x.transpose() << " | " << v.transpose() << "\n"; 

        // Find the closest restriction: u.X + u.v*t = a: (a - u.X)/(u.v) = t
        unsigned int i = 10;
        std::vector<V> orthogonal_vectors;

        while(i-- > 0) {
            std::cout << "-------- New iteration -------\n";
            std::cout << "x,v: " << x.transpose() << " | " << v.transpose() << "\n";
            
            // x can only move along the planes that have been touched so far, so
            // it needs to be projected onto them
            
            // for(const V& vector: orthogonal_vectors){
            //     v = remove_projection(v, vector);
            //     std::cout << "   removed " << vector.transpose() << " projection: " << v.transpose() << "\n";
            // }

            auto maybe_new = next_intersection(x, v, matrix, joltages);
            if(!maybe_new){
                std::cout << "No new intersection\n";
                break;
            }
            auto [new_i, t] = *maybe_new; 
            V u = matrix.at(new_i);
            
            std::cout << "Next intersection: " << u.transpose() << " t: " << t << "\n";

            // std::cout << "Adding new plane vector to orthogonal vectors\n";
            // for(const V& vector: orthogonal_vectors){
            //     u = remove_projection(u, vector);
            //     std::cout << "   removed " << vector.transpose() << " projection: " << u.transpose() << "\n";
            // }
            // orthogonal_vectors.push_back(u);

            // std::cout << "Current orthogonal vectors:\n";
            // for(const auto& ve: orthogonal_vectors){
            //     std::cout << ve.transpose() << "\n";
            // }
            
            // double dif = joltages.at(new_i) - u.dot(x);
            // std::cout << "Dif: " << dif << "\n";
            // double den = -u.dot(v);
            // std::cout << "Den: " << den << "\n" << std::flush;
            // double tt = dif/den;
            // std::cout << "modified t: " << tt << " \n";
            x = x + v*t;

            v = cost;
            v = remove_projection(v, u);

            std::cout << "update x,v: " << x.transpose() << " | " << v.transpose() << "\n";
            print_current_joltages(x, matrix, joltages);
        }


    




    }

    return 0;
}

int main(int argc, char** argv){
    if(argc != 2){
        std::cerr << "Not enough arguments. Exiting.\n";
        return 1;
    }

    auto lines = utils::parse_input(argv[1]);
    const auto output = parse(lines);
    std::cout << output << "\n";

    L count = first_part(output.machines);
    std::cout << "Count: " << count << "\n";

    L count2 = second_part(output.machines);
    std::cout << "Count: " << count2 << "\n";

}
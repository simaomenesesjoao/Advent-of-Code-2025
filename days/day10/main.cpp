#include <optional>
#include <string>
#include <vector>
#include <iostream>
#include "utils/utils.h"
#include "utils/lp.h"
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

L second_part(const std::vector<Machine>& machines) {

    L count = 0;
    for(const auto& machine: machines){

        unsigned int num_buttons = machine.buttons.size();
        unsigned int num_joltages = machine.joltages.size();

        Eigen::MatrixXd matrix = Eigen::MatrixXd::Zero(num_joltages, num_buttons);
        Eigen::VectorXd constraints = Eigen::VectorXd(num_joltages);
        
        for(unsigned int i=0; i<num_buttons; i++){
            const auto& button = machine.buttons.at(i);
            for(int digit: button){
                matrix(digit,i) = 1;
            }
        }

        for(unsigned int i=0; i<num_joltages; i++){
            constraints(i) = machine.joltages.at(i);
        }

        Eigen::VectorXd cost = Eigen::VectorXd::Ones(num_buttons);



        auto sol = lp_minimize(matrix, constraints, cost);


        std::cout << "Cost: " << sol.cost << "\n";
        count += sol.cost;

    }

    return count;
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
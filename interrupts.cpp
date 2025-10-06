/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 * @author Sohaila Haroun
 * @author Zaineb Ben Hmida
 *
 */

 #include <random>
 #include <cmath>
 #include "interrupts.hpp"

 int main(int argc, char** argv) {
    //std::cout << "Hello World!" << std::endl;
     //vectors is a C++ std::vector of strings that contain the address of the ISR
     //delays  is a C++ std::vector of ints that contain the delays of each device
     //the index of these elemens is the device number, starting from 0
     auto [vectors, delays] = parse_args(argc, argv);
     std::ifstream input_file(argv[1]);
 
     std::string trace;      //!< string to store single line of trace file
     std::string execution;  //!< string to accumulate the execution output
 
     /******************ADD YOUR VARIABLES HERE*************************/
    int current_time = 0;
    const int context_save_time = 10; 
    const int isr_activity_time = 40;
    const float cpu_relative = CPU_SPEED / 100;
 
 
     /******************************************************************/
 
     //parse each line of the input trace file
     while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);
 
         /******************ADD YOUR SIMULATION CODE HERE*************************/
        if (activity == "CPU") {
            execution += std::to_string(current_time) + ", " +
                        std::to_string(static_cast<int>(std::ceil(duration_intr*cpu_relative))) + ", CPU burst\n"; //made CPU bursts depend on speed
            current_time += static_cast<int>(std::ceil(duration_intr*cpu_relative));
        }
        else if (activity == "SYSCALL") {
            auto [execution_history, new_time] = intr_boilerplate(current_time, duration_intr, 
                                                                context_save_time, vectors);
            execution += execution_history;
            current_time = new_time;

                        // ISR Body Execution, each activty between 40ms-200ms unitl isr done
            int device_delay = delays[duration_intr-1];
            //setting up random num gen to randomuze activity time 
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(40, 200); // random ISR activity time between 40 and 200 ms
            while (device_delay > 0) {
                int chunk = dist(gen); // generate random chunk size
                chunk = std::min(chunk, device_delay);
                execution += std::to_string(current_time) + ", " + std::to_string(chunk) + ", ISR SYSCALL Activity\n";
                current_time += chunk;
                device_delay -= chunk;
            }

            execution += std::to_string(current_time) + ", 1, IRET\n";
            current_time++;
        }
        else if (activity == "END_IO") {
            auto [execution_history, new_time] = intr_boilerplate(current_time, duration_intr,
                                                                context_save_time, vectors);
            execution += execution_history;
            current_time = new_time;

            // ISR Body Execution, each activty between 40ms-200ms unitl isr done
            int device_delay = delays[duration_intr-1];
            //setting up random num gen to randomuze activity time 
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(40, 200); // random ISR activity time between 40 and 200 ms
            while (device_delay > 0) {
                int chunk = dist(gen); // generate random chunk size
                chunk = std::min(chunk, device_delay);
                execution += std::to_string(current_time) + ", " + std::to_string(chunk) + ", ISR END_IO Activity\n";
                current_time += chunk;
                device_delay -= chunk;
            }

            execution += std::to_string(current_time) + ", 1, IRET\n";
            current_time++;
        }
 
 
         /************************************************************************/
 
     }
 
     input_file.close();
 
     write_output(execution);
 
     return 0;
 }
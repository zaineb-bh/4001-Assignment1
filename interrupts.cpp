/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 * @author Sohaila Haroun
 * @author Zaineb Ben Hmida
 *
 */

 
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
    const  int isr_activity_time = 40;
 
 
     /******************************************************************/
 
     //parse each line of the input trace file
     while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);
 
         /******************ADD YOUR SIMULATION CODE HERE*************************/
        if (activity == "CPU") {
            execution += std::to_string(current_time) + ", " +
                        std::to_string(duration_intr) + ", CPU burst\n";
            current_time += duration_intr;
        }
        else if (activity == "SYSCALL") {
            auto [execution_history, new_time] = intr_boilerplate(current_time, duration_intr, 
                                                                context_save_time, vectors);
            execution += execution_history;
            current_time = new_time;

            // ISR Body Execution, I don't get it, each activty 40ms unitl isr done? for now
            int device_delay = delays[duration_intr-1];
            while (device_delay > 0) {
                int chunk = std::min(isr_activity_time, device_delay);
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

            // ISR Body Execution, each activty 40ms unitl isr done
            int device_delay = delays[duration_intr-1];
            while (device_delay > 0) {
                int chunk = std::min(isr_activity_time, device_delay);
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
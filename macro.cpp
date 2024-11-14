#include <iostream>
#include <stdint.h>
#include <termcolor/termcolor.hpp>
#include <indicators/progress_spinner.hpp>

using namespace indicators;

int main() {
    indicators::ProgressSpinner spinner{
        option::PostfixText{"Compilando codigo Isa! 😼"},
        option::ForegroundColor{Color::yellow},
        option::SpinnerStates{std::vector<std::string>{"⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂", "⠁"}},
        option::FontStyles{std::vector<FontStyle>{FontStyle::bold}}
    };
    auto job = [&spinner]() {
     int progress = 0;
        while (progress <= 100) {
            if (progress == 100) {
                spinner.set_option(indicators::option::ForegroundColor{indicators::Color::green});
                spinner.set_option(indicators::option::PrefixText{"✔"});
                spinner.set_option(indicators::option::ShowSpinner{false});
                spinner.set_option(indicators::option::ShowPercentage{false});
                spinner.set_option(indicators::option::PostfixText{"Compilado com sucesso!"});
                spinner.mark_as_completed();
                break;
            } else {
                spinner.tick();
                progress += 1;  // Simula o progresso
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(40));
        }
    };
std::thread thread(job);
thread.join();  
    #ifdef _WIN32
        std::cout << "Compilando para Windows" << std::endl;
    #elif defined(__APPLE__) && defined(__MACH__)
        std::cout << "Compilando para macOS" << std::endl;
    #elif defined(__linux__)
        std::cout << termcolor::color<211, 54, 130> << "[-] Target X86_64 Linux" << std::endl;
    #else
        std::cout << "Sistema operacional desconhecido" << std::endl;
    #endif
    return 0;
}
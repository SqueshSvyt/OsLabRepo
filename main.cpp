#include <iostream>
#include "Linker.h"

int main(){
    std::cout << "Program start: " << '\n';
    try{
        if(Run() == 1){
            std::cout << "Program stopped or killed: " << '\n';
            return 0;
        }
    }catch(const std::exception& e){
        std::cerr << "Error: " << e.what() << std::endl;
    }
    std::cout << "Program end without problems!" << '\n';
    return 0;
}
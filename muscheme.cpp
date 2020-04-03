#include <string>
#include <iostream>


std::string eval(std::string cmd) {
    return cmd;
}

void repl() {
    std::string prompt="μλ>";
    std::string line;
    std::string resp;
    bool esc=false;
    while (!esc) {
        std::cout << prompt;
        std::cin >> line;
        if (line=="(quit)") esc=true;
        else {
            resp=eval(line);
            std::cout << "\n-> " + resp +"\n";
        }
    }
}

int main(int argc, char *argv[]) {
    repl();
    return 0;
}

#include <stdio.h>
#include <fstream>

int main(int , char **) {
    std::ofstream ofs("version.h");
    ofs << "constexpr int version_const = 42;";
    printf("Host done\n");
}

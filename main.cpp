#include "csv.h"
#include <cassert>

int main() {
    io::CSVReader<3> in("bom_and_newlines.csv");
    in.read_header(io::ignore_extra_column, "vendor", "size", "speed");
    std::string vendor; int size; double speed;
    
    in.read_row(vendor, size, speed);
    assert(vendor == "Intel" && size == 16 && speed == 3.5);
}

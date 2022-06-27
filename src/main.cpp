#include "Image.h"
#include <ctime>
#include <iostream>
#include <filesystem>

int main(int argc, char** argv) {
    clock_t start, end;
    start = clock();

    std::string path = "/Users/ebs/art/filters/emma";
    for (const auto & entry : std::filesystem::directory_iterator(path))
        std::cout << entry.path() << std::endl;
    

    Image liar = Image("face.png");
    liar.liar();
    liar.write("face_liar.png");

    end = clock();
    printf ("Process finished in %0.8f seconds.\n",
            ((float) end - start) / CLOCKS_PER_SEC);
    return 0;
}

#include "Image.h"
#include <ctime>
#include <iostream>
#include <filesystem>

int main(int argc, char** argv) {
    clock_t start = clock();

    std::string directory = "emma";
    std::string path = "img/" + directory;
    std::string converted_path = path + "_converted";
    std::filesystem::create_directories(converted_path);

    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() != ".png") continue;
        
        // make filenames
        std::string source_filename = path + "/" + entry.path().filename().c_str();
        char* source_filename_char_ptr = const_cast<char*>(source_filename.c_str());
        std::string destination_filename = converted_path + "/" + entry.path().filename().c_str();
        char* destination_filename_char_ptr = const_cast<char*>(destination_filename.c_str());

        // call liar
        Image liar = Image(source_filename_char_ptr);
        liar.liar();
        liar.write(destination_filename_char_ptr);
    }


    std::cout << "Process finished in " << ((float) clock() - start) / CLOCKS_PER_SEC << " seconds." << std::endl;
    return 0;
}

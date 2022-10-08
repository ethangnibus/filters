#include "Image.h"
#include <ctime>
#include <iostream>
#include <filesystem>
#include <glob.h>

int main(int argc, char** argv) {
    clock_t start = clock();
    
    std::string directory = "curiosity_liar";
    std::string path = "img/" + directory;
    std::string converted_path = path + "_converted";
    std::filesystem::create_directories(converted_path);
    char* const_path = const_cast<char*>(path.c_str());

    size_t fileCount = 0;

    for (const auto & entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() != ".jpg") continue;
        fileCount += 1;
    }

    std::cout << "There are " << fileCount << " files!" << std::endl;

    // std::string startFilename = path + "/0.png";
    // char* start_filename_char_ptr = const_cast<char*>(startFilename.c_str());
    // Image curr_img = Image(start_filename_char_ptr);
    // Image last_img = Image(start_filename_char_ptr);

    



    for (size_t i = fileCount - 1; i >= 0; i -= 1) {
        // make filenames
        std::string source_filename = path + "/" + std::to_string(i) + ".jpg";
        char* source_filename_char_ptr = const_cast<char*>(source_filename.c_str());

        std::string last_filename = converted_path + "/" + std::to_string(i+1) + ".jpg";
        char* last_filename_char_ptr = const_cast<char*>(source_filename.c_str());

        // std::string palette_filename = "img/spike/" + std::to_string(i) + ".jpg";
        // char* palette_filename_char_ptr = const_cast<char*>(palette_filename.c_str());

        std::string destination_filename = converted_path + "/" + std::to_string(i) + ".jpg";
        char* destination_filename_char_ptr = const_cast<char*>(destination_filename.c_str());


        // call liar
        Image curr_img = Image(source_filename_char_ptr);
        Image last_img = Image(last_filename_char_ptr);
        // Image palette = Image(palette_filename_char_ptr);
        // curr_img.colorTo(last_img);
        // curr_img.liar();
        curr_img.inverse();
        curr_img.write(destination_filename_char_ptr);
    }


    std::cout << "Process finished in " << ((float) clock() - start) / CLOCKS_PER_SEC << " seconds." << std::endl;
    return 0;
}

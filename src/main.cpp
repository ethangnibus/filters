#include "Image.h"

int main(int argc, char** argv) {
    Image test("test.png");

    Image liar = test;
    liar.liar();
    liar.write("test_liar.png");

    return 0;
}

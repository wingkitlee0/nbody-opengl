#include "nbody.hpp"


int main(int argc, char* args[]) {
    //NBody* nbody = NBody_new();
    auto nbody = new NBody();
    if (nbody == NULL) {
        return -1;
    }

    nbody->NBody_run();

    delete nbody;
    return 0;
}

#include <mist.hpp>

int main(int ac, char **av)
{
    if (ac != 4) {
        std::cout << "Usage: ./renderer path_to_vox_file chunk_size debug(0 or 1)" << std::endl;
        return 84;
    } else {
        int chunkSize = 0;
        int debug = 0;
        try {
            chunkSize = atoi(av[2]);
            if (chunkSize <= 0)
                throw std::exception();
            debug = atoi(av[3]);
            if (debug != 0 && debug != 1)
                throw std::exception();
        } catch (std::exception &e) {
            std::cout << "Usage: ./renderer path_to_vox_file chunk_size" << std::endl;
            return 84;
        }
        Engine engine;
        engine.init(chunkSize, debug);
        engine.loadVoxFile(av[1]);
        engine.run();
    }
    return 0;
}

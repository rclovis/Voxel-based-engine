#include <mist.hpp>

int main(int ac, char **av)
{
    if (ac != 3) {
        std::cout << "Usage: ./renderer path_to_vox_file chunk_size debug(0 or 1)" << std::endl;
        return 84;
    } else {
        int chunkSize = 0;
        try {
            chunkSize = atoi(av[2]);
            if (chunkSize <= 0)
                throw std::exception();
        } catch (std::exception &e) {
            std::cout << "Usage: ./renderer path_to_vox_file chunk_size" << std::endl;
            return 84;
        }
        Engine engine;
        engine.init(chunkSize);
        engine.loadVoxFile(av[1]);
        engine.run();
    }
    return 0;
}

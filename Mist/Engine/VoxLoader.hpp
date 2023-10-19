#pragma once
#include "VoxelRenderer.hpp"

class VoxLoader {
    public:
        VoxLoader(VoxelRenderer *_voxelRenderer);
        ~VoxLoader();
        void loadVox(const char *path);
        void loadPalette(FILE *file, int totalSize);
        void voxelDataToChunks(std::vector<Voxel> voxels, int sizeX, int sizeY, int sizeZ);

    protected:
    private:
        VoxelRenderer *_voxelRenderer;
        std::vector<uint32_t> _palette;
};

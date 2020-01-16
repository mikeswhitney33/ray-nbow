#include "../ray-tracer/ray-tracer.hpp"
#include <cstdlib>

#ifdef DATA_DIR
std::string data_dir = DATA_DIR;
#else
std::string data_dir = "../../test/test_data";
#endif // DATA_DIR

#include <sstream>

int main(int argc, char ** argv)
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <outname>" << std::endl;
        return -1;
    }

    RayTracingScene scene = RayTracingScene::FromScene(data_dir + "/scenes/basic.scene", data_dir);
    float *pix = scene.getDistances(vec3{0, 0, -1}, {0, 0, 0}, {0, 1, 0});
    scene.savePPM("test.ppm", pix);
    delete [] pix;
#ifdef __APPLE__
    return system("open test.ppm");
#else
    return system("display test.ppm");
#endif // __APPLE__
}

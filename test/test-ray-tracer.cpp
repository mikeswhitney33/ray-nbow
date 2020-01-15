#include "../ray-tracer/ray-tracer.hpp"
#include <cstdlib>

int main(int argc, char ** argv)
{
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <outname>" << std::endl;
        return -1;
    }
    RayTracingScene scene;
    scene.addShape(new Sphere({0, 0, 0}, 0.2));
    scene.savePPM("test.ppm");
    system("display test.ppm");
    return 0;
}
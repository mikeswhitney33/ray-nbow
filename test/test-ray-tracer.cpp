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
    std::stringstream ss;
    ss << "(1, 2, 3)";
    vec3 x;
    ss >> x;
    std::cout << "(" << x.x << "," << x.y << "," << x.z << ")" << std::endl;
    if(argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <outname>" << std::endl;
        return -1;
    }
    std::cout << rotate(radians(45), {0, 1, 0}) << std::endl;

    RayTracingScene scene;
    scene.addShape(new Sphere({-0.2, 0, 0}, 0.2));
    Transform t({0.5, 0, 0}, {0, 0, 0}, {0.5, 0.5, 0.5});
    scene.addObj(data_dir + "/objs/cube.obj", t);
    float *pix = scene.getDistances(vec3{0, 0, -1}, {0, 0, 0}, {0, 1, 0});
    scene.savePPM("test.ppm", pix);
    delete [] pix;
    system("display test.ppm");
    return 0;
}

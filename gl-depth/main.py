"""
main.py
this runs the main program
"""
import json
import moderngl
import numpy as np
import pywavefront

from PIL import Image

def load_obj(obj):
    filename = obj['filename']
    matid = obj['matid']
    wvobj = pywavefront.Wavefront(filename)
    primitives = []
    index = 0
    for mesh in wvobj.mesh_list:
        for mat in mesh.materials:
            for i in range(0, len(mat.vertices), 9):
                shape = {
                    "type":"triangle",
                    "matid":matid,
                    "a":[mat.vertices[i], mat.vertices[i+1], mat.vertices[i+2]],
                    "b":[mat.vertices[i+3], mat.vertices[i+4], mat.vertices[i+5]],
                    "c":[mat.vertices[i+6], mat.vertices[i+7], mat.vertices[i+8]]
                }
                primitives.append(shape)
                print(index)
                index += 1
    return primitives


def load_scene(filename):
    with open(filename, "r") as file:
        scene_data = json.load(file)
    if 'ambient' not in scene_data:
        scene_data['ambient'] = [0.7, 0.7, 0.7]
    if 'background' not in scene_data:
        scene_data['background'] = [0.2, 0.3, 0.3]
    if 'shapes' not in scene_data:
        scene_data['shapes'] = []
    if 'materials' not in scene_data:
        scene_data['materials'] = []
    if 'width' not in scene_data:
        scene_data['width'] = 800
    if 'height' not in scene_data:
        scene_data['height'] = 600
    if 'fov' not in scene_data:
        scene_data['fov'] = 45
    if 'eye' not in scene_data:
        scene_data['eye'] = [0, 0, -3]
    if 'center' not in scene_data:
        scene_data['center'] = [0, 0, 0]
    if 'up' not in scene_data:
        scene_data['up'] = [0, 1, 0]
    shapes = scene_data['shapes']
    objs = [shape for shape in shapes if shape['type'] == 'obj']
    primitives = [shape for shape in shapes if shape['type'] != 'obj']
    for obj in objs:
        primitives.extend(load_obj(obj))
    scene_data['shapes'] = primitives

    return scene_data

def normalize(vec):
    return vec / np.sqrt(np.dot(vec, vec))


def lookat(eye, center, up):
    forward = normalize(center - eye)
    side = normalize(np.cross(forward, up))
    upward = normalize(np.cross(side, forward))
    return np.array([
        [-side[0], upward[0], -forward[0], 0],
        [-side[1], upward[1], -forward[1], 0],
        [-side[2], upward[2], -forward[2], 0],
        [-np.dot(eye, side), -np.dot(eye, upward), np.dot(eye, forward), 1]])

def perspective(fov, aspect, z_near, z_far):
    zmul = (-2.0 * z_near * z_far) / (z_far - z_near)
    ymul = (1.0 / np.tan(fov * np.pi / 360))
    xmul = ymul / aspect
    return np.array([[xmul, 0, 0, 0],[0, ymul, 0, 0],[0, 0, -1, -1],[0, 0, zmul, 0]])

def vec32str(vec3):
    return f"vec3({','.join([str(v) for v in vec3])})"

def mat2str(idx, mat):
    return f"materials[{idx}] = Material({mat['type']},{vec32str(mat['ka'])},{vec32str(mat['kd'])},{vec32str(mat['ks'])},{mat['alpha']});"

def mats2str(mats):
    return f"""Material materials[{len(mats)}];
{''.join([mat2str(i, mat) for i, mat in enumerate(mats)])}
"""

def sphere2str(idx, sphere):
    return f"shapes[{idx}] = makeShape(materials[{sphere['matid']}],makeSphere({vec32str(sphere['center'])},{sphere['rad']}));"

def triangle2str(idx, triangle):
    return f"shapes[{idx}] = makeShape(materials[{triangle['matid']}],makeTriangle({vec32str(triangle['a'])}, {vec32str(triangle['b'])}, {vec32str(triangle['c'])}));"

def shape2str(idx, shape):
    if shape['type'] == 'sphere':
        return sphere2str(idx, shape)
    elif shape['type'] == 'triangle':
        return triangle2str(idx, shape)
    else:
        return ""

def shapes2str(shapes):
    return ''.join([shape2str(i, shape) for i, shape in enumerate(shapes)])

def main():
    """The entry point to the program"""
    ctx = moderngl.create_standalone_context()

    with open("shaders/pt-vert.glsl", "r") as vert_file:
        vert_code = vert_file.read()
    with open("shaders/pt-frag.glsl", "r") as frag_file:
        frag_code = frag_file.read()

    scene_data = load_scene("scenes/basic.json")

    frag_code = frag_code.replace("{%% shapes %%}", f"""
#define NUM_SHAPES {len(scene_data['shapes'])}
Shape shapes[NUM_SHAPES];
void initShapes() {{
    {mats2str(scene_data['materials'])}
    {shapes2str(scene_data['shapes'])}
}}
    """)
    width = scene_data['width']
    height = scene_data['height']

    prog = ctx.program(
        vertex_shader=vert_code,
        fragment_shader=frag_code,
    )

    view = lookat(np.array(scene_data['eye']), np.array(scene_data['center']), np.array(scene_data['up']))
    prog['view'] = tuple(view.reshape(-1))
    prog['fov'] = scene_data['fov']
    prog['aspect'] = width / height
    prog['background'] = tuple(scene_data['background'])
    prog['ambient'] = tuple(scene_data['ambient'])

    vertices = np.array([-1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0])

    vbo = ctx.buffer(vertices.astype('f4').tobytes())
    vao = ctx.simple_vertex_array(prog, vbo, 'in_vert')

    fbo = ctx.simple_framebuffer((width, height))
    fbo.use()
    fbo.clear(0.2, 0.3, 0.3, 1.0)
    vao.render(moderngl.TRIANGLE_FAN)

    image = Image.frombytes('RGB', fbo.size, fbo.read(), 'raw', 'RGB', 0, -1).show()
    npimage = np.array(image)
    print(npimage.shape, npimage.min(), npimage.max())

if __name__ == "__main__":
    main()

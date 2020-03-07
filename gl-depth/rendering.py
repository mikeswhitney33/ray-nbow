"""
rendering.py
this handles all the rendering functions
"""
import moderngl
import numpy as np
from PIL import Image

from scene_io import load_scene


def normalize(vec):
    """ normalizes a vector """
    return vec / np.sqrt(np.dot(vec, vec))


def lookat(eye, center, up_vec):
    """
    makes a lookat matrix
    @param eye: the location of the camera
    @param center: the point the camera is looking at
    @param up: the general up direction
    """
    forward = normalize(center - eye)
    side = normalize(np.cross(forward, up_vec))
    upward = normalize(np.cross(side, forward))
    return np.array([
        [-side[0], upward[0], forward[0], 0],
        [-side[1], upward[1], forward[1], 0],
        [-side[2], upward[2], forward[2], 0],
        [-np.dot(eye, side), -1 * np.dot(eye, upward), np.dot(eye, forward), 1]])


def perspective(fov, aspect, z_near, z_far):
    """
    perspective:
    calculates the perspective matrix.
    @param fov - float: the field of view
    @param aspect - float: the aspect ratio
    @param z_near - float: the near clipping plane
    @param z_far - float: the far clipping plane
    """
    zmul = (-2.0 * z_near * z_far) / (z_far - z_near)
    ymul = (1.0 / np.tan(fov * np.pi / 360))
    xmul = ymul / aspect
    return np.array([
        [xmul, 0, 0, 0],
        [0, ymul, 0, 0],
        [0, 0, -1, -1],
        [0, 0, zmul, 0]])


def vec32str(vec3):
    """
    converts a vec3 to a string
    """
    return f"vec3({','.join([str(v) for v in vec3])})"


def mat2str(idx, mat):
    """
    converts a material to a string
    """
    return f"materials[{idx}] = Material(" + \
        f"{mat['type']},{vec32str(mat['ka'])}," + \
        f"{vec32str(mat['kd'])}," + \
        f"{vec32str(mat['ks'])},{mat['alpha']});"


def mats2str(mats):
    """
    converts a list of materials to a string
    """
    return f"""Material materials[{len(mats)}];
{''.join([mat2str(i, mat) for i, mat in enumerate(mats)])}
"""


def sphere2str(idx, sphere):
    """
    converts a sphere to a string
    """
    return f"shapes[{idx}] = makeShape(" + \
        f"materials[{sphere['matid']}]," + \
        f"makeSphere({vec32str(sphere['center'])}," + \
        f"{sphere['rad']}));"


def triangle2str(idx, triangle):
    """
    converts a triangle to a string
    """
    return f"shapes[{idx}] = makeShape(" + \
        f"materials[{triangle['matid']}]," + \
        f"makeTriangle({vec32str(triangle['a'])}," + \
        f"{vec32str(triangle['b'])}," + \
        f"{vec32str(triangle['c'])}));"


def shape2str(idx, shape):
    """
    converts a shape to a string
    """
    if shape['type'] == 'sphere':
        return sphere2str(idx, shape)
    elif shape['type'] == 'triangle':
        return triangle2str(idx, shape)
    else:
        return ""


def shapes2str(shapes):
    """
    converts a list of shapes to a string
    """
    return ''.join([shape2str(i, shape) for i, shape in enumerate(shapes)])


def trace2str(depth):
    """
    converts a trace function to a string
    """
    if depth == 0:
        return """
vec3 trace0(vec3 orig, vec3 dir) {
    int id;
    float t;
    if(!intersect(orig, dir, id, t)) {
        return background;
    }
    Shape shape = shapes[id];
    vec3 pt = orig + dir * t;
    vec3 N = getNormal(shape, pt, dir);

    if(shape.mat.matType == MAT_TYPE_LIGHT) {
        return shape.mat.kd;
    }
    vec3 color = ambient * shape.mat.ka;
    vec3 V = normalize(-dir);
    for(int i = 0;i < NUM_SHAPES;i++) {
        if(i == id || shapes[i].mat.matType != MAT_TYPE_LIGHT) {
            continue;
        }
        vec3 L = -getShapeDirection(shapes[i], pt);
        int _id;
        float _t;
        if(intersect(pt + N * 0.0000001, -L, _id, _t) && _id != i) {
            continue;
        }

        vec3 R = reflect(L, N);

        vec3 diff = shape.mat.kd * max(dot(L, N), 0) * shapes[i].mat.kd;
        vec3 spec = shape.mat.ks * pow(max(dot(R, V), 0), shape.mat.alpha) * shapes[i].mat.ks;
        color += diff + spec;
    }
    return color;
}
"""
    else:
        return f"""
vec3 trace{depth}(vec3 orig, vec3 dir) {{
    int id;
    float t;
    if(!intersect(orig, dir, id, t)) {{
        return background;
    }}
    Shape shape = shapes[id];
    vec3 pt = orig + dir * t;
    vec3 N = getNormal(shape, pt, dir);

    if(shape.mat.matType == MAT_TYPE_LIGHT) {{
        return shape.mat.kd;
    }}

    vec3 kd = shapes[id].mat.kd;
    if(shape.mat.matType == MAT_TYPE_SPEC) {{
        vec3 reflOrig = pt + N * 0.0000001;
        vec3 newDir = reflect(dir, N);
        kd = trace{depth-1}(reflOrig, newDir);
    }}
    else if(shape.mat.matType == MAT_TYPE_REFR) {{
        float eps;
        vec3 newDir = getRefrDir(dir, N, 1, 1.03, eps);
        vec3 refrOrig = pt + N * eps;

        kd = trace{depth-1}(refrOrig, newDir);
    }}
    vec3 color = ambient * shape.mat.ka;
    vec3 V = normalize(-dir);
    for(int i = 0;i < NUM_SHAPES;i++) {{
        if(i == id || shapes[i].mat.matType != MAT_TYPE_LIGHT) {{
            continue;
        }}
        vec3 L = -getShapeDirection(shapes[i], pt);
        int _id;
        float _t;
        if(intersect(pt + N * 0.0000001, -L, _id, _t) && _id != i) {{
            continue;
        }}

        vec3 R = reflect(L, N);

        vec3 diff = kd * max(dot(L, N), 0) * shapes[i].mat.kd;
        vec3 spec = shape.mat.ks * pow(max(dot(R, V), 0), shape.mat.alpha) * shapes[i].mat.ks;
        color += diff + spec;
    }}
    return color;
}}
"""

def get_program(ctx, shapes, materials, depth):
    """ gets the shader program """
    with open("shaders/pt-vert.glsl", "r") as vert_file:
        vert_code = vert_file.read()
    with open("shaders/pt-frag.glsl", "r") as frag_file:
        frag_code = frag_file.read()

    frag_code = frag_code.replace("{%% shapes %%}", f"""
#define NUM_SHAPES {len(shapes)}
Shape shapes[NUM_SHAPES];
void initShapes() {{
    {mats2str(materials)}
    {shapes2str(shapes)}
}}
    """)

    frag_code = frag_code.replace("{%% trace %%}", f"""
{''.join([trace2str(d) for d in range(depth)])}
vec3 trace(vec3 eye, vec3 dir) {{
    return trace{depth-1}(eye, dir);
}}
""")

    prog = ctx.program(
        vertex_shader=vert_code,
        fragment_shader=frag_code,
    )
    return prog


def render_scene(scene_file, random_func, depth, image_size):
    """renders the scene and returns a pil image"""
    ctx = moderngl.create_standalone_context()
    scene_data = load_scene(scene_file, random_func)
    prog = get_program(ctx, scene_data['shapes'], scene_data['materials'], depth)

    width, height = image_size

    view = lookat(
        np.array(scene_data['eye']),
        np.array(scene_data['center']),
        np.array(scene_data['up']))
    prog['view'] = tuple(view.reshape(-1))
    prog['fov'] = scene_data['fov']
    prog['aspect'] = width / height
    prog['background'] = tuple(scene_data['background'])
    prog['ambient'] = tuple(scene_data['ambient'])

    vertices = np.array([-1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0])

    vbo = ctx.buffer(vertices.astype('f4').tobytes())
    vao = ctx.simple_vertex_array(prog, vbo, 'in_vert')

    fbo = ctx.simple_framebuffer(image_size)
    fbo.use()
    fbo.clear(0.2, 0.3, 0.3, 1.0)
    vao.render(moderngl.TRIANGLE_FAN)

    return Image.frombytes('RGB', fbo.size, fbo.read(), 'raw', 'RGB', 0, -1)

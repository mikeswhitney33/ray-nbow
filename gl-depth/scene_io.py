""" this handles all the IO for scenes """
import json
import pywavefront

def load_obj(obj):
    """
    load_obj
    loads an obj file
    @param obj - the obj object found in the scene data
    """
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


def load_scene(filename, sample_func):
    """
    load_scene
    loads a scene from a json file
    @param filename - str: the name of the json file
    @param sample_func - function: the function for sampling materials
    """
    with open(filename, "r") as file:
        scene_data = json.load(file)
    if 'ambient' not in scene_data:
        scene_data['ambient'] = [0.7, 0.7, 0.7]
    if 'background' not in scene_data:
        scene_data['background'] = [0.2, 0.3, 0.3]
    if 'shapes' not in scene_data:
        scene_data['shapes'] = []
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

    materials = []
    matids = []
    for shape in scene_data['shapes']:
        if shape['matid'] not in matids:
            matids.append(shape['matid'])
            materials.append(sample_func())
    scene_data['materials'] = materials
    return scene_data

import argparse
import sys

import bpy
import random
import numpy as np


from PIL import Image, ImageFile
ImageFile.LOAD_TRUNCATED_IMAGES = True

IMAGE = None

def remove_all_materials():
    for material in bpy.data.materials:
        material.user_clear()
        bpy.data.materials.remove(material)

def set_random_color(node):
    global IMAGE
    if IMAGE is None:
        color = (
            np.random.uniform(0, 1),
            np.random.uniform(0, 1),
            np.random.uniform(0, 1), 1)
    else:
        color = IMAGE[np.random.randint(0, IMAGE.shape[0]-1)]
        color = (*tuple(color), 1)
    node.inputs['Color'].default_value = color


def make_diffusebsdfNode(mat):
    diffuse = mat.node_tree.nodes.new(type="ShaderNodeBsdfDiffuse")
    set_random_color(diffuse)
    return diffuse

def make_glossybsdfNode(mat):
    glossy = mat.node_tree.nodes.new(type="ShaderNodeBsdfGlossy")
    set_random_color(glossy)
    glossy.inputs['Roughness'].default_value = random.uniform(0, 0.5)
    return glossy

def make_glassbsdfNode(mat):
    glass = mat.node_tree.nodes.new(type="ShaderNodeBsdfGlass")
    set_random_color(glass)
    glass.inputs['Roughness'].default_value = random.uniform(0, 0.5)
    glass.inputs['IOR'].default_value = random.uniform(0.25, 1.5)
    return glass


BSDF_GENERATORS = (make_diffusebsdfNode, make_glossybsdfNode, make_glassbsdfNode)


def main():
    global IMAGE
    argv = sys.argv
    argv = argv[argv.index("--")+1:]

    parser = argparse.ArgumentParser()
    parser.add_argument("--image", type=str, default=None)
    args = parser.parse_args(argv)
    if args.image is not None:
        print(args.image)
        image = Image.open(args.image).convert("RGB")
        if image is None:
            print("IMAGE NONE!")
        else:
            image = np.array(image, dtype=np.float32) / 255.
            print(image.min(), image.max())
            IMAGE = image.reshape(-1, 3)

    remove_all_materials()
    for i, obj in enumerate(bpy.data.objects):
        if obj.type in ("LIGHT", "CAMERA"):
            continue
        mat_name = f"Material.{i}"
        mat = bpy.data.materials.get(mat_name) or bpy.data.materials.new(mat_name)
        mat.use_nodes = True
        mat.node_tree.nodes.remove(mat.node_tree.nodes.get('Principled BSDF'))
        node = random.choice(BSDF_GENERATORS)(mat)

        mat_output = mat.node_tree.nodes.get('Material Output')

        mat.node_tree.links.new(mat_output.inputs[0], node.outputs[0])
        obj.active_material = mat



if __name__ == "__main__":
    print("=========== Running Script ===========")
    main()
    print("=========== Finished Script ==========")

"""Importa una exportacion del ejecutable y guarda el escenario completo editable.
blender --background --python scripts/crear_escenario_blender.py -- entrada.obj salida.blend
"""
from pathlib import Path
import sys,bpy
from mathutils import Vector
args=sys.argv[sys.argv.index('--')+1:]
source,target=map(lambda s:Path(s).resolve(),args[:2])
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
bpy.ops.wm.obj_import(filepath=str(source),forward_axis='NEGATIVE_Z',up_axis='Y')
scene=bpy.context.scene
scene.world.color=(.035,.035,.05)
bpy.ops.object.light_add(type='AREA',location=(-25,-20,45))
key=bpy.context.object;key.name='Luz_general';key.data.energy=1800;key.data.shape='DISK';key.data.size=35
key.rotation_euler=(Vector((0,0,0))-key.location).to_track_quat('-Z','Y').to_euler()
lights=[(0,3.75,0),(-20,2.55,8),(20,2.55,8),(-20,2.55,-5),(-13,2.55,-17),(13,2.55,-17),(20,2.55,-5)]
lights += [(x,3.8,z) for x,z in [(-15,13),(15,13),(-15,-11),(15,-11),(-5,-17),(5,-17),(-5,17),(5,17)]]
for i,(x,y,z) in enumerate(lights):
    bpy.ops.object.light_add(type='POINT',location=(x,-z,y));o=bpy.context.object;o.name=f'Puntual_{i+1:02d}';o.data.energy=240;o.data.color=(1,.70,.34);o.data.shadow_soft_size=.5
bpy.ops.object.camera_add(location=(38,-40,32))
camera=bpy.context.object;camera.rotation_euler=(-camera.location).to_track_quat('-Z','Y').to_euler();camera.data.lens=32;scene.camera=camera
scene.render.resolution_x=1600;scene.render.resolution_y=900
scene['descripcion']='Escenario original completo exportado del recorrido C++: arquitectura, cinco obras, puestos y personajes estaticos.'
scene['edicion']='Objetos separados como Pieza_N. Editar aqui para trabajo artistico; el codigo C++ sigue siendo la fuente del escenario en ejecucion.'
scene['animaciones']='Sin rigging ni keyframes. No hay acciones animadas.'
bpy.ops.file.pack_all();target.parent.mkdir(parents=True,exist_ok=True)
bpy.ops.wm.save_as_mainfile(filepath=str(target))
print('ESCENARIO EDITABLE GUARDADO:',target)

"""Ejecutar: blender --background --python scripts/crear_obras_blender.py
Obras originales estilizadas; no replicas de una obra artesanal real.
OBJ/MTL estaticos, UV, normales y fuente editable .blend. Sin rig ni animacion.
"""
import bpy, math, random
from mathutils import Vector
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'NocheDeLosRabanos'/'Models'/'Proyecto'
OUT.mkdir(parents=True,exist_ok=True)
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
rng=random.Random(41947650)
def mat(name,c):
    m=bpy.data.materials.new(name);m.diffuse_color=(*c,1);m.use_nodes=True
    bs=m.node_tree.nodes.get('Principled BSDF');bs.inputs['Base Color'].default_value=(*c,1);bs.inputs['Roughness'].default_value=.66
    return m
red=mat('Piel_de_rabano',(.64,.035,.16));white=mat('Corte_blanco',(.96,.90,.78));green=mat('Hojas',(.12,.32,.07));dark=mat('Incisiones',(.10,.035,.04))
husks=[mat('Totomoxtle_'+str(i),(.52+i*.045,.28+i*.04,.09+i*.025)) for i in range(5)]
flowers=[mat('Flor_'+str(i),c) for i,c in enumerate([(.84,.08,.32),(.93,.40,.025),(.95,.71,.04),(.50,.05,.28)])]
wood=mat('Madera',(.29,.13,.055));gold=mat('Centro_flor',(.46,.25,.045))
def finish(o,m):
    o.data.materials.append(m)
    for f in o.data.polygons:f.use_smooth=True
    return o
def sphere(p,s,m):
    bpy.ops.mesh.primitive_uv_sphere_add(segments=16,ring_count=10,location=p);o=bpy.context.object;o.scale=s;return finish(o,m)
def cylinder(a,b,r,m,vertices=12,r2=None):
    a,b=Vector(a),Vector(b);v=b-a
    bpy.ops.mesh.primitive_cone_add(vertices=vertices,radius1=r,radius2=r if r2 is None else r2,depth=v.length,location=(a+b)/2)
    o=bpy.context.object;o.rotation_euler=v.to_track_quat('Z','Y').to_euler();return finish(o,m)
def box(p,s,m):
    bpy.ops.mesh.primitive_cube_add(size=1,location=p);o=bpy.context.object;o.scale=s;o.data.materials.append(m);return o
def mesh(name,verts,faces,m):
    data=bpy.data.meshes.new(name);data.from_pydata(verts,[],faces);data.update();o=bpy.data.objects.new(name,data);bpy.context.collection.objects.link(o);return finish(o,m)
def leaf(start,angle,length,width,m=green,lean=.5):
    x,y,z=start;v=[]
    for i in range(7):
        t=i/6;w=math.sin(math.pi*t)*width
        for side in [-1,0,1]:
            v.append((x+math.cos(angle)*lean*t+math.sin(angle)*w*side,y+math.sin(angle)*lean*t-math.cos(angle)*w*side,z+length*t-(.09 if side else 0)*math.sin(t*math.pi)))
    f=[]
    for i in range(6):
        for j in range(2):a=i*3+j;f.append((a,a+1,a+4,a+3))
    return mesh('Hoja_plegada',v,f,m)
def root(p,scale=1,angle=0):
    x,y,z=p;profile=[(0,.02),(.12,.07),(.28,.25),(.50,.35),(.77,.32),(.93,.19),(1,.09)];v=[]
    for h,r in profile:
        for k in range(20):
            a=k/20*math.tau;v.append((x+r*scale*math.cos(a),y+r*scale*math.sin(a),z+h*scale))
    faces=[]
    for j in range(len(profile)-1):
        for k in range(20):faces.append((j*20+k,j*20+(k+1)%20,(j+1)*20+(k+1)%20,(j+1)*20+k))
    o=mesh('Rabano_tallado',v,faces,red);o.data.materials.append(white)
    for f in o.data.polygons:
        if f.center.z<z+.22*scale:f.material_index=1
    for j in range(5):leaf((x,y,z+scale),angle+j*math.tau/5,.32*scale,.08*scale,lean=.24*scale)
    return o
def figure(x,y,h=1,pose=0):
    root((x,y,.28*h),.70*h)
    sphere((x,y,1.07*h),(.20*h,.17*h,.22*h),red)
    sphere((x,y-.145*h,1.08*h),(.135*h,.05*h,.14*h),white)
    for dx in [-.065,.065]:sphere((x+dx*h,y-.195*h,1.11*h),(.019*h,.016*h,.021*h),dark)
    for side in [-1,1]:
        cylinder((x+side*.13*h,y,.34*h),(x+side*.18*h,y-.04,.05*h),.065*h,white)
        sphere((x+side*.18*h,y-.08,.04*h),(.10*h,.16*h,.045*h),red)
        shoulder=(x+side*.22*h,y,.87*h);hand=(x+side*(.49 if pose else .40)*h,y-.05,(1.12 if pose and side==1 else .66)*h)
        cylinder(shoulder,hand,.055*h,white);sphere(hand,(.075*h,)*3,red)
    for j in range(5):leaf((x,y,1.27*h),j*math.tau/5,.26*h,.06*h,lean=.19*h)

def export_group(name,objects):
    # Eje Blender Z arriba -> OpenGL Y arriba. UV y materiales propios.
    pts=[o.matrix_world@v.co for o in objects for v in o.data.vertices]
    minz=min(p.z for p in pts);maxz=max(p.z for p in pts);scale=1.7/(maxz-minz)
    minx,maxx=min(p.x for p in pts),max(p.x for p in pts);miny,maxy=min(p.y for p in pts),max(p.y for p in pts)
    center=Vector(((minx+maxx)/2,(miny+maxy)/2,minz));used={m.name:m for o in objects for m in o.data.materials}
    with (OUT/(name+'.mtl')).open('w') as f:
        f.write('# Materiales originales del equipo; generacion reproducible\n')
        for n,m in used.items():f.write(f'newmtl {n}\nKd {m.diffuse_color[0]:.5f} {m.diffuse_color[1]:.5f} {m.diffuse_color[2]:.5f}\nKs 0.08 0.08 0.08\nNs 24\n\n')
    with (OUT/(name+'.obj')).open('w') as f:
        f.write(f'# Obra original estilizada. Metros; eje Y arriba. Sin animaciones.\nmtllib {name}.mtl\n');offset=0;uvcount=0;normalcount=0
        for o in objects:
            o.data.calc_loop_triangles();world=o.matrix_world;normalmat=world.to_3x3().inverted().transposed()
            f.write('o '+o.name.replace(' ','_')+'\n')
            for v in o.data.vertices:
                p=(world@v.co-center)*scale;f.write(f'v {p.x:.5f} {p.z:.5f} {-p.y:.5f}\n')
            lastmat=None
            uvdata=o.data.uv_layers.active.data if o.data.uv_layers.active else None
            for tri in o.data.loop_triangles:
                material=o.data.materials[tri.material_index].name
                if material!=lastmat:f.write('usemtl '+material+'\n');lastmat=material
                refs=[]
                for k,idx in enumerate(tri.vertices):
                    uv=uvdata[tri.loops[k]].uv if uvdata else (o.data.vertices[idx].co.x,o.data.vertices[idx].co.z)
                    n=normalmat@(o.data.vertices[idx].normal if o.data.polygons[tri.polygon_index].use_smooth else tri.normal);n.normalize()
                    f.write(f'vt {uv[0]:.5f} {uv[1]:.5f}\nvn {n.x:.5f} {n.z:.5f} {-n.y:.5f}\n');uvcount+=1;normalcount+=1
                    refs.append(f'{offset+idx+1}/{uvcount}/{normalcount}')
                f.write('f '+' '.join(refs)+'\n')
            offset+=len(o.data.vertices)
    collection=bpy.data.collections.new(name);bpy.context.scene.collection.children.link(collection)
    for o in objects:
        for c in list(o.users_collection):c.objects.unlink(o)
        collection.objects.link(o)
    print('EXPORTADA',name,len(objects),'piezas')

groups=[]
def begin():return set(bpy.data.objects)
def end(before,name):
    bpy.context.view_layer.update();obs=[o for o in bpy.data.objects if o not in before and o.type=='MESH'];export_group(name,obs);groups.append(obs)

b=begin()
for i in range(3):figure((i-1)*.75,abs(i-1)*.13,h=1-.12*abs(i-1),pose=1)
end(b,'danza')
b=begin();figure(-.65,0,.92);figure(.65,.12,.78)
box((0,-.22,.45),(.85,.55,.10),wood)
for x in [-.34,.34]:cylinder((x,-.22,0),(x,-.22,.45),.035,wood)
for j in range(5):root(((j%3-.8)*.22,-.28+(j//3)*.17,.5),.23)
end(b,'mercado')
b=begin()
sphere((0,0,.63),(.78,.29,.34),red);sphere((.66,-.04,.85),(.31,.27,.30),red);sphere((.90,-.18,.78),(.22,.20,.14),white)
for x in [-.5,.48]:
    for y in [-.20,.20]:cylinder((x,y,.6),(x+.06,y,.08),.10,red);sphere((x+.1,y-.04,.08),(.18,.11,.09),white)
for y in [-.17,.17]:sphere((.65,y,1.07),(.10,.065,.12),red);sphere((.86,y-.06,.94),(.025,.025,.03),dark)
for i in range(5):cylinder((-.65-i*.10,0,.62+i*.07),(-.75-i*.1,0,.69+i*.07),.045,red)
for i in range(22):
    a=i*2.4;x=-.52+(i%7)*.17;y=.27*math.cos(a);z=.65+.26*math.sin(a)
    sphere((x,y,z),(.07,.03,.045),white)
leaf((.5,0,1.05),0,.32,.08);end(b,'jaguar')
b=begin()
cylinder((0,0,0),(0,0,.9),.36,husks[0],24,r2=.10)
for i in range(18):
    a=i*math.tau/18
    leaf((math.cos(a)*.5,math.sin(a)*.5,.03),a+math.pi,.86,.16,husks[i%5],lean=.37)
sphere((0,0,1.0),(.17,.14,.21),husks[3]);cylinder((0,0,.70),(0,0,.9),.18,husks[2],16,r2=.12)
for side in [-1,1]:cylinder((side*.15,0,.83),(side*.42,-.15,.62),.06,husks[3]);sphere((side*.42,-.15,.62),(.07,)*3,husks[4])
for i in range(9):leaf((0,0,1.15),i*math.tau/9,.27,.05,husks[i%5],lean=.22)
end(b,'totomoxtle')
b=begin();cylinder((0,0,0),(0,0,.38),.24,white,20,r2=.30)
for j in range(18):
    a=j*2.4;r=.20+(.12*(j%3));p=(math.cos(a)*r,math.sin(a)*r,.65+.09*(j%5))
    cylinder((0,0,.3),p,.017,green,8)
    for k in range(9):
        t=k*math.tau/9;tilt=math.radians(60)
        petal=sphere((p[0]+math.cos(t)*.10,p[1]+math.sin(t)*.1*math.cos(tilt),p[2]+math.sin(t)*.1*math.sin(tilt)),(.08,.045,.027),flowers[j%4]);petal.rotation_euler.x=tilt
    center=sphere(p,(.05,.05,.036),gold);center.rotation_euler.x=math.radians(60)
for i in range(7):leaf((0,0,.34),i*math.tau/7,.45,.08,lean=.4)
end(b,'flor')

# Disponer las obras en fila en el archivo fuente, conservando los OBJ centrados.
for i,obs in enumerate(groups):
    for o in obs:o.location.x+=(i-2)*4
bpy.context.scene['descripcion']='Cinco obras originales estilizadas para Noche de los Rabanos. Sin animaciones.'
bpy.context.scene['convencion_exportacion']='OBJ con Y arriba; altura 1.7 unidades; base y=0.'
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'Obras_NocheDeLosRabanos.blend'))
print('Fuente editable y cinco OBJ/MTL guardados en',OUT)

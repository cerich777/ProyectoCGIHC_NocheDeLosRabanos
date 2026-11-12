"""Texturas originales reproducibles; no descarga recursos externos."""
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont, ImageFilter
import math, random

ROOT = Path(__file__).resolve().parents[1] / 'NocheDeLosRabanos' / 'images' / 'Proyecto'
ROOT.mkdir(parents=True, exist_ok=True)
rng = random.Random(41947650)
N = 256
def texture(name, base, fn):
    im = Image.new('RGB', (N,N)); pixels=[]
    for y in range(N):
        for x in range(N):
            value=fn(x,y)+rng.uniform(-.035,.035)
            pixels.append(tuple(max(0,min(255,int(c*value))) for c in base))
    im.putdata(pixels); im.save(ROOT/name)
noise=Image.new('L',(32,32));noise.putdata([rng.randint(90,240) for _ in range(32*32)]);noise=noise.resize((N,N),Image.Resampling.BICUBIC).filter(ImageFilter.GaussianBlur(2))
texture('piedra.png',(180,161,137),lambda x,y: .77+noise.getpixel((x,y))*.001)
grain=[rng.uniform(.78,1.0) for _ in range(N)]
texture('madera.png',(174,121,73),lambda x,y: .85*grain[y]+.035*math.sin(y*.7+math.sin(x*.025)*.9))
texture('cobre.png',(88,141,126),lambda x,y: .85+.14*math.sin(x*.055)*math.cos(y*.075))
texture('hojas.png',(83,134,69),lambda x,y: .80+.13*math.sin(x*.14+y*.19)+.07*math.cos(x*.43))
texture('tela.png',(245,233,208),lambda x,y: .9+.04*(x%2)+.04*(y%2))
texture('muro.png',(232,213,179),lambda x,y: .93+.06*math.sin(x*.07)*math.cos(y*.08))
def paver(x,y):
    yy=y%64; xx=(x+(32 if (y//64)%2 else 0))%128
    if xx<3 or yy<3:return .53
    return .91+.06*math.sin((x//128)*9+(y//64)*7)
texture('adoquin.png',(180,169,151),paver)

def font(size,bold=False):
    return ImageFont.truetype('C:/Windows/Fonts/'+('georgiab.ttf' if bold else 'georgia.ttf'),size)
names=[('01  /  DANZA DE RÁBANOS','Categoría libre'),('02  /  VIDA EN EL MERCADO','Categoría tradicional'),('03  /  EL JAGUAR DE LA SIERRA','Categoría libre'),('04  /  MEMORIA DEL MAÍZ','Totomoxtle'),('05  /  JARDÍN DE OAXACA','Flor inmortal'),('06  /  TALLER DEL ARTESANO','Proceso y tradición'),('NOCHE DE LOS RÁBANOS','Oaxaca de Juárez  ·  23 de diciembre'),('EL KIOSCO DEL ZÓCALO','Un punto de encuentro')]
for i,(title,sub) in enumerate(names):
    im=Image.new('RGB',(1024,192),(71,27,43));d=ImageDraw.Draw(im)
    d.rectangle((7,7,1016,184),outline=(217,173,91),width=3)
    size=43
    while d.textlength(title,font=font(size,True))>950:size-=1
    d.text((512,68),title,fill=(255,241,214),font=font(size,True),anchor='mm')
    d.text((512,133),sub,fill=(232,195,132),font=font(31),anchor='mm')
    for xx in (32,992):d.polygon([(xx,75),(xx+9,91),(xx,107),(xx-9,91)],fill=(217,173,91))
    im.save(ROOT/f'cartel_{i}.png')
print('15 texturas y carteles originales creados en',ROOT)

import json
import sys

def gen_program(name="bunny_prog"):
  prog_def = {}
  base_name = name.split("_")[0]
  prog_def['name'] = "bunny_prog" 
  prog_def['vert'] = base_name + "_vert.glsl"
  prog_def['frag'] = base_name + "_frag.glsl"
  uniforms = []
  uniforms.append("P")
  uniforms.append("V")
  uniforms.append("MV")
  uniforms.append("collected")
  prog_def['uniforms'] = uniforms
  attributes = []
  attributes.append("vertPos")
  attributes.append("vertNor")
  prog_def['attributes'] = attributes
  print json.dumps(prog_def, sort_keys=True, indent=2)

if __name__ == "__main__":
  if sys.argv[1] == "-e" or sys.argv[1] == "--example":
    gen_program() 
  else:
    print "Doesn't generate yet"

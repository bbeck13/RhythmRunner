import json
import sys

def gen_texture(filename):
  tex_def = {}
  name = filename.split("/")[-1].split(".")[0]
  tex_def["name"] = name
  tex_def["filename"] = filename
  tex_def["unit"] = 0
  tex_def["wrap_mode_x"] = 10497 # GL_REPEAT
  tex_def["wrap_mode_y"] = 10497 # GL_REPEAT
  output_file = open(name + ".json", 'w')
  output_file.write(json.dumps(tex_def, sort_keys=True, indent=2))

if __name__ == "__main__":
  for item in sys.argv[1:]:
    gen_texture(item)

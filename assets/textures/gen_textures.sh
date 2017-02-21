#!/bin/bash
rm *.json
python ../../utils/TextureJSONGen.py *.jpg
python ../../utils/TextureJSONGen.py *.bmp
python ../../utils/TextureJSONGen.py *.png

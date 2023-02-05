import glob
import re
import json

dir_to_exclude = []

files = glob.glob('**/*.cc', recursive=True)
files_paths = [_ for _ in files if _.split("\\")[0] not in dir_to_exclude]
# files_names = [_.split("\\")[-1] for _ in files if _.split("\\")[0] not in dir_to_exclude]

# print(f'List of file names with path: {files_paths}')
# print(f'List of file names: {files_names}')

res = set()
for file in files_paths:
    with open(file, 'r') as file:
        data = file.read()
        res.update(re.findall(r'(?<=\[").+?(?="\])', data))


props = {}

for prop in res:
    props[prop] = {}


dictionary = {
    "$schema": "http://json-schema.org/draft-07/schema#",
    "title": "NESS input file schema",
    "description": " JSON Schema for NESS input files",
    "type": "object",
    "additionalProperties": False,
    "properties": props
}

# Serializing json
json_object = json.dumps(dictionary, indent=4)
 
# Writing to sample.json
with open("schema.json", "w+") as outfile:
    outfile.write(json_object)
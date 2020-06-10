import argparse
import sys
import os

argParser = argparse.ArgumentParser()
argParser.add_argument("--input_dir", type = str, help = "Directory with the multi-view texture folders")
argParser.add_argument("--output_file", type = str, help = "filepath to save the output video texture maps")

k_TSPPath = "D:\\_dev\\_Projects\\f360\\Resources\\TSP\\TextureStitching.exe"
k_UVAtlasPAth = "D:\\Tools\\UVAtlas-master\\Bin\\uvatlastool.exe"
k_MeshlabPath = "\"C:\\Program Files/VCG/Meshlab/meshlabserver.exe\""
k_MeshlabMlx = "D:\\_dev\\_Projects\\f360\\Resources\\Python\\Khazdan\\remove_dups.mlx"

k_texelName = "texels-%02d.png"
k_maskName = "mask-%02d.png"
k_meshName = "animated_template.ply"

def main(args):
	inputDir = args.input_dir
	output = args.output_file
	outputDir = os.path.dirname(output)
	tempOutputPath = os.path.join(outputDir, "temp")

	print("input: {}".format(inputDir))
	print("output: {}".format(outputDir))

	frameFolders = os.listdir(inputDir)
	frameFolders.sort(key=int)
	counter = 0
	for frameFolder in frameFolders:
		# get absolute paths
		absPath = os.path.join(inputDir, frameFolder)
		meshName = os.path.join(absPath, k_meshName)
		texelName = os.path.join(absPath, k_texelName)
		maskName = os.path.join(absPath, k_maskName)
		outName = "texture_{:03}.png".format(int(frameFolder))
		out = os.path.join(tempOutputPath, outName)
		# absKhazdan = os.path.abspath(k_TSPPath)
		cli = "{} --in {} {} {} --multi --out {}".format(k_TSPPath, meshName, texelName, maskName, out)
		print("Running: {}".format(cli))
		os.system(cli)		

if __name__ == "__main__":
	args = argParser.parse_args()
	main(args)
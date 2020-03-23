import argparse
import os
import cv2
import sys
import numpy as np
import imageio as im
import torch
import torchvision
from visdom import Visdom
import csv
from torchvision import transforms 
import math
from functions4inf import *
from loader_mod import IlluminationModule, Inference_Data
from autoenc_ldr2hdr import G    
from torch.utils.data import DataLoader

def deringing(coeffs, window):
    #convolve the coefficients with a low pass filter in spatial domain .
    deringed_coeffs = torch.zeros_like(coeffs)
    deringed_coeffs[:, 0] += coeffs[:, 0]
    deringed_coeffs[:, 1:1 + 3] += \
        coeffs[:, 1:1 + 3] * math.pow(math.sin(math.pi * 1.0 / window) / (math.pi * 1.0 / window), 4.0)
    deringed_coeffs[:, 4:4 + 5] += \
        coeffs[:, 4:4 + 5] * math.pow(math.sin(math.pi * 2.0 / window) / (math.pi * 2.0 / window), 4.0)
    return deringed_coeffs

def parse_arguments(args):
    usage_text = (
        "Lighting Regression Evaluation code for Laval Indoor HDR dataset [reference_TBA]"
        "Usage:  python3 laval_eval.py --test_batch_size 24 --epochs 1 --chkpnt_path .\ [options],"
        "   with [options]:"
    )
    parser = argparse.ArgumentParser(description=usage_text)
    #output path for predicted lighting env map
    parser.add_argument('--output', type=str, default='./output/', help='Low pass filter for SH')
    #durations
    #hardware
    parser.add_argument('-b',"--batch_size", type = int, help = "Train with a <batch_size> number of samples each train iteration.")
    parser.add_argument('-g','--gpu', type=str, default='0', help='The ids of the GPU(s) that will be utilized. (e.g. 0 or 0,1, or 0,2). Use -1 for CPU.')
    #other
    parser.add_argument("--chkpnt_path",default='./model.pth', type = str, help = "evaluation path")
    parser.add_argument('-d','--disp_iters', type=int, default=50, help='Log training progress (i.e. loss etc.) on console every <disp_iters> iterations.')
    parser.add_argument('-c','--checkpoint_iters', type=int, default=1000, help='Save checkpoint (i.e. weights & optimizer) every <checkpoint_iters> iterations.')
    parser.add_argument('-t','--test_iters', type=int, default=1000, help='Test model every <test_iters> iterations.')
    parser.add_argument('--save_iters', type=int, default=100, help='Maximum test iterations to perform each test run.')
    #paths
    parser.add_argument('--weight_init', type=str, default="xavier", help='Weight initialization method, or path to weights file (for fine-tuning or continuing training)')
    parser.add_argument('--model', default="resnet", type=str, help='Model selection argument.')    
    #optimization
    parser.add_argument('-o','--optimizer', type=str, default="adam", help='The optimizer that will be used during training.')
    parser.add_argument("--opt_state", type = str, help = "Path to stored optimizer state file (for continuing training)")
    parser.add_argument('-l','--lr', type=float, default=0.01, help='Optimization Learning Rate.')
    parser.add_argument('-m','--momentum', type=float, default=0.9, help='Optimization Momentum.')
    parser.add_argument('--momentum2', type=float, default=0.999, help='Optimization Second Momentum (optional, only used by some optimizers).')
    parser.add_argument('--epsilon', type=float, default=1e-8, help='Optimization Epsilon (optional, only used by some optimizers).')
    parser.add_argument('--weight_decay', type=float, default=0, help='Optimization Weight Decay.')    
    #hardware'-n','--name', type=str, default='default_name', help='The name of this train/test. Used when storing information.')    
    parser.add_argument('-n','--name', type=str, default='default_name', help='The name of this train/test. Used when storing information.')    
    parser.add_argument("--visdom", type=str, nargs='?', default=None, const='localhost', help = "Visdom server IP (port defaults to 8097)")
    parser.add_argument("--visdom_iters", type=int, default=100, help = "Iteration interval that results will be reported at the visdom server for visualization.")
    parser.add_argument("--seed", type=int, default=1337, help="Fixed manual seed, zero means no seeding.")
    #network specific params
    parser.add_argument("--ssim_window", type=int, default=7, help = "Kernel size to use in SSIM calculation.")
    parser.add_argument("--ssim_mode", type=str, default='gaussian', help = "Type of SSIM averaging (either gaussian or box).")
    parser.add_argument("--ssim_std", type=float, default=1.5, help = "SSIM standard deviation value used when creating the gaussian averaging kernels.")
    parser.add_argument("--ssim_alpha", type=float, default=0.5, help = "Alpha factor to weight the SSIM and L1 losses, where a x SSIM and (1 - a) x L1.")
    #details
    parser.add_argument("--width", type=float, default=512, help = "Spherical image width.")
    parser.add_argument("--sh_threshold", type=float, default=0.003, help = "Spherical image width.")
    parser.add_argument("--scale", type=float, default=0.01, help = "Scale factor for sh")
    parser.add_argument('--ldr2hdr_model', type=str, default='./ldr2hdr.pth', help='model file')
    parser.add_argument('--n_channel_input', type=int, default=3, help='number of input channels')
    parser.add_argument('--n_channel_output', type=int, default=3, help='number of output channels')
    parser.add_argument('--n_generator_filters', type=int, default=64, help='number of initial generator filters')
    parser.add_argument('--deringing', type=int, default=1, help='Low pass filter for SH')
    parser.add_argument('--input', type=str, default='', help='Low pass filter for SH')
    parser.add_argument('--spherical_weights', type=int, default=1, help='Low pass filter for SH')
    parser.add_argument('-e',"--epochs", type = int, help = "Train for a total number of <epochs> epochs.")
    return parser.parse_known_args(args)

def eval(module, args):
    gpus = [int(id) for id in args.gpu.split(' ') if int(id) >= 0]
    
    #Eval() autoencoder
    AutoEnc_model = torch.load(args.ldr2hdr_model)
    AutoEnc = G(args.n_channel_input * 1, args.n_channel_output, args.n_generator_filters)
    AutoEnc.load_state_dict(AutoEnc_model['state_dict_G'])
    AutoEnc = AutoEnc.cuda()
    #
    if (os.path.isdir(args.out_path)!=True):
        os.mkdir(args.out_path)
    #time
    start_time = time.time()
    inference_data = Inference_Data(args.input_path)
    _filename, _file_extention = os.path.splitext(args.input_path) 
    assert _file_extention in ['.png', '.jpg']

    dataloader = DataLoader(inference_data, batch_size=1, shuffle=False, num_workers=1)
    for i, data in enumerate(dataloader):
        input_img = data.cuda().float()
        with torch.no_grad():
            #prediction from HDR
            right_rgb = AutoEnc(input_img)
            p_coeffs = module(right_rgb).view(1,9,3).cuda().float()
            p_coeffs = deringing(p_coeffs, 6.0).cuda().float()
            p_coeffs_np = p_coeffs.cpu().detach().numpy().squeeze(0)
            p_diff_conv_np = shReconstructSignal(p_coeffs_np, width=args.width)
            "Save predicted lighting env map"
            out_path = args.out_path+os.path.basename(args.input_path)
            filename, file_extension = os.path.splitext(out_path)
            file_extension = '.exr'
            out_path = filename + file_extension
            cv2.imwrite(out_path, p_diff_conv_np)
    elapsed_time = time.time() - start_time
    print("Elapsed time: %2.4f" %elapsed_time)

def main(args):
    print("Loading Checkpoint . . . \n")
    module = IlluminationModule(args.batch_size).cuda()
    module.load_state_dict(torch.load(args.chkpnt_path))
    eval(module,args)
    
if __name__ == '__main__':
    args, unknown = parse_arguments(sys.argv)
    main(args)
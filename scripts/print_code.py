#!/usr/bin/python3

import os, sys

SRC_PATH = "../src/"

def print_comments(path):
    with open(path, "r") as file:
        for line in file:
            sys.stdout.write(line)

def main():
    for root, dirs, files in os.walk(SRC_PATH):
        for f in files:
            if (f.endswith('.h') or
                f.endswith('.c') or
                f.endswith('.asm') or
                f.endswith('.s')):

                rel_path = os.path.join(root, f)

                print("\n=========================================================")
                print(rel_path)
                print("===========================================================")
                print_comments(rel_path)

main()

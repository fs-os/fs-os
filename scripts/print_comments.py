#!/usr/bin/python3

import os, sys

SRC_PATH = "../src/"

def print_c_comments(path):
    had_slash    = False
    had_asterisk = False
    should_print = False

    with open(path, "r") as file:
        for line in file:
            for c in line:
                if c == '*':
                    had_asterisk = True

                    if had_slash:                   # /*
                        sys.stdout.write('/')       # first slash
                        should_print = True         # start of comment, print

                    had_slash = False
                elif c == '/':
                    had_slash = True

                    if had_asterisk:                # */
                        if should_print:
                            sys.stdout.write('/')       # last slash
                            sys.stdout.write('\n')      # newline (just visual)

                        should_print = False        # end of comment, stop printing

                    had_asterisk = False
                else:
                    had_slash = False
                    had_asterisk = False
                
                if (should_print):
                    sys.stdout.write(c)

def print_asm_comments(path):
    should_print = False

    with open(path, "r") as file:
        for line in file:
            for c in line:
                if c == ';':
                    should_print = True

                if (should_print):
                    sys.stdout.write(c)

                # Check afterwards to print the '\n'
                if c == '\n':
                    should_print = False

def main():
    # Iterate "*.c" and "*.h" files
    for root, dirs, files in os.walk(SRC_PATH):
        for f in files:
            if f.endswith('.h') or f.endswith('.c'):
                rel_path = os.path.join(root, f)

                print("\n=========================================================")
                print(rel_path)
                print("===========================================================")
                print_c_comments(rel_path)

    # Iterate "*.asm" and "*.s" files. Another loop to make sure asm files are at the
    # bottom.
    for root, dirs, files in os.walk(SRC_PATH):
        for f in files:
            if f.endswith('.asm') or f.endswith('.s'):
                rel_path = os.path.join(root, f)

                print("\n=========================================================")
                print(rel_path)
                print("===========================================================")
                print_asm_comments(rel_path)

main()


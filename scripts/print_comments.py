#!/usr/bin/python3

import os, sys

SRC_PATH = "../src/"

def print_comments(path):
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

def main():
    for root, dirs, files in os.walk(SRC_PATH):
        for f in files:
            if f.endswith('.h') or f.endswith('.c'):
                rel_path = os.path.join(root, f)

                print("\n=========================================================")
                print(rel_path)
                print("===========================================================")
                print_comments(rel_path)

main()


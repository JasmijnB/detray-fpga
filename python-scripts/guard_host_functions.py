#! /usr/bin/env python3

import os
import re
from collections import defaultdict
from util import bcolors, get_files, OUTPUT_DIR

DIRECTORY = "../vecmem"
HOST_NAME = "VECMEM_HOST"
VITIS_COMPILE_MACRO = "DETRAY_COMPILE_VITIS"


def find_host_only_functions(word: str, files: list[str]) -> dict[str, list[int]]:
    """
    Find a word in the specified files.
    """
    word_regex = f"\\b{word}\\b"
    line_indices = defaultdict(list)
    for file in files:
        if file.endswith("qualifiers.hpp"):
            continue

        if file.endswith("types.hpp"):
            continue

        with open(file, 'r') as f:
            # find all lines that contain the word
            lines = f.readlines()
            for index, line in enumerate(lines):
                match = re.search(word_regex, line)
                if match:
                    end_index = find_end_line(lines[index:])
                    if index > 0 and lines[index - 1].strip().endswith('>'):
                        start_index = index - 1
                        while start_index > 1 and not lines[start_index].strip().startswith('template'):
                            start_index -= 1
                    else:
                        start_index = index
                    line_indices[file].append((start_index, index + end_index))
                    
    return line_indices

def find_mem_resource_includes(files: list[str], line_indices=defaultdict(list)) -> dict[str, list[int]]:
    """
    Find all files that include mem_resource.hpp.
    """
    mem_resource_regex = r'#include\s*["<]vecmem/memory/memory_resource\.hpp[">]'
    
    for file in files:
        if file.endswith("qualifiers.hpp"):
            continue

        with open(file, 'r') as f:
            # find all lines that contain the word
            lines = f.readlines()
            for index, line in enumerate(lines):
                match = re.search(mem_resource_regex, line)
                if match:
                    line_indices[file].append((index, index))
                    
    return line_indices

def find_end_line(lines: list[str]) -> int:
    curly_stack = 0
    round_stack = 0
    function_def_started = False
    round_brace_found = False
    
    for index, line in enumerate(lines):
        if line.strip().startswith('//'):
            continue

        for char_i, char in enumerate(line):
            if char == ';' and round_stack == 0 and curly_stack == 0:
                return index

            if char == '(':
                round_brace_found = True
                round_stack += 1
            elif char == ')':
                round_stack -= 1

            if char == '{':
                if round_stack == 0 and round_brace_found:
                    function_def_started = True
                curly_stack += 1
            elif char == '}':
                curly_stack -= 1


        if curly_stack == 0 and function_def_started:
            return index

        if curly_stack < 0:
            raise Exception(f"Stack size is negative: {curly_stack} in file {lines}")

    raise Exception(f"No curly braces found in the given lines:\n {"".join(lines)}")


def check_results(indices: dict[str, list[int]], files: list[str]) -> bool:
    """
    Check if the indices are valid.
    """
    for file, positions in indices.items():
        if file not in files:
            print(f"{bcolors.FAIL}File {file} not found in the list of files.{bcolors.ENDC}")
            return False

        with open(file, 'r') as f:
            lines = f.readlines()

        for start, end in positions:
            if start < 0 or end >= len(lines):
                print(f"{bcolors.FAIL}Invalid indices {start}, {end} in file {file}.{bcolors.ENDC}")
                return False
        for start, end in positions:
            if start > end:
                print(f"{bcolors.FAIL}Start index {start} is greater than end index {end} in file {file}.{bcolors.ENDC}")
                return False
        
        for (_, end_i), (start_i, _) in zip(positions[:-1], positions[1:]):
            if end_i > start_i:
                print(f"{bcolors.FAIL}Overlapping indices {end_i}, {start_i} in file {file}.{bcolors.ENDC}")
                return False

    return True

def insert_at_indices(indices: dict, begin_insert: str, end_insert: str) -> dict[str, list[str]]:
    res = {}
    for file, positions in indices.items():
        insert_amount = 0
        print(f"File: {file}")
        with open(file, 'r') as f:
            lines = f.readlines()

        for start, end in positions:
            lines.insert(start + insert_amount, begin_insert)
            insert_amount += 1
            lines.insert(end + 1 + insert_amount, end_insert)
            insert_amount += 1

        res[file] = lines

    return res

def main():
    files = get_files(DIRECTORY)
    found_lines = find_host_only_functions(HOST_NAME, files)
#    found_lines = find_mem_resource_includes(files, found_lines)

    # sort the items in found_lines by the first index of the tuple
    found_lines = {k: sorted(v, key=lambda x: x[0]) for k, v in found_lines.items()}

    if not check_results(found_lines, files):
        print(f"{bcolors.FAIL}Invalid indices found.{bcolors.ENDC}")
        exit(1)

    showcase_files = insert_at_indices(found_lines, f"{bcolors.OKGREEN}START\n", f"END\n {bcolors.ENDC}")
    modified_files = insert_at_indices(found_lines, f"#ifndef {VITIS_COMPILE_MACRO}\n", f"#endif // {VITIS_COMPILE_MACRO}\n")
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    for file, lines in showcase_files.items():
        print(f"===[ {file} ]===")
        print("".join(lines))
#        res = input("Correct? (y/n): ")
        res = 'y'
        if res.lower() == 'y':
            mod_lines = modified_files[file]
            file_path = os.path.join(OUTPUT_DIR, file)
            os.makedirs(os.path.dirname(file_path), exist_ok=True)
            with open(file_path, 'w') as f:
                f.writelines(mod_lines)
        else:
            print(f"{bcolors.WARNING}Skipping file {file}.{bcolors.ENDC}")
            with open(os.path.join(OUTPUT_DIR, 'to_do.txt'), 'a+') as f:
                f.write(f"{file}\n")

        print("\n")

if __name__ == "__main__":
    main()

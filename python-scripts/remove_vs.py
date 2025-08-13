#! /usr/bin/env python3

from util import bcolors, get_files, OUTPUT_DIR
import os
import regex

INPUT_DIR = 'detray'

pattern = r'''
(?P<full>
    \b(?P<trait>\w+_v)       # capture name like is_same_v
    \s*<                     # open angle
        (?:                  # contents inside <>
            [^<>]+           # non-angle content
            |
            (?&angle)        # recurse into nested angle brackets
        )*
    >                        # close angle
)

(?(DEFINE)                   # define recursive group 'angle'
    (?P<angle>
        <                   # opening angle
            (?:[^<>]+|(?&angle))*  # nested or non-angle
        >
    )
)
'''

replacement_words = [
    "is_arithmetic_v",
    "is_scalar_v",
    "is_convertible_v",
    "is_same_v",
    "is_object_v",
    "conjunction_v",
]

exception_words = [
    "forward_iterator_v",
    "input_iterator_v",
    "random_access_iterator_v",
    "bidirectional_iterator_v",
    "output_iterator_v",
    "forward_range_v",
    "input_range_v",
    "random_access_range_v",
    "bidirectional_range_v",
    "output_range_v",
]

def replacer(match):
    full = match.group("full")
    trait = match.group("trait")
    if trait in exception_words or len(trait) < 3 or trait[-2:] != "_v":
        return full
#    if trait not in replacement_words:
#        print(f"{bcolors.WARNING}Trait {trait} not in replacement words.{bcolors.ENDC}")
#        return full  

    core = trait[:-2]
    return full.replace(trait, core, 1) + "::value "


def replace_words_in_file(file: str, words: list[str]):
    """
    Replace words in the specified file.
    """
    with open(file, 'r') as f:
        contents = ''.join(f.read())

    if regex.search(pattern, contents, flags=regex.VERBOSE) is None:
        return None

    return regex.sub(pattern, replacer, contents, flags=regex.VERBOSE)



def main():
    files = get_files(INPUT_DIR)
    for file in files:
        if not file.endswith('.hpp'):
            continue

        mod_lines = replace_words_in_file(file, replacement_words)
        if mod_lines is None:
            continue

        print(f"{bcolors.OKBLUE}Processing {file}{bcolors.ENDC}")
        file_path = os.path.join(OUTPUT_DIR, file)
        os.makedirs(os.path.dirname(file_path), exist_ok=True)
        with open(file_path, 'w') as f:
            f.write(mod_lines)

        replace_words_in_file(file, replacement_words)

if __name__ == "__main__":
    main()

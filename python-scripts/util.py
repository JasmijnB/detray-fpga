import os

OUTPUT_DIR = 'output'

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def get_files(directory) -> list[str]:
    """
    Get all files in the specified directory.
    """
    """Get all files in the specified directory."""
    def inner(directory, files):
        for file in os.listdir(directory):
            if file.endswith(".hpp"):
                file_path = os.path.join(directory, file)
                files.append(file_path)

            if os.path.isdir(os.path.join(directory, file)):
                inner(os.path.join(directory, file), files)
    
    files = []
    inner(directory, files)
    return files
                    
def find_matching_curly_braces(lines: list[str]) -> list[tuple[tuple[int, int], tuple[int, int]]]:
    """
    Find matching curly braces in the given lines.
    """
    stack_size = 0
    start_i = None
    end_i = None
    
    for index, line in enumerate(lines):
        if line.strip().startswith('//'):
            continue

        for char_i, char in enumerate(line):
            if char == '{':
                if stack_size == 0:
                    start_i = (index, char_i)
                stack_size += 1
            elif char == '}':
                stack_size -= 1
                if stack_size == 0 and start_i is not None:
                    end_i = (index, char_i)
                    return start_i, end_i

    raise Exception(f"No curly braces found in the given lines:\n {"".join(lines)}")

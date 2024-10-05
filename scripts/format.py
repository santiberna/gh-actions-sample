import argparse
import pathlib
import subprocess

# List of extensions to match
EXTENSIONS = {".cpp", ".hpp", ".h"}


def glob_recursive_files(path, extensions):
    ret = []
    path_obj = pathlib.Path(path)
    for file_path in path_obj.rglob("*"):
        if file_path.is_file() and file_path.suffix in extensions:
            ret.append(file_path)
    return ret


def call_clang_format(files):
    args = ["clang-format", "-i", "-style=file"]
    args += files

    return subprocess.call(args)


def main():

    parser = argparse.ArgumentParser(description='Packages the project to /package/')
    parser.add_argument('-d', '--directory', help="Directories to Format", type=str, nargs='*')
 
    args = parser.parse_args()
    files = []

    for dir in args.directory:
        files += glob_recursive_files(dir, EXTENSIONS)

    call_clang_format(files)


if __name__ == "__main__":
    main()

import sys
import subprocess


COMPILE_SUCCESS = "Project compilation succeded"
COMPILE_FAIL = "Project failed to compile"

CONFIGURE_SUCCESS = "Configure generated successfully"
CONFIGURE_FAIL = "Failed to configure build files"

def configure(config):
    return subprocess.run(["cmake", "--preset=" + config]).returncode

def clean(config):
    return subprocess.run(["cmake", "--build", "build/" + config, "--target clean"]).returncode

def build(config):
    return subprocess.run(["cmake", "--build", "build/" + config]).returncode

def check(code, success_message, fail_message):
    if code == 0:
        print(success_message)
        return
    else:
        print(fail_message)
        sys.exit(1)

def main():
    preset = sys.argv[1]

    if (preset == "Clean"):
        clean("Debug")
        clean("Release")
    else:
        check(configure(preset), CONFIGURE_SUCCESS, CONFIGURE_FAIL)
        check(build(preset), COMPILE_SUCCESS, COMPILE_FAIL)
    

if __name__ == "__main__":
    main()

    # cmake --build C:/foo/build/ --target clean
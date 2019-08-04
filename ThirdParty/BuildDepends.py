import sys
import os
import shutil
import platform
import atexit
import subprocess

# check os
os_name = platform.system().lower()
BUILD_OS = [v for k, v in {"win": "win", "darwin": "osx", "linux" : "linux"}.items() if k in os_name][0]
print "Build OS: ", BUILD_OS


def create_dir(dir_name):
    if not os.path.exists(dir_name):
        os.mkdir(dir_name)

def remove_dir(dir_name):
    if os.path.exists(dir_name):
        shutil.rmtree(dir_name)

def create_empty_dir(dir_name):
    remove_dir(dir_name)
    create_dir(dir_name)

# get real path
SOURCE_DIR = os.path.realpath("source")
BUILD_DIR = os.path.realpath("build")
INSTALL_DIR = os.path.realpath(".")
create_dir(BUILD_DIR)

def run_cmd(cmd):
    print '-- run -- ', cmd
    poll_code = None
    try:
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, bufsize=0)
        while True:
            output = p.stdout.readline()
            if output:
                print output,
            poll_code = p.poll()
            if poll_code is not None:
                break
    except Exception, e:
        p.terminate()
        print e
    finally:
        # flush, counldn't block here
        p.stdout.flush()
        output =  p.stdout.read()
        if output:
            print output,
    assert poll_code == 0, 'run cmd failed, code %s!'%poll_code
    

class Builder(object):
    def __init__(self, target=None):
        self.ninja = None

    def build_ninja(self):
        source_dir = os.path.realpath("ninja")
        build_dir = os.path.join(BUILD_DIR, "ninja")

        suffix = ".exe" if BUILD_OS == "win" else ""
        self.ninja = os.path.join(build_dir, "ninja%s"%(suffix, ))
        if os.path.exists(self.ninja):
            print '================== Ninja Alredy Build, Skipped... =================='
            return
        
        create_empty_dir(build_dir)

        cmd = "cd %s && python %s/configure.py --bootstrap"%(build_dir, source_dir)
        run_cmd(cmd)

    def clean(self):
        remove_dir(BUILD_DIR)

    def build_lib(self, lib_name,  cmake_args=None):
        source_dir = os.path.join(SOURCE_DIR, lib_name)
        if not os.path.exists(source_dir):
            source_dir = os.path.join(SOURCE_DIR, "..", lib_name)
        if not os.path.exists(source_dir):
            print 'error', lib_name, 'not exist'
            return

        build_dir = os.path.join(BUILD_DIR, lib_name)
        install_dir = os.path.realpath(".")

        create_empty_dir(build_dir)

        now_path = os.path.realpath(".")
        os.chdir(build_dir)
        
        print '================== BUILD %s BEGIN =================='%(lib_name.upper(), )
        final_cmake_args = []
        
        if cmake_args:
            final_cmake_args.extend(cmake_args)
        final_cmake_args.append("-DCMAKE_INSTALL_PREFIX=%s"%(install_dir,))
        final_cmake_args.append("-DCMAKE_MAKE_PROGRAM=%s"%self.ninja)
        
        cmake_arg_str = " ".join(final_cmake_args)
        cmd = " ".join(["cmake -G Ninja", cmake_arg_str, "%s"%(source_dir)])
        
        run_cmd(cmd)
        run_cmd(self.ninja)
        run_cmd(self.ninja + " install")
        print '================== BUILD %s END =================='%(lib_name.upper(), )

        os.chdir(now_path)

if __name__ == "__main__":
    builder = Builder()
    builder.build_ninja()
    builder.build_lib("HLSLcc", ["-DHLSLCC_LIBRARY_SHARED=OFF"])
   
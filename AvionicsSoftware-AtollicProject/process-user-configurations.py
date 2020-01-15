import yaml
import os
from pathlib import Path, PurePosixPath, WindowsPath
import sys


CC = '-gcc'
CXX = '-g++'
AS = '-as'
AR = '-ar'
OBJCOPY = '-objcopy'
OBJDUMP = '-objdump'
SIZE = '-size'
GDB = '-gdb'

C_COMPILERS = dict({CC: 'CMAKE_C_COMPILER', CXX: 'CMAKE_CXX_COMPILER'})

TOOLS = [AS, AR, OBJCOPY, OBJDUMP, SIZE, GDB]


def gen_aux_config_files():
    with open('user-configurations.yaml') as f:
        # use safe_load instead load
        dataMap = yaml.safe_load(f)

        if dataMap is None:
            raise Exception("Configuration file \"user-configurations.yaml\" is empty!")
        try:
            os.mkdir("generated")
        except OSError:
            print("Creation of the directory generated failed ")
        else:
            print("Successfully created the generated directory")

        cmake = open("generated/CMakeConfigFile.cmake", "w")
        batch = open("generated/CMDConfigFile.cmd", "w")
        ps = open("generated/PSConfigFile.ps1", 'w')
        bat_build = open("REBUILD.bat", 'w')
        ps_build = open("REBUILD.ps1", 'w')

        for key in dataMap:
            if sys.platform == 'win32':
                path = WindowsPath(dataMap.get(key))
            else:
                path = PurePosixPath(dataMap.get(key))

            if not os.path.isdir(str(path)):
                cmake.close()
                batch.close()
                ps.close()
                os.remove(cmake.name)
                os.remove(batch.name)
                os.remove(ps.name)
                raise Exception("Path does not exist or is not a directory: " + dataMap.get(key))

            cmake.write('SET(' + key + ' \"' + str(path).replace('\\', '/') + '\")')
            cmake.write('\n')
            batch.write('set ' + key + '=' + ' \"' + str(path) + '\"')
            batch.write('\n')
            ps.write('$' + key + '=' + ' \"' + str(path) + '\"')
            ps.write('\n')


        ARM_TOOLS_DIR = dataMap.get('ARM_TOOLS_DIR')
        for compiler_executable_name in os.listdir(ARM_TOOLS_DIR):
            if sys.platform == 'win32':
                if compiler_executable_name.endswith(CC + '.exe'):
                    cmake.write('SET(EABI_PREFIX \"' + compiler_executable_name[0:-8] + '\")')
                    cmake.write('\n')
                    batch.write('set EABI_PREFIX=' + ' \"' + compiler_executable_name[0:-8] + '\"')
                    batch.write('\n')
                    ps.write('$EABI_PREFIX=' + ' \"' + compiler_executable_name[0:-8] + '\"')
                    ps.write('\n')
                    break
            else:
                if compiler_executable_name.endswith(CC):
                    cmake.write('SET(EABI_PREFIX \"' + compiler_executable_name[0:-4] + '\")')
                    cmake.write('\n')
                    batch.write('set EABI_PREFIX=' + ' \"' + compiler_executable_name[0:-4] + '\"')
                    batch.write('\n')
                    ps.write('$EABI_PREFIX=' + ' \"' + compiler_executable_name[0:-4] + '\"')
                    ps.write('\n')
                    break

        if sys.platform != 'win32':
            return


        bat_build.write("@ECHO OFF")
        bat_build.write("\n")
        bat_build.write("rd /s /q \"build\" > nul 2>&1")
        bat_build.write("\n")
        bat_build.write("rd /s /q \"bin\" > nul 2>&1")
        bat_build.write("\n")
        bat_build.write("mkdir build")
        bat_build.write("\n")
        bat_build.write("cd build")
        bat_build.write("\n")
        bat_build.write(str(Path(dataMap.get("CLION_CMAKE_PATH"))) + "\cmake .. -G \"CodeBlocks - MinGW Makefiles\"")
        bat_build.write("\n")
        bat_build.write(str(Path(dataMap.get("MINGW_ENV_PATH"))) + "\mingw32-make -j8")
        bat_build.write("\n")
        bat_build.write("cd ..")


        ps_build.write("rm -r -fo build 2>&1 | out-null")
        ps_build.write("\n")
        ps_build.write("rm -r -fo bin 2>&1 | out-null")
        ps_build.write("\n")
        ps_build.write("mkdir build")
        ps_build.write("\n")
        ps_build.write("cd build")
        ps_build.write("\n")
        ps_build.write(str(Path(dataMap.get("CLION_CMAKE_PATH"))) + "\cmake .. -G \"CodeBlocks - MinGW Makefiles\"")
        ps_build.write("\n")
        ps_build.write(str(Path(dataMap.get("MINGW_ENV_PATH"))) + "\mingw32-make -j8")
        ps_build.write("\n")
        ps_build.write("cd ..")




if __name__ == '__main__':
    gen_aux_config_files()

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

        cmake = open("CMakeConfigFile.cmake", "w")
        batch = open("CMDConfigFile.cmd", "w")
        ps    = open("PSConfigFile.ps1", 'w')


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

            path = str(path).replace('\\','/')

            cmake.write('SET(' + key + ' \"' + str(path) + '\")')
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


if __name__ == '__main__':
    gen_aux_config_files()

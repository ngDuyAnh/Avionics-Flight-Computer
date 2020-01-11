import yaml
import os
from pathlib import Path

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
    cmake = open("CMakeConfigFile.cmake", "w")
    batch = open("CMDConfigFile.cmd", "w")
    ps    = open("PSConfigFile.ps1", 'w')
    with open('user-configurations.yaml') as f:
        # use safe_load instead load
        dataMap = yaml.safe_load(f)
        for key in dataMap:
            path = Path(dataMap.get(key))
            if not path.is_dir():
                raise Exception("Path does not exist or is not a directory: " + dataMap.get(key))

            cmake.write('SET(' + key + ' \"' + dataMap.get(key) + '\")')
            cmake.write('\n')
            batch.write('set ' + key + '=' + ' \"' + dataMap.get(key) + '\"')
            batch.write('\n')
            ps.write('$ ' + key + '=' + ' \"' + dataMap.get(key) + '\"')
            ps.write('\n')

    ARM_TOOLS_DIR = dataMap.get('ARM_TOOLS_DIR')
    for compiler_executable_name in os.listdir(ARM_TOOLS_DIR):
        if compiler_executable_name.endswith(CC):
            cmake.write('SET(EABI_PREFIX \"' + compiler_executable_name[0:-4] + '\")')
            break


if __name__ == '__main__':
    gen_aux_config_files()

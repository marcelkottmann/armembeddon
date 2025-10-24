import subprocess
from os.path import join
from os import getcwd

Import("env")

script_path = join(getcwd(), "scripts/codon_main_build.sh")
print(script_path)

try:
    subprocess.run([script_path], check=True, shell=True)
except subprocess.CalledProcessError as e:
    print(f"Error: {e}")
    env.Exit(1)

import subprocess
Import("env")

python_file = env.GetProjectOption("main_py", default="main.py")

# Remove .py extension for file base
file_base = python_file[:-3] if python_file.endswith(".py") else python_file

codon_obj = f"build/{file_base}.o"
codon_src = f"src/{file_base}.py"
script_path = "scripts/codon_main_build.sh"
project_dir = env["PROJECT_DIR"]

def before_linker(source, target, env):
  try:
      subprocess.run([f"{script_path} {project_dir} {file_base}"], check=True, shell=True)
  except subprocess.CalledProcessError as e:
      print(f"Error: {e}")
      env.Exit(1)


firmware_target = env.File(env.subst("$BUILD_DIR/${PROGNAME}.elf"))
codon_source = env.File("../"+codon_src)
codon_build_script = env.File("../scripts/codon_main_build.sh")

env.AddPreAction(firmware_target, before_linker)
env.Depends(firmware_target, [codon_source, codon_build_script])

env.Append(
  LINKFLAGS=[
      codon_obj,      
  ]
)



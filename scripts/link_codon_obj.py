import subprocess
Import("env")


file_base="func"
codon_obj = f"build/{file_base}.o"
codon_src = f"src/{file_base}.py"
script_path = "scripts/codon_main_build.sh"

def before_linker(source, target, env):
  try:
      subprocess.run([f"{script_path} {file_base}"], check=True, shell=True)
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



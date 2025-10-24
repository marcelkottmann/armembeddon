import subprocess
Import("env")


codon_obj = "build/codon_main.o"
codon_src = "src/codon_main.py"
script_path = "scripts/codon_main_build.sh"

def before_linker(source, target, env):
  try:
      subprocess.run([script_path], check=True, shell=True)
  except subprocess.CalledProcessError as e:
      print(f"Error: {e}")
      env.Exit(1)


firmware_target = env.File(env.subst("$BUILD_DIR/${PROGNAME}.elf"))
codon_source = env.File("../"+codon_src)

env.AddPreAction(firmware_target, before_linker)
env.Depends(firmware_target, [codon_source])

env.Append(
  LINKFLAGS=[
      codon_obj,      
  ]
)



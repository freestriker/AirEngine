glslangValidator.exe --target-env vulkan1.3 -t -V Present.vert -IUtils/ -o Spv/Present.vert.spv
glslangValidator.exe --target-env vulkan1.3 -t -V Present.frag -IUtils/ -o Spv/Present.frag.spv

glslangValidator.exe --target-env vulkan1.3 -t -V RtTest.rchit -IUtils/ -o Spv/RtTest.rchit.spv
glslangValidator.exe --target-env vulkan1.3 -t -V RtTest.rgen -IUtils/ -o Spv/RtTest.rgen.spv
glslangValidator.exe --target-env vulkan1.3 -t -V RtTest.rmiss -IUtils/ -o Spv/RtTest.rmiss.spv

cmd

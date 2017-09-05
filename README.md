# Efficient-Path-profiling
Implemented an LLVM Instrumentation Pass which inserts path profiling code to original program and a 
runtime library which counts path taken(for innermost loops) and produces profile output.

The "InstrumentPass" pass uses a "InstrumentPass" class,  is a subclass of LoopPass. The pass first finds the 
reverse tolopogical sort.After finding the reverse tolopogical sort , edge values and the NumPaths for each of the blocks in the loop are found.These values are calculated according to the Ball-Larus Instrumentation algorithm. The edge values are stored in an edgeValue
map and the numPaths map is used for storing the values of blocks.
The getOrInsertFunction method is used  for adding function declaration.Then CallInst::Create method is
used for inserting function calls inside the main c program.It adds init_path_reg(), inc_path_reg(), and finalize_path_reg()
 functions at appropriate locations inside main c code.The successors required in the pass are found by using succ_iterator which iterates over each of the successors using succ_begin and succ_end.All the basic blocks inside a loop are found by using block_iterator .
The epp_runtime.cpp file implements the runtime library.It has It has  two global maps i.e. count and r.The
count will store the number of times each loop and path gets executed .And the r map will be incremented as
an edge gets executed.It key is the loopId.The library implements the init_path_reg(), inc_path_reg(), finalize_path_reg() and dump_path_regs() functions.
The class is registered,using command line argument “epp”, and a name “Efficient Path Profiling
Instrument Pass."”.
To run the script ,  use    ./run.sh filename
Eq.)
 simpleloop.cc files can be run by
./run.sh simpleloop

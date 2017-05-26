#ifndef EE382V_ASSIGNMENT2_EPP_H
#define EE382V_ASSIGNMENT2_EPP_H

#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"

namespace ee382v
{

	class InstrumentPass: public llvm::LoopPass
	{
		private:
			// Private fields go here
			int global_loop_id = 0;
		public:
			static char ID;
			struct newBlock1
			{
				llvm::BasicBlock *b;
				int color; //white -0 , grey - 1 , black -2
				int startTime;
				int finishTime;
				struct newBlock1 *parent;
			};
			typedef struct newBlock1 newBlock;
			int time ;
			int pathID;
			std::vector<newBlock *> newCreatedBlocks;
			std::vector<newBlock *>::iterator it;
			std::map<llvm::BasicBlock *, int> numPath;
			std::map<std::pair<llvm::BasicBlock *, llvm::BasicBlock *>, int> edgeValue;
			InstrumentPass() : llvm::LoopPass(ID) { };
			llvm::Function* printf_prototype(llvm::LLVMContext& ctx, llvm::Module *mod);
			std::vector<std::vector<llvm::BasicBlock *>> totalPaths;
			void printAllPaths(llvm::Loop *loop);
			void  printPaths(llvm::BasicBlock *b, llvm::Loop *loop);
			void  printRecursive(llvm::BasicBlock *b, std::vector<llvm::BasicBlock *> paths , llvm::Loop *loop);
			void  storeResult( std::vector<llvm::BasicBlock *> paths);
			void getInnerMostLoop(llvm::Loop* loop);
			void assignEdges(llvm::Loop* loop);
			bool runOnLoop(llvm::Loop*, llvm::LPPassManager&);
			void DFS_visit(llvm::Loop *loop , newBlock *b1);
			void createAddNewBlock(llvm::BasicBlock *b1, int color1, InstrumentPass::newBlock *b2);
			void AddNewBlockPrint(llvm::BasicBlock *b1, int color1);
			void findReverseTopologicalSort(llvm::Loop *loop);
			void depthFirstSearch(llvm::Loop *loop);
			void getAnalysisUsage(llvm::AnalysisUsage &AU) const;
	};

}

#endif

//

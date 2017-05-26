#define DEBUG_TYPE "InstrumentPass"

#include "InstrumentPass.h"

#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include <vector>
#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include "epp_runtime.h"



using namespace ee382v;
using namespace llvm;




void InstrumentPass::DFS_visit(llvm::Loop *loop , InstrumentPass::newBlock *b1)//Performing depth first search
{
	InstrumentPass::newBlock *k1;
	time=time+1;
	b1->startTime =time;
	b1->color = 1;
	for(succ_iterator s = succ_begin(b1->b), e = succ_end(b1->b);s!=e;++s){
		for(std::vector<newBlock *>::iterator k = newCreatedBlocks.begin() , e1 = newCreatedBlocks.end() ; k!=e1 ; ++k)
		{
			if((*s)==((*k)->b))
			{
				k1 =*k;
				break;
			}

		}
		if(k1->color==0)
		{
			k1->parent = b1;
			DFS_visit(loop, k1);
		}
	}
	b1->color = 2;
	time=time+1;
	b1->finishTime=time;

}



void InstrumentPass::createAddNewBlock(llvm::BasicBlock *b1, int color1, InstrumentPass::newBlock *b2)
{
	newBlock *a =new  newBlock;
	a->b= b1;
	a->color = color1;
	a->parent = b2;
	a->startTime=0;
	a->finishTime=0;
	newCreatedBlocks.push_back(a);
}




void InstrumentPass::depthFirstSearch(llvm::Loop *loop)
{
	for(llvm::Loop::block_iterator i=loop->block_begin() , e = loop->block_end() ; i!=e ; ++i)
	{
		createAddNewBlock(*i, 0, NULL);
	}
	time =0;
	for(std::vector<newBlock *>::iterator j=newCreatedBlocks.begin() , e =newCreatedBlocks.end() ; j!=e ; ++j)
	{
		if((*j)->color==0)
		{
			DFS_visit(loop , *j);
		}
	}

}




bool compareByLength(const InstrumentPass::newBlock *a, const InstrumentPass::newBlock *b)
{
	return a->finishTime < b->finishTime;
}



void InstrumentPass::findReverseTopologicalSort(llvm::Loop *loop)  //finding reverse topological sort
{
	depthFirstSearch(loop);
	std::sort(newCreatedBlocks.begin(), newCreatedBlocks.end(), compareByLength);

}


void  InstrumentPass::assignEdges(Loop *loop)   //Assigning values to edges
{
	for(std::vector<newBlock *>::iterator i= newCreatedBlocks.begin() , e = newCreatedBlocks.end() ; i!=e ; ++i)
	{
		if(loop->getLoopLatch() == (*i)->b)
		{
			numPath[(*i)->b]=1;
		} 

		else
		{ 
			numPath[(*i)->b]=0;
			std::pair<BasicBlock *, BasicBlock *>  a;
			std::vector<BasicBlock *> Successors;
			Successors.clear();
			for(succ_iterator s = succ_begin((*i)->b), e1 = succ_end((*i)->b);s!=e1;++s)
			{
				Successors.push_back(*s); 
			}
			for(std::vector<BasicBlock *>::reverse_iterator l =  Successors.rbegin() , m= Successors.rend() ; l!=m ;++l)

			{
				if(!(loop->contains(*l)))
				{
					numPath[(*l)]=1;
				}
			}
			for(std::vector<BasicBlock *>::reverse_iterator l =  Successors.rbegin() , m= Successors.rend() ; l!=m ;++l)
			{
				a = std::make_pair((*i)->b, *l);
				edgeValue[a]=numPath[(*i)->b];

				numPath[(*i)->b] = numPath[(*i)->b] + numPath[*l];
			}
		}


	}
	/*	for(std::map<BasicBlock * , int>::iterator i= numPath.begin() , e= numPath.end() ; i!=e ; ++i)
		{
		errs() <<i->first->getName() << "\n";
		errs()  << i->second << "\n";
		}
		for(std::map<std::pair<BasicBlock * , BasicBlock *>,int>::iterator i= edgeValue.begin() , e = edgeValue.end() ; i!=e ; ++i)
		{
		errs() << i->first.first->getName() << "  ";
		errs() << i->first.second->getName() << "  ";
		errs() << i->second << "\n";
		}*/
}

void  InstrumentPass::printPaths(BasicBlock *b, Loop *loop)
{
	std::vector<BasicBlock *> paths;
	printRecursive(b , paths,loop);
}


void  InstrumentPass::printRecursive(BasicBlock *b, std::vector<BasicBlock *> paths , Loop *loop)
{
	paths.push_back(b);


	if((!(loop->contains(b))))
	{
		storeResult(paths);

	}
	else
	{
		for(succ_iterator s = succ_begin(b), e = succ_end(b);s!=e;++s){
			if((*s==loop->getHeader()))

			{
				storeResult(paths);
			}
			else
			{
				printRecursive(*s,paths, loop);
			}
		}
	}
}


void  InstrumentPass::storeResult( std::vector<BasicBlock *> paths)
{
	totalPaths.push_back(paths);
}


void InstrumentPass::printAllPaths(Loop *loop)  //Code to print all the paths
{
	std::pair<BasicBlock *, BasicBlock *>  a;
	std::map<std::pair<llvm::BasicBlock *, llvm::BasicBlock *>, int>::iterator  it2;
	BasicBlock *pi = NULL;
	for(std::vector<std::vector<BasicBlock *>>::reverse_iterator i =totalPaths.rbegin() , e= totalPaths.rend() ; i!=e ; ++i)
	{
		outs()<< global_loop_id  << "       " ;

		for(std::vector<BasicBlock *>::iterator i1= (*i).begin() , e1= (*i).end() ; i1!=e1 ; ++i1)
		{
			if(pi!=NULL)
			{
				a=  std::make_pair(pi, *i1);
				it2 = edgeValue.find(a);
				pathID += it2->second ;
			}
			pi=*i1;
		}
		outs() << pathID <<"           ";
		pathID=0;
		for(std::vector<BasicBlock *>::iterator i1= (*i).begin() , e1= (*i).end() ; i1!=e1 ; ++i1)
		{
			if(loop->contains(*i1))
				outs() << (*i1)->getName() << "->" ;
		}
		outs() << "Exit" << "\n";
		pi=NULL;


	}
}



void InstrumentPass::getInnerMostLoop(llvm::Loop* loop) //getting INNERMOST loop
{
	if(!(loop->empty()))
		return;
	else
	{
		global_loop_id+=1;
		findReverseTopologicalSort(loop);
		assignEdges(loop);
		printPaths(loop->getHeader() , loop);
		printAllPaths(loop);

	}
}




bool InstrumentPass::runOnLoop(llvm::Loop* loop, llvm::LPPassManager& lpm)
{
	LoopInfo& loopInfo = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
	newCreatedBlocks.clear();
	numPath.clear();
	edgeValue.clear();
	totalPaths.clear();
	pathID=0;
	getInnerMostLoop(loop);
if((loop->empty())){

	LLVMContext& Ctx = loop->getHeader()->getParent()->getContext();

	Constant* func = loop->getHeader()->getParent()->getParent()->getOrInsertFunction("init_path_reg", Type::getVoidTy(Ctx), Type::getInt32Ty(Ctx), NULL);
	Function *newFunc = dyn_cast<Function>(func);
	Constant* func1 = loop->getHeader()->getParent()->getParent()->getOrInsertFunction("inc_path_reg", Type::getVoidTy(Ctx), Type::getInt32Ty(Ctx),Type::getInt32Ty(Ctx), NULL);
	Function *newFunc1 = dyn_cast<Function>(func1);
	Constant* func2 = loop->getHeader()->getParent()->getParent()->getOrInsertFunction("finalize_path_reg", Type::getVoidTy(Ctx), Type::getInt32Ty(Ctx), NULL);
	Function *newFunc2 = dyn_cast<Function>(func2);


	APInt LoopId(32,global_loop_id);
	Value *init_arg_values2[] = {Constant::getIntegerValue(Type::getInt32Ty(Ctx),LoopId)};

	SmallVector< BasicBlock* , 8 > ExitBlocks;
	loop->getExitBlocks(ExitBlocks);
	Instruction *newInst = CallInst::Create(newFunc2,init_arg_values2, "");
	newInst->insertBefore((loop->getLoopLatch())->getFirstNonPHI());

	for(SmallVectorImpl<BasicBlock *>::iterator i=ExitBlocks.begin() , e= ExitBlocks.end() ; i!=e ;++i)
	{
		newInst = CallInst::Create(newFunc2,init_arg_values2, "");
		newInst->insertBefore((*i)->getFirstNonPHI());
	}
	int t=0;
	for( std::map<std::pair<llvm::BasicBlock *, llvm::BasicBlock *>,int>::iterator i = edgeValue.begin(),e= edgeValue.end() ; i!=e; ++i)
	{
		APInt val(32,i->second);
		Value *init_arg_values[] = {Constant::getIntegerValue(Type::getInt32Ty(Ctx),LoopId),Constant::getIntegerValue(Type::getInt32Ty(Ctx),val)};


		for(succ_iterator s = succ_begin(i->first.first), e1 = succ_end(i->first.first);s!=e1;++s)
		{
			t+=1;
		}
		if(t==1)
		{
			Instruction *newInst = CallInst::Create(newFunc1,init_arg_values, "");
			newInst->insertBefore(((i->first.first)->getTerminator()));

		}

		else
		{
			Instruction *newInst = CallInst::Create(newFunc1,init_arg_values, "");
			newInst->insertBefore(((i->first.second)->getFirstNonPHI()));

		}
		t=0;	
	}
	for(succ_iterator s = succ_begin(loop->getHeader()), e = succ_end(loop->getHeader());s!=e;++s)
	{
		Instruction *newInst = CallInst::Create(newFunc,init_arg_values2, "");
		newInst->insertBefore(((*s)->getFirstNonPHI()));

	}
}
	return false;
}
void InstrumentPass::getAnalysisUsage(AnalysisUsage &AU) const
{
	AU.addRequired<LoopInfoWrapperPass>();
	AU.addRequired<DominatorTreeWrapperPass>();
	AU.addRequiredTransitive<DominatorTreeWrapperPass>();
	AU.setPreservesCFG();
}

char InstrumentPass::ID = 0;
static RegisterPass<InstrumentPass> X("epp", "Efficient Path Profiling Instrument Pass.");




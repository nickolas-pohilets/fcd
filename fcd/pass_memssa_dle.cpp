//
// pass_memssa_dle.cpp
// Copyright (C) 2015 Félix Cloutier.
// All Rights Reserved.
//
// This file is distributed under the University of Illinois Open Source
// license. See LICENSE.md for details.
//

#include <gflags/gflags.h>
#include <glog/logging.h>

#include "remill/BC/Version.h"

#include "fcd/passes.h"
#include "fcd/compat/MemorySSA.h"

#include <llvm/ADT/PostOrderIterator.h>
#include <llvm/IR/PatternMatch.h>

using namespace llvm;
using namespace std;

namespace
{
	struct MemorySSADLE final : public FunctionPass
	{
		static char ID;
		
		MemorySSADLE() : FunctionPass(ID)
		{
		}

#if LLVM_VERSION_NUMBER >= LLVM_VERSION(3, 9)
		virtual void getAnalysisUsage(AnalysisUsage& au) const override
		{
			au.addRequired<AAResultsWrapperPass>();
			au.addRequired<DominatorTreeWrapperPass>();
			au.setPreservesAll();
		}
		
		bool runOnBasicBlock(MemorySSA& mssa, BasicBlock& bb)
		{
			bool changed = false;
			SmallVector<LoadInst*, 10> deletedLoads;
			if (auto accessList = mssa.getBlockAccesses(&bb))
			{
				for (const MemoryAccess& access : *accessList)
				{
					if (auto useOrDef = dyn_cast<MemoryUseOrDef>(&access))
					{
						if (auto use = dyn_cast<MemoryUse>(&access))
						if (auto load = dyn_cast<LoadInst>(use->getMemoryInst()))
						{
							auto parent = useOrDef->getDefiningAccess();
							if (auto def = dyn_cast<MemoryDef>(parent))
							if (auto store = dyn_cast_or_null<StoreInst>(def->getMemoryInst()))
							{
								auto storedValue = store->getValueOperand();
								// sanity test
								if (storedValue->getType() == load->getType())
								{
									load->replaceAllUsesWith(storedValue);
									deletedLoads.push_back(load);
									changed = true;
								}
							}
						}
					}
					else
					{
						break;
					}
				}
			}
			
			for (LoadInst* deletedLoad : deletedLoads)
			{

				auto access = mssa.getMemoryAccess(deletedLoad);
				assert(access != nullptr);
				deletedLoad->eraseFromParent();
#if LLVM_VERSION_NUMBER >= LLVM_VERSION(5, 0)
				MemorySSAUpdater updater(&mssa);
				updater.removeMemoryAccess(access);
#else
				mssa.removeMemoryAccess(access);
#endif
			}
			
			return changed;
		}
		
		virtual bool runOnFunction(Function& f) override
		{
			auto& aaResults = getAnalysis<AAResultsWrapperPass>().getAAResults();
			auto& domTree = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
			MemorySSA mssa(f, &aaResults, &domTree);
			
			bool changed = false;
			for (BasicBlock* bb : ReversePostOrderTraversal<BasicBlock*>(&f.getEntryBlock()))
			{
				changed |= runOnBasicBlock(mssa, *bb);
			}
			return changed;
		}
#else
		virtual bool runOnFunction(Function& f) override
		{
			CHECK(false) << "LLVM 3.9 required for MemorySSA based optimizations";
		}
#endif
	};
	
	char MemorySSADLE::ID = 0;
	
	RegisterPass<MemorySSADLE> memSsaDle("memssadle", "MemorySSA-based dead load elimination");
}

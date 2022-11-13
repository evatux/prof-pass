#include <iostream>

#include "llvm/Pass.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

struct ProfPass : public ModulePass {
    static char ID;
    ProfPass() : ModulePass(ID) {}

    virtual bool runOnModule(Module &M) override {
        auto &CTX = M.getContext();

        PointerType *ProfArgTy = PointerType::getUnqual(Type::getInt8Ty(CTX));
        FunctionType *ProfTy = FunctionType::get(IntegerType::getInt32Ty(CTX), ProfArgTy, /*IsVarArgs=*/true);
        FunctionCallee Prof = M.getOrInsertFunction("prof_register_foo", ProfTy);

        // Set attributes as per inferLibFuncAttributes in BuildLibCalls.cpp
        Function *ProfF = dyn_cast<Function>(Prof.getCallee());
        ProfF->setDoesNotThrow();
        ProfF->addParamAttr(0, Attribute::NoCapture);
        ProfF->addParamAttr(0, Attribute::ReadOnly);

        FunctionCallee ProfUnregister = M.getOrInsertFunction("prof_unregister_foo", ProfTy);

        for (const auto &F : M) {
            if (F.isDeclaration()) continue;

            errs() << "@@@ I saw a function called " << F.getName() << "!\n";

            IRBuilder<> Builder(const_cast<llvm::Instruction *>(&*F.getEntryBlock().getFirstInsertionPt()));
            auto FuncName = Builder.CreateGlobalStringPtr(F.getName());
            Builder.CreateCall(Prof, {FuncName});

            for (const auto &B : F.getBasicBlockList()) {
                auto t = B.getTerminator();
                if (dyn_cast<ReturnInst>(t)) {
                    auto before_return_inst = t->getPrevNode();
                    errs() << "@@@ I saw a return after " << before_return_inst->getOpcodeName() << "!\n";
                    IRBuilder<> BuilderUnregister(const_cast<llvm::Instruction *>(t));
                    BuilderUnregister.CreateCall(ProfUnregister, {FuncName});
                }
            }
        }
        return false;
    }
};

char ProfPass::ID = 0;

static RegisterStandardPasses RegisterMyPass(
        //PassManagerBuilder::EP_EarlyAsPossible,
        PassManagerBuilder::EP_ModuleOptimizerEarly,
        [](const PassManagerBuilder &, legacy::PassManagerBase &PM) { PM.add(new ProfPass()); });

static RegisterStandardPasses
    RegisterMyPass0(PassManagerBuilder::EP_EnabledOnOptLevel0,
        [](const PassManagerBuilder &, legacy::PassManagerBase &PM) { PM.add(new ProfPass()); });

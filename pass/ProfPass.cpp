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

        llvm::Constant *ProfFormatStr = llvm::ConstantDataArray::getString(
                CTX, "(llvm-tutor)\n");
        Constant *ProfFormatStrVar =
            M.getOrInsertGlobal("ProfFormatStr", ProfFormatStr->getType());
        dyn_cast<GlobalVariable>(ProfFormatStrVar)->setInitializer(ProfFormatStr);

        for (const auto &F : M) {
            if (F.isDeclaration()) continue;

            errs() << "@@@ I saw a function called " << F.getName() << "!\n";

            IRBuilder<> Builder(const_cast<llvm::Instruction *>(&*F.getEntryBlock().getFirstInsertionPt()));
            auto FuncName = Builder.CreateGlobalStringPtr(F.getName());
            llvm::Value *FormatStrPtr = Builder.CreatePointerCast(ProfFormatStrVar, ProfArgTy, "formatStr");
            Builder.CreateCall(Prof, {FuncName});
        }
        return false;
    }
};

char ProfPass::ID = 0;

static RegisterStandardPasses RegisterMyPass(
        //PassManagerBuilder::EP_EarlyAsPossible,
        PassManagerBuilder::EP_ModuleOptimizerEarly,
        [](const PassManagerBuilder &, legacy::PassManagerBase &PM) { PM.add(new ProfPass()); });

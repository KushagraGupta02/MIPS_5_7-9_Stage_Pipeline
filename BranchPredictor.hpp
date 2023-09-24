#ifndef __BRANCH_PREDICTOR_HPP__
#define __BRANCH_PREDICTOR_HPP__

#include <vector>
#include <bitset>
#include <cassert>

struct BranchPredictor {
    virtual bool predict(uint32_t pc) = 0;
    virtual void update(uint32_t pc, bool taken) = 0;
};

struct SaturatingBranchPredictor : public BranchPredictor {
    std::vector<std::bitset<2>> table;
    SaturatingBranchPredictor(int value) : table(1 << 14, value) {}

    bool predict(uint32_t pc) {
        return (table[pc % table.size()][1]);
    }

    void update(uint32_t pc, bool taken) {
        std::bitset<2> &counter = table[pc % table.size()];
        if (taken){
            if (counter == 0){counter = 1;}
            else if (counter == 1){counter = 2;}
            else if (counter == 2){counter = 3;}
            else {}
        }
        else{
            if (counter == 1){counter = 0;}
            else if (counter == 2){counter = 1;}
            else if (counter == 3){counter = 2;}
            else {}
        }
    }
};

struct BHRBranchPredictor : public BranchPredictor {
    std::vector<std::bitset<2>> bhrTable;
    std::bitset<2> bhr;
    BHRBranchPredictor(int value) : bhrTable(1 << 2, value), bhr(value) {}

    bool predict(uint32_t pc) {
        return (bhrTable[bhr.to_ulong()][1]);
    }

    void update(uint32_t pc, bool taken) {
        int x = bhr.to_ulong();
        if (taken){
            if (bhr==0 or bhr==2){bhr=1;}
            else {bhr=3;}

            if (bhrTable[x]==0) {bhrTable[x]=1;}
            else if (bhrTable[x]==1) {bhrTable[x]=2;}
            else if (bhrTable[x]==2) {bhrTable[x]=3;}
            else {}
        }
        else {
            if (bhr==0 or bhr==2){bhr=0;}
            else {bhr=2;}

            if (bhrTable[x]==3) {bhrTable[x]=2;}
            else if (bhrTable[x]==2) {bhrTable[x]=1;}
            else if (bhrTable[x]==1) {bhrTable[x]=0;}
            else {}
        }
    }
};

struct SaturatingBHRBranchPredictor : public BranchPredictor {
    std::vector<std::bitset<2>> bhrTable;
    std::bitset<2> bhr;
    std::vector<std::bitset<2>> table;
    std::vector<std::bitset<2>> combination;
    SaturatingBHRBranchPredictor(int value, int size) : bhrTable(1 << 2, value), bhr(value), table(1 << 14, value), combination(size, value) {
        assert(size <= (1 << 16));
    }

    bool predict(uint32_t pc) {
        int x = (pc%table.size()*4 + table[pc%table.size()].to_ulong())%combination.size();
        return combination[x].to_ulong() > 1; 
    }

    void update(uint32_t pc, bool taken) {
        int x = (pc%table.size()*4 + table[pc%table.size()].to_ulong())%combination.size();
        if (taken) {
            if (table[pc%table.size()] == 0 or table[pc%table.size()] ==2){table[pc%table.size()] = 1;}
            else {table[pc%table.size()] = 3;}
            
            if (combination[x]==0) {combination[x]=1;}
            else if (combination[x]==1) {combination[x]=2;}
            else if (combination[x]==2) {combination[x]=3;}
            else {}
        }
        else {
            if (table[pc%table.size()] == 0 or table[pc%table.size()] ==2){table[pc%table.size()] = 0;}
            else {table[pc%table.size()] = 2;}

            if (combination[x]==3) {combination[x]=2;}
            else if (combination[x]==2) {combination[x]=1;}
            else if (combination[x]==1) {combination[x]=0;}
            else {}
        }
    }
};
#endif
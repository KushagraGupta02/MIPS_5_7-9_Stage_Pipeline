/**
 * @file MIPS_Processor.hpp
 * @author Mallika Prabhakar and Sayam Sethi
 * 
 */

#ifndef __MIPS_PROCESSOR_HPP__
#define __MIPS_PROCESSOR_HPP__

#include <unordered_map>
#include <string>
#include <climits>
#include <functional>
#include <vector>
#include <fstream>
#include <exception>
#include <iostream>
#include <boost/tokenizer.hpp>

struct MIPS_Architecture
{		
	struct IF1
	{   int opcode= -1;
        int opcodeAux = -1;
        int instr = 0;
		bool idle = 0;
	};
    struct IF2
	{   int opcode= -1;
        int opcodeAux = -1;
        int branch = 0;
		bool idle = 0;
        int instr = 0;
	};
	struct Dec1
	{   int branch = 0;
        int opcode= -1;
        int opcodeAux =-1;
        int read_reg1;
		int read_reg2;
		int write_reg;
		int write_data;
		int read_data1;
		int read_data2;
		bool idle = 1;
	};
    struct Dec2
	{   int branch = 0;
        int opcode= -1;
        int opcodeAux =-1;
        int read_reg1;
		int read_reg2;
		int write_reg;
		int write_data;
		int read_data1;
		int read_data2;
		bool idle = 1;
	};
    struct RR
	{   int branch = 0;
        int opcode= -1;
        int opcodeAux =-1;
        int read_reg1;
		int read_reg2;
		int write_reg;
		int write_data;
		int read_data1;
		int read_data2;
		bool idle = 1;
	};
	struct EX
	{   int branch = 0;
        int opcode= -1;
        int opcodeAux =-1;
        int write_data;
        int data1;
		int data2;
		int result;
        int read_reg1;
        int read_reg2;
		int write_reg;  
		bool idle = 1;
        int lock=0;
	};

	struct MEM1
	{	int opcode= -1;
		int address;
		int write_data_mem;
		int write_reg;
		int read_data;    // read from memory
		int forward_data; // read from alu
		bool idle = 1;
        bool temp=0;
        int temp1 = -1;
        int temp2 = -1;
	};
    struct MEM2
	{	int opcode= -1;
		int address;
		int write_data_mem;
		int write_reg;
		int read_data;    // read from memory
		int forward_data; // read from alu
		bool idle = 1;
        bool temp=0;
        int temp1 = -1;
        int temp2 = -1;
	};
	struct WB
	{   int opcode= -1;
		bool idle = 1;
		int read_data;
		int write_reg;

	};

	
	// struct WB
	// {
	// 	bool idle = 1;
	// 	int result;
	// 	int write_reg = -1;

	// };


	int registers[32] = {0}, PCcurr = 0, PCnext;
	
	int acquired[32] = {0};
	std::unordered_map<std::string, std::function<int(MIPS_Architecture &, std::string, std::string, std::string)>> instructions;
	std::unordered_map<std::string, int> registerMap, address;
    std::unordered_map<std::string, int> opcodeDict;
    std::unordered_map<std::string, int> opcodeDictAux;
    std::vector<int> aaa{-1,-1};
    std::vector<int> bbb{-1,-1};
    std::vector<int> ccc{-1,-1};
    std::vector<std::vector<int>> Latches{aaa,bbb,ccc};
    // Latches.push_back(aaa);
    // Latches.push_back(bbb);
	static const int MAX = (1 << 20);
	int data[MAX >> 2] = {0};
    
	std::vector<std::vector<std::string>> commands;
	std::vector<int> commandCount;
	enum exit_code
	{
		SUCCESS = 0,
		INVALID_REGISTER,
		INVALID_LABEL,
		INVALID_ADDRESS,
		SYNTAX_ERROR,
		MEMORY_ERROR
	};

	// constructor to initialise the instruction set
	MIPS_Architecture(std::ifstream &file)
	{
		instructions = {{"add", &MIPS_Architecture::add}, {"sub", &MIPS_Architecture::sub}, {"mul", &MIPS_Architecture::mul}, {"beq", &MIPS_Architecture::beq}, {"bne", &MIPS_Architecture::bne}, {"slt", &MIPS_Architecture::slt}, {"j", &MIPS_Architecture::j}, {"lw", &MIPS_Architecture::lw}, {"sw", &MIPS_Architecture::sw}, {"addi", &MIPS_Architecture::addi}};
		registers[1]=0;
		registers[2]=0;
		registers[3]=0;
		registers[4]=0;
		for (int i = 0; i < 32; ++i)
			registerMap["$" + std::to_string(i)] = i;
		registerMap["$zero"] = 0;
		registerMap["$at"] = 1;
		registerMap["$v0"] = 2;
		registerMap["$v1"] = 3;
		for (int i = 0; i < 4; ++i)
			registerMap["$a" + std::to_string(i)] = i + 4;
		for (int i = 0; i < 8; ++i)
			registerMap["$t" + std::to_string(i)] = i + 8, registerMap["$s" + std::to_string(i)] = i + 16;
		registerMap["$t8"] = 24;
		registerMap["$t9"] = 25;
		registerMap["$k0"] = 26;
		registerMap["$k1"] = 27;
		registerMap["$gp"] = 28;
		registerMap["$sp"] = 29;
		registerMap["$s8"] = 30;
		registerMap["$ra"] = 31;
        opcodeDict["add"]=0;
        opcodeDictAux["add"]=0;
        opcodeDict["sub"]=0;
        opcodeDictAux["sub"]=1;
        opcodeDict["mul"]=0;
        opcodeDictAux["mul"]=2;
        opcodeDict["and"]=0;
        opcodeDictAux["and"]=3;
        opcodeDict["or"]=0;
        opcodeDictAux["or"]=4;
        opcodeDict["slt"]=0;
        opcodeDictAux["slt"]=5;

        opcodeDict["sw"]=1;
        opcodeDict["lw"]=2;
        opcodeDict["beq"]=3;
        opcodeDictAux["beq"]=0;
        opcodeDict["bne"]=3;
        opcodeDictAux["bne"]=1;
        opcodeDict["j"]=4;
        opcodeDict["addi"]=5;
        opcodeDictAux["addi"]=0;
		constructCommands(file);
		commandCount.assign(commands.size(), 0);
	}

	// perform add operation
	int add(std::string r1, std::string r2, std::string r3)
	{
		return op(r1, r2, r3, [&](int a, int b)
				  { return a + b; });
	}

	// perform subtraction operation
	int sub(std::string r1, std::string r2, std::string r3)
	{
		return op(r1, r2, r3, [&](int a, int b)
				  { return a - b; });
	}

	// perform multiplication operation
	int mul(std::string r1, std::string r2, std::string r3)
	{
		return op(r1, r2, r3, [&](int a, int b)
				  { return a * b; });
	}

	// perform the binary operation
	int op(std::string r1, std::string r2, std::string r3, std::function<int(int, int)> operation)
	{
		if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0)
			return 1;
		registers[registerMap[r1]] = operation(registers[registerMap[r2]], registers[registerMap[r3]]);
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform the beq operation
	int beq(std::string r1, std::string r2, std::string label)
	{
		return bOP(r1, r2, label, [](int a, int b)
				   { return a == b; });
	}

	// perform the bne operation
	int bne(std::string r1, std::string r2, std::string label)
	{
		return bOP(r1, r2, label, [](int a, int b)
				   { return a != b; });
	}

	// implements beq and bne by taking the comparator
	int bOP(std::string r1, std::string r2, std::string label, std::function<bool(int, int)> comp)
	{
		if (!checkLabel(label))
			return 4;
		if (address.find(label) == address.end() || address[label] == -1)
			return 2;
		if (!checkRegisters({r1, r2}))
			return 1;
		PCnext = comp(registers[registerMap[r1]], registers[registerMap[r2]]) ? address[label] : PCcurr + 1;
		return 0;
	}

	// implements slt operation
	int slt(std::string r1, std::string r2, std::string r3)
	{
		if (!checkRegisters({r1, r2, r3}) || registerMap[r1] == 0)
			return 1;
		registers[registerMap[r1]] = registers[registerMap[r2]] < registers[registerMap[r3]];
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform the jump operation
	int j(std::string label, std::string unused1 = "", std::string unused2 = "")
	{
		if (!checkLabel(label))
			return 4;
		if (address.find(label) == address.end() || address[label] == -1)
			return 2;
		PCnext = address[label];
		return 0;
	}

	// perform load word operation
	int lw(std::string r, std::string location, std::string unused1 = "")
	{
		if (!checkRegister(r) || registerMap[r] == 0)
			return 1;
		int address = locateAddress(location);
		if (address < 0)
			return abs(address);
		registers[registerMap[r]] = data[address];
		PCnext = PCcurr + 1;
		return 0;
	}

	// perform store word operation
	int sw(std::string r, std::string location, std::string unused1 = "")
	{
		if (!checkRegister(r))
			return 1;
		int address = locateAddress(location);
		if (address < 0)
			return abs(address);
		data[address] = registers[registerMap[r]];
		PCnext = PCcurr + 1;
		return 0;
	}
	int locateAddress(std::string location)
	{
		if (location.back() == ')')
		{
			try
			{
				int lparen = location.find('('), offset = stoi(lparen == 0 ? "0" : location.substr(0, lparen));
				std::string reg = location.substr(lparen + 1);
				reg.pop_back();
				if (!checkRegister(reg))
					return -3;
				int address = registers[registerMap[reg]] + offset;
				if (address >= MAX)
					return -3;
				return registerMap[reg];
			}
			catch (std::exception &e)
			{
				return -4;
			}
		}
		try
		{
			int address = stoi(location);
			if (address >= MAX)
				return -3;
			return 0;
		}
		catch (std::exception &e)
		{
			return -4;
		}
	}

    int locateOffset(std::string location)
	{
		if (location.back() == ')')
		{
			try
			{
				int lparen = location.find('('), offset = stoi(lparen == 0 ? "0" : location.substr(0, lparen));
				std::string reg = location.substr(lparen + 1);
				reg.pop_back();
				if (!checkRegister(reg))
					return -3;
				int address = registers[registerMap[reg]] + offset;
				if (address >= MAX)
					return -3;
				return offset;
			}
			catch (std::exception &e)
			{
				return -4;
			}
		}
		try
		{
			int address = stoi(location);
			if (address >= MAX)
				return -3;
			return address;
		}
		catch (std::exception &e)
		{
			return -4;
		}
	}

	// perform add immediate operation
	int addi(std::string r1, std::string r2, std::string num)
	{
		if (!checkRegisters({r1, r2}) || registerMap[r1] == 0)
			return 1;
		try
		{
			registers[registerMap[r1]] = registers[registerMap[r2]] + stoi(num);
			PCnext = PCcurr + 1;
			return 0;
		}
		catch (std::exception &e)
		{
			return 4;
		}
	}
	// checks if label is valid
	inline bool checkLabel(std::string str)
	{
		return str.size() > 0 && isalpha(str[0]) && all_of(++str.begin(), str.end(), [](char c)
														   { return (bool)isalnum(c); }) &&
			   instructions.find(str) == instructions.end();
	}

	// checks if the register is a valid one
	inline bool checkRegister(std::string r)
	{
		return registerMap.find(r) != registerMap.end();
	}

	// checks if all of the registers are valid or not
	bool checkRegisters(std::vector<std::string> regs)
	{
		return std::all_of(regs.begin(), regs.end(), [&](std::string r)
						   { return checkRegister(r); });
	}

	/*
		handle all exit codes:
		0: correct execution
		1: register provided is incorrect
		2: invalid label
		3: unaligned or invalid address
		4: syntax error
		5: commands exceed memory limit
	*/
	void handleExit(exit_code code, int cycleCount)
	{
		std::cout << '\n';
		switch (code)
		{
		case 1:
			std::cerr << "Invalid register provided or syntax error in providing register\n";
			break;
		case 2:
			std::cerr << "Label used not defined or defined too many times\n";
			break;
		case 3:
			std::cerr << "Unaligned or invalid memory address specified\n";
			break;
		case 4:
			std::cerr << "Syntax error encountered\n";
			break;
		case 5:
			std::cerr << "Memory limit exceeded\n";
			break;
		default:
			break;
		}
		if (code != 0)
		{
			std::cerr << "Error encountered at:\n";
			for (auto &s : commands[PCcurr])
				std::cerr << s << ' ';
			std::cerr << '\n';
		}
		std::cout << "\nFollowing are the non-zero data values:\n";
		for (int i = 0; i < MAX / 4; ++i)
			if (data[i] != 0)
				std::cout << 4 * i << '-' << 4 * i + 3 << std::hex << ": " << data[i] << '\n'
						  << std::dec;
		std::cout << "\nTotal number of cycles: " << cycleCount << '\n';
		std::cout << "Count of instructions executed:\n";
		for (int i = 0; i < (int)commands.size(); ++i)
		{
			std::cout << commandCount[i] << " times:\t";
			for (auto &s : commands[i])
				std::cout << s << ' ';
			std::cout << '\n';
		}
	}

	// parse the command assuming correctly formatted MIPS instruction (or label)
	void parseCommand(std::string line)
	{
		// strip until before the comment begins
		line = line.substr(0, line.find('#'));
		std::vector<std::string> command;
		boost::tokenizer<boost::char_separator<char>> tokens(line, boost::char_separator<char>(", \t"));
		for (auto &s : tokens)
			command.push_back(s);
		// empty line or a comment only line
		if (command.empty())
			return;
		else if (command.size() == 1)
		{
			std::string label = command[0].back() == ':' ? command[0].substr(0, command[0].size() - 1) : "?";
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command.clear();
		}
		else if (command[0].back() == ':')
		{
			std::string label = command[0].substr(0, command[0].size() - 1);
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command = std::vector<std::string>(command.begin() + 1, command.end());
		}
		else if (command[0].find(':') != std::string::npos)
		{
			int idx = command[0].find(':');
			std::string label = command[0].substr(0, idx);
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command[0] = command[0].substr(idx + 1);
		}
		else if (command[1][0] == ':')
		{
			if (address.find(command[0]) == address.end())
				address[command[0]] = commands.size();
			else
				address[command[0]] = -1;
			command[1] = command[1].substr(1);
			if (command[1] == "")
				command.erase(command.begin(), command.begin() + 2);
			else
				command.erase(command.begin(), command.begin() + 1);
		}
		if (command.empty())
			return;
		if (command.size() > 4)
			for (int i = 4; i < (int)command.size(); ++i)
				command[3] += " " + command[i];
		command.resize(4);
		commands.push_back(command);
	}

	// construct the commands vector from the input file
	void constructCommands(std::ifstream &file)
	{
		std::string line;
		while (getline(file, line))
			parseCommand(line);
		file.close();
	}
    void executeCommandsUnpipelined()
	{	
		std::cout << commands[0][0] ;
		if (commands.size() >= MAX / 4)
		{
			handleExit(MEMORY_ERROR, 0);
			return;
		}

		int clockCycles = 0;
		while (PCcurr < commands.size())
		{
			++clockCycles;
			std::vector<std::string> &command = commands[PCcurr];
			if (instructions.find(command[0]) == instructions.end())
			{
				handleExit(SYNTAX_ERROR, clockCycles);
				return;
			}
			exit_code ret = (exit_code) instructions[command[0]](*this, command[1], command[2], command[3]);
			if (ret != SUCCESS)
			{
				handleExit(ret, clockCycles);
				return;
			}
			++commandCount[PCcurr];
			PCcurr = PCnext;
            std::vector<int> b = {0};
			printRegisters(clockCycles,b);
		}
		handleExit(SUCCESS, clockCycles);
	}
	void executeCommandsPipelined()
	{	
		struct IF1 S1;
        struct IF2 S2;
		struct Dec1 S3;
		struct Dec2 S4;
		struct RR S5;
        struct EX S6;
        struct MEM1 S7;
        struct MEM2 S8;
        struct WB S9;
        struct WB S10;
		int clockCycles = 0;
        std::vector<int> b = {0};
        printRegisters(clockCycles,b);
		while((S1.idle==0 or S2.idle==0 or S3.idle==0 or S4.idle==0 or S5.idle==0 or S6.idle==0 or S7.idle==0 or S8.idle==0 or S9.idle==0 or S10.idle==0)){
            std::vector<int > b = {0};
            if (S10.idle!=1){
                if (S10.opcode==0 or S10.opcode==5){
                    acquired[S10.write_reg]-=1;
                    S10.idle=1;
                    continue;}
                else if (S10.opcode==2){
                    acquired[S10.write_reg]-=1;
                    S10.idle=1;
                    continue;
                }
                }
			if (S9.idle!=1){
                if (S9.opcode==0 or S9.opcode ==5){
				registers[S9.write_reg]=S9.read_data;
				S9.idle = 1;
                S10.idle=0;
                S10.write_reg=S9.write_reg;
                S10.opcode=S9.opcode;}
                else if (S9.opcode==1){
				S9.idle = 1;
                S6.lock-=1;
                }
                else if (S9.opcode==2){
                    registers[S9.write_reg]=S9.read_data;
                S10.idle=0;
                S10.write_reg=S9.write_reg;
                S10.opcode=S9.opcode;
				S9.idle = 1;
                S6.lock-=1;}
                else if (S9.opcode==3 or S9.opcode==4){
                S9.idle = 1;
            }
			}

			if(S8.idle!=1){
                 if (S8.opcode==1){
                data[S8.forward_data]=S8.write_data_mem;
                b[0]=1;
                b.push_back(S8.forward_data);
                b.push_back(S8.write_data_mem);
                S9.write_reg=S8.write_reg;
				S9.idle = 0;
				S8.idle = 1;
                S9.opcode=S8.opcode;
            }
            else if (S8.opcode==2){
                S9.read_data=data[S8.forward_data];
				S9.write_reg=S8.write_reg;
				S9.idle = 0;
				S8.idle = 1;
                S9.opcode=S8.opcode;
            }
            }
            if(S7.idle!=1){
                 if (S7.opcode==1 or S7.opcode==2){
                // data[S8.forward_data]=S8.write_data_mem;
                S8.write_reg=S7.write_reg;
                S8.forward_data=S7.forward_data;
                S8.write_data_mem=S7.write_data_mem;
                // std::cout<<"MEM: "<<std::endl;
				// std::cout<<"write reg: "<<S9.write_reg<<std::endl;
				// std::cout<<"write_data_mem: "<<S8.write_data_mem<<std::endl;
				// // acquired[S5.write_reg]=0;
				S8.idle = 0;
				S7.idle = 1;
                S8.opcode=S7.opcode;
                S6.lock+=1;
            }
            }
			if (S6.idle!=1){
                if (S6.opcode==0 or S6.opcode==5){
                    if (S6.lock==0){
                    if (S6.opcodeAux==0){S6.result = S6.data1+S6.data2;}
                    else if (S6.opcodeAux==1){S6.result = S6.data1-S6.data2;}
                    else if (S6.opcodeAux==2){S6.result = S6.data1*S6.data2;}
                    else if (S6.opcodeAux==3){S6.result = S6.data1&S6.data2;}
                    else if (S6.opcodeAux==4){S6.result = S6.data1|S6.data2;}
                    else if (S6.opcodeAux==5){S6.result = S6.data1<S6.data2;}
				S9.read_data=S6.result;
				S9.write_reg = S6.write_reg;
				
				S9.idle = 0;
				S6.idle = 1;
                S9.opcode=S6.opcode;}
                else{
                    clockCycles++;
					printRegisters(clockCycles,b);
                    continue;
                }}
                else if (S6.opcode==1){
				S6.result = (S6.data1+S6.data2)/4;
				S7.forward_data=S6.result;
				S7.write_reg = S6.write_reg;
                S7.write_data_mem=S6.write_data;
				S7.idle = 0;
				S6.idle = 1;
                S7.opcode=S6.opcode;}
                else if (S6.opcode==2){
                    S6.result = (S6.data1+S6.data2)/4;
                    S7.forward_data=S6.result;
				    S7.write_reg = S6.write_reg;			
				S7.idle = 0;
				S6.idle = 1;
                S7.opcode=S6.opcode;}
                else if (S6.opcode==3){
                    S6.branch=S5.branch;
                    bool c;
                    if (S6.opcodeAux==0){c=S6.data1==S6.data2;}
                    else if(S6.opcodeAux==1){c=S6.data1!=S6.data2;}
                    if(c){
                        S1.instr=S6.write_reg;
                    }
                    else{
                        S1.instr=S6.branch;
                    }
                    S9.idle = 0 ;
				    S6.idle = 1;
                    S5.idle = 1;
                    S4.idle=1;
				    S3.idle = 1;
                    S2.idle = 1;
                    S9.opcode=S6.opcode;
                    S9.write_reg = S6.write_reg;
                    clockCycles++;
					printRegisters(clockCycles,b);
                    continue;
				
                }
                else if (S6.opcode==4){
				S9.idle = 0;
                S6.idle = 1;
                S9.opcode=S6.opcode;
            }
			}
            if (S1.instr>=commands.size()){
				S1.idle=1;
			}
            else{
                S1.idle=0;
            }

			if (S5.idle!=1){
				if (S5.opcode==0 or S5.opcode == 5){
				if ((acquired[S5.read_reg1]>0) or (S5.opcode==0 && acquired[S5.read_reg2]>0)){
					clockCycles++;
                    printRegisters(clockCycles,b);
					continue;
				}
				else{
					S5.read_data1 = registers[S5.read_reg1];
                    if (S5.opcode==0){S5.read_data2 = registers[S5.read_reg2];}
                    else{S5.read_data2 = S5.read_reg2;}
					S6.data1 = S5.read_data1;
					S6.data2 = S5.read_data2;
					S6.write_reg = S5.write_reg;
					acquired[S5.write_reg] += 1;
					S6.idle = 0;
					S5.idle = 1;
                    S6.opcode=S5.opcode;
                    S6.opcodeAux=S5.opcodeAux;
				}
			}
            else if (S5.opcode < 3 && 0<S5.opcode){
                if (acquired[S5.read_reg1]>0){
                    clockCycles++;
					printRegisters(clockCycles,b);
					continue;
				}
                else{
					S5.read_data1 = registers[S5.read_reg1];
					S6.data1 = S5.read_data1;
                    S6.data2= S5.read_reg2;
					S6.write_reg = S5.write_reg;
					if (S5.opcode == 2){acquired[S5.write_reg] += 1;}
                    else{
                        if (acquired[S5.write_reg]==0){
                            S5.write_data=registers[S5.write_reg];
                        }
                        else{
                        clockCycles++;
                            printRegisters(clockCycles,b);
                                continue;
                    }
                    S6.write_data=S5.write_data;}
					S6.idle = 0;
					S5.idle = 1;
                    S6.opcode=S5.opcode;
            }}
            else if (S5.opcode ==3){
                if (acquired[S5.read_reg1]>0 or acquired[S5.read_reg2]>0){
                    clockCycles++;
					printRegisters(clockCycles,b);
                    continue;
				}
                else{
                S5.read_data1 = registers[S5.read_reg1];
                S5.read_data2 = registers[S5.read_reg2];
                S6.data1 = S5.read_data1;
                S6.data2 = S5.read_data2;
                S6.write_reg = S5.write_reg;
                S6.idle = 0;
                S5.idle = 1;
                S6.opcode=S5.opcode;
                S4.idle = 1;
                S3.idle = 1;
                S2.idle = 1;
                S1.idle = 1;
                S6.opcodeAux=S5.opcodeAux;
                S5.branch = S4.branch;}
            }
            else if (S5.opcode ==4){
				S6.idle = 0;
                S5.idle = 1;
                S6.opcode=S5.opcode;
            }
            }
            if (S1.instr>=commands.size()){
				S1.idle=1;
			}
            else{
                S1.idle=0;
            }
            if (S4.idle!=1){
                if (S4.opcode==4){
                S1.instr=S4.write_reg;
                S5.write_reg = S4.write_reg;
                S5.idle = 1;
                S4.idle = 1;
                S3.idle = 1;
                S2.idle = 1;
                    clockCycles++;
                    printRegisters(clockCycles,b);
                    continue;
            }
                S5.read_reg1=S4.read_reg1;
                S5.read_reg2=S4.read_reg2;
                S5.write_reg=S4.write_reg;
                S5.idle = 0;
                S4.idle = 1;
                S5.opcode=S4.opcode;
                S5.opcodeAux=S4.opcodeAux;
                S4.branch = S3.branch;
            }
            if (S1.instr>=commands.size()){
				S1.idle=1;
			}
			else{S1.idle=0;}

            if (S3.idle!=1){
                S4.read_reg1=S3.read_reg1;
                S4.read_reg2=S3.read_reg2;
                S4.write_reg=S3.write_reg;
                S4.idle = 0;
                S3.idle = 1;
                S4.opcode=S3.opcode;
                S4.opcodeAux=S3.opcodeAux;
                S3.branch = S2.branch;
            }
            if (S1.instr>=commands.size()){
				S1.idle=1;
			}
			else{S1.idle=0;}

			if (S2.idle!=1){
                if (S2.opcode == 0 or S2.opcode == 5){
                    // acquired[S3.write_reg] = 1; 
                    S3.read_reg1 = registerMap[commands[S2.instr][2]];
                    if (S2.opcode ==0){S3.read_reg2 = registerMap[commands[S2.instr][3]];}
                    else{S3.read_reg2 = stoi(commands[S2.instr][3]);}
                    S3.write_reg = registerMap[commands[S2.instr][1]];
                    S3.idle = 0;
                    S2.idle = 1;
                    S3.opcode=S2.opcode;
                    S3.opcodeAux=S2.opcodeAux;
                    S2.instr=S1.instr;
                    }
				else if (0 < S2.opcode && S2.opcode < 3){
                    // if (S2.opcode == 2){acquired[S2.write_reg] = 1;}
                    S3.write_reg = registerMap[commands[S2.instr][1]];
                    S3.read_reg1 = locateAddress(commands[S2.instr][2]);
                    S3.read_reg2 = locateOffset(commands[S2.instr][2]);
                    // acquired[S2.write_reg] = 1;
                    S3.idle = 0;
                    S2.idle = 1;
                    S3.opcode=S2.opcode;
                    S2.opcode=S1.opcode;
                    S2.instr=S1.instr;
			}
                else if (S2.opcode ==3){
                    S3.read_reg1 = registerMap[commands[S2.instr][1]];
                    S3.read_reg2 = registerMap[commands[S2.instr][2]];
                    S3.write_reg = address[commands[S2.instr][3]];
                    S3.idle = 0;
                    S2.idle = 1;
                    S3.opcode=S2.opcode;
                    S3.opcodeAux=S2.opcodeAux;
                    S2.instr=S1.instr;
                    S2.branch=S2.instr;
                }
                else if (S2.opcode ==4){
                    S3.write_reg = address[commands[S2.instr][1]];
                    S3.idle = 0;
                    S2.idle = 1;
                    S3.opcode=S2.opcode;
                    S2.instr=S1.instr;
                }
            }
                
            if (S1.instr>=commands.size()){
				S1.idle=1;
			}
			else{S1.idle=0;}

            if (S1.idle!=1){
                S1.opcode = opcodeDict[commands[S1.instr][0]];
                S1.opcodeAux = opcodeDictAux[commands[S1.instr][0]];
                S2.idle = 0;
                S1.idle = 1;
                S2.opcode=S1.opcode;
                S2.opcodeAux=S1.opcodeAux;
                S2.instr=S1.instr;
                S1.instr++;
            }
			if (S1.instr>=commands.size()){
				S1.idle=1;
			}
			else{S1.idle=0;}

			clockCycles++;
			printRegisters(clockCycles,b);
		}
    }

	void executeCommandsPipelinedBypass()
	{	
		struct IF1 S1;
        struct IF2 S2;
		struct Dec1 S3;
		struct Dec2 S4;
		struct RR S5;
        struct EX S6;
        struct MEM1 S7;
        struct MEM2 S8;
        struct WB S9;
        struct WB S10;
		int clockCycles = 0;
        std::vector<int> b = {0};
        printRegisters(clockCycles,b);
		while((S1.idle==0 or S2.idle==0 or S3.idle==0 or S4.idle==0 or S5.idle==0 or S6.idle==0 or S7.idle==0 or S8.idle==0 or S9.idle==0 or S10.idle==0)){
            std::vector<int > b = {0};
            if (S10.idle!=1){
                if (S10.opcode==0 or S10.opcode==5){
                    acquired[S10.write_reg]-=1;
                    S10.idle=1;
                    continue;}
                else if (S10.opcode==2){
                    acquired[S10.write_reg]-=1;
                    S10.idle=1;
                    continue;
                }
                }
			if (S9.idle!=1){
                if (S9.opcode==0 or S9.opcode ==5){
				registers[S9.write_reg]=S9.read_data;
				//first half
				S9.idle = 1;
                S10.idle=0;
                S10.write_reg=S9.write_reg;
                S10.opcode=S9.opcode;}
                else if (S9.opcode==1){
				S9.idle = 1;
                S6.lock-=1;
                }
                else if (S9.opcode==2){
                    registers[S9.write_reg]=S9.read_data;
				//first half
                S10.idle=0;
                S10.write_reg=S9.write_reg;
                S10.opcode=S9.opcode;
				S9.idle = 1;
                S6.lock-=1;
                std::vector<int> a;
                a.push_back(S9.write_reg);
                a.push_back(S9.read_data);
                Latches[2]=Latches[1];
                Latches[1]=Latches[0];
                Latches[0]=a;
                }
                else if (S9.opcode==3 or S9.opcode==4){
                S9.idle = 1;
            }
			}

			if(S8.idle!=1){
                if (S8.opcode==1){
                    if (S8.write_data_mem==INT_MIN){
                        if (Latches[0][0]==S8.write_reg){
                            S8.write_data_mem=Latches[0][1];
                            
                        }
                        else if(Latches[1][0]==S8.write_reg){
                            S8.write_data_mem=Latches[1][1];
                        }
                        else if(Latches[2][0]==S8.write_reg){
                            S8.write_data_mem=Latches[2][1];
                        }
                        else
                        {clockCycles++;
                        printRegisters(clockCycles,b);
                        continue;}} 
                data[S8.forward_data]=S8.write_data_mem;
                b[0]=1;
                b.push_back(S8.forward_data);
                b.push_back(S8.write_data_mem);
                S9.write_reg=S8.write_reg;
				S9.idle = 0;
				S8.idle = 1;
                S9.opcode=S8.opcode;
            }
            else if (S8.opcode==2){
                S9.read_data=data[S8.forward_data];
				S9.write_reg=S8.write_reg;
				S9.idle = 0;
				S8.idle = 1;
                S9.opcode=S8.opcode;
            }
            }
            if(S7.idle!=1){
                 if (S7.opcode==1 or S7.opcode==2){
                S8.write_reg=S7.write_reg;
                S8.forward_data=S7.forward_data;
                S8.write_data_mem=S7.write_data_mem;
				S8.idle = 0;
				S7.idle = 1;
                S8.opcode=S7.opcode;
                S6.lock+=1;
            }
            }
			if (S6.idle!=1){
                if (S6.opcode==0 or S6.opcode==5){
                    if (S6.lock==0){                 
                    if (S6.data1==INT_MIN || S6.data2==INT_MIN){
                        if (S6.data1==INT_MIN){
                            if (Latches[0][0]==S6.read_reg1){
                                S6.data1=Latches[0][1];
                            }
                            else if(Latches[1][0]==S6.read_reg1){
                                S6.data1=Latches[1][1];
                            }
                            else if(Latches[2][0]==S6.read_reg1){
                                S6.data1=Latches[2][1];
                            }
                            else
                            {clockCycles++;
                        printRegisters(clockCycles,b);
					        continue;}}
                        if (S6.data2==INT_MIN){ 
                            if (Latches[0][0]==S6.read_reg2){
                                S6.data2=Latches[0][1];
                            }
                            else if(Latches[1][0]==S6.read_reg2){
                                S6.data2=Latches[1][1];
                            }
                            else if(Latches[2][0]==S6.read_reg2){
                                S6.data2=Latches[2][1];
                            }
                            else{clockCycles++;
                        printRegisters(clockCycles,b);
					        continue;}
                        }}
                    if (S6.opcodeAux==0){S6.result = S6.data1+S6.data2;}
                    else if (S6.opcodeAux==1){S6.result = S6.data1-S6.data2;}
                    else if (S6.opcodeAux==2){S6.result = S6.data1*S6.data2;}
                    else if (S6.opcodeAux==3){S6.result = S6.data1&S6.data2;}
                    else if (S6.opcodeAux==4){S6.result = S6.data1|S6.data2;}
                    else if (S6.opcodeAux==5){S6.result = S6.data1<S6.data2;}
				S9.read_data=S6.result;
				S9.write_reg = S6.write_reg;
				S9.idle = 0;
				S6.idle = 1;
                S9.opcode=S6.opcode;
                std::vector<int> a;
                a.push_back(S6.write_reg);
                a.push_back(S6.result);
                Latches[1]=Latches[0];
                Latches[0]=a;
                }
                else{
                    if (S6.opcodeAux==0){S6.result = S6.data1+S6.data2;}
                    else if (S6.opcodeAux==1){S6.result = S6.data1-S6.data2;}
                    else if (S6.opcodeAux==2){S6.result = S6.data1*S6.data2;}
                    else if (S6.opcodeAux==3){S6.result = S6.data1&S6.data2;}
                    else if (S6.opcodeAux==4){S6.result = S6.data1|S6.data2;}
                    else if (S6.opcodeAux==5){S6.result = S6.data1<S6.data2;}
				
                std::vector<int> a;
                
                a.push_back(S6.write_reg);
                a.push_back(S6.result);
                // Latches[1]=Latches[0];
                // Latches[0]=a;
					clockCycles++;
                        printRegisters(clockCycles,b);
                    continue;}
                }
                else if (S6.opcode==1){
                if (S6.data1==INT_MIN){
                        if (Latches[0][0]==S6.read_reg1){
                                S6.data1=Latches[0][1];
    
                            }
                            else if(Latches[1][0]==S6.read_reg1){
                                S6.data1=Latches[1][1];
                            }
                            else if(Latches[2][0]==S6.read_reg1){
                                S6.data1=Latches[2][1];
                            }
                        else
                        {clockCycles++;
                        printRegisters(clockCycles,b);
                        continue;}}

				S6.result = (S6.data1+S6.data2)/4;
				S7.forward_data=S6.result;
				S7.write_reg = S6.write_reg;
                S7.write_data_mem=S6.write_data;
				
				
				S7.idle = 0;
				S6.idle = 1;
                S7.opcode=S6.opcode;}
                else if (S6.opcode==2){
                    if (S6.data1==INT_MIN){
                        if (Latches[0][0]==S6.read_reg1){
                                S6.data1=Latches[0][1];
                                
                            }
                            else if(Latches[1][0]==S6.read_reg1){
                                S6.data1=Latches[1][1];
                            }
                            else if(Latches[2][0]==S6.read_reg1){
                                S6.data1=Latches[2][1];
                            }
                        else
                        {
                        clockCycles++;
                        printRegisters(clockCycles,b);
                        continue;}}
                    S6.result = (S6.data1+S6.data2)/4;
                    S7.forward_data=S6.result;
				    S7.write_reg = S6.write_reg;
                    
				
				S7.idle = 0;
				S6.idle = 1;
                S7.opcode=S6.opcode;
                }
                else if (S6.opcode==3){
                    
                    S6.branch=S5.branch;
                    
                    if (S6.data1==INT_MIN || S6.data2==INT_MIN){
                        if (S6.data1==INT_MIN){
                            if (Latches[0][0]==S6.read_reg1){
                                S6.data1=Latches[0][1];
                                
                            }
                            else if(Latches[1][0]==S6.read_reg1){
                                S6.data1=Latches[1][1];
                            }
                            else if(Latches[2][0]==S6.read_reg1){
                                S6.data1=Latches[2][1];
                            }
                            else
                            {
                            clockCycles++;
                        printRegisters(clockCycles,b);
					        continue;}}
                        if (S6.data2==INT_MIN){ 
                            if (Latches[0][0]==S6.read_reg2){
                                S6.data2=Latches[0][1];
                            }
                            else if(Latches[1][0]==S6.read_reg2){
                                S6.data2=Latches[1][1];
                            }
                            else if(Latches[2][0]==S6.read_reg2){
                                S6.data2=Latches[2][1];
                            }
                            else{clockCycles++;
                        printRegisters(clockCycles,b);
					        continue;}
                        }}
                    bool c;

                    if (S6.opcodeAux==0){c=S6.data1==S6.data2;}
                    else if(S6.opcodeAux==1){c=S6.data1!=S6.data2;}
                    if(c){
                       S1.instr=S6.write_reg;
                    }
                    else{
                        S1.instr=S6.branch;
                        
                    }
                    S9.idle = 0 ;
				    S6.idle = 1;
                    S5.idle = 1;
                    S4.idle=1;
				    S3.idle = 1;
                    S2.idle = 1;
                    S9.opcode=S6.opcode;
                    S9.write_reg = S6.write_reg;

                    clockCycles++;
                    printRegisters(clockCycles,b);
                    continue;
				
                }
                else if (S6.opcode==4){
				S9.idle = 0;
                S6.idle = 1;
                S9.opcode=S6.opcode;
            }
			}
            if (S1.instr>=commands.size()){
				S1.idle=1;
			}
            else{
                S1.idle=0;
            }

			if (S5.idle!=1){
				if (S5.opcode==0 or S5.opcode == 5){
                    if (S5.opcode==0){
                        
                        if(acquired[S5.read_reg2]>0){S6.data2=INT_MIN;
                        }
                        else{S5.read_data2 = registers[S5.read_reg2];
                        S6.data2 = S5.read_data2;}}
                     else{S5.read_data2 = S5.read_reg2;
                        S6.data2 = S5.read_data2;}
                    if (acquired[S5.read_reg1]>0){S6.data1=INT_MIN;
                    }
                    else{
					S5.read_data1 = registers[S5.read_reg1];
					S6.data1 = S5.read_data1;}

					S6.write_reg = S5.write_reg;
					
					acquired[S5.write_reg] += 1;
					S6.idle = 0;
					S5.idle = 1;
                    S6.opcode=S5.opcode;
                    S6.opcodeAux=S5.opcodeAux;
                    S6.read_reg1=S5.read_reg1;
                    S6.read_reg2=S5.read_reg2;
				}
            else if (S5.opcode < 3 && 0<S5.opcode){
                if (acquired[S5.read_reg1]>0){S6.data1=INT_MIN;}
                else{
					S5.read_data1 = registers[S5.read_reg1];
					S6.data1 = S5.read_data1;}
                    S6.data2= S5.read_reg2;
					S6.write_reg = S5.write_reg;
				
					if (S5.opcode == 2){acquired[S5.write_reg] += 1;}
                    else{
                        if (acquired[S5.write_reg]==0){
                            S5.write_data=registers[S5.write_reg];
                        }
                        else{
                            S5.write_data=INT_MIN;
                    }}
                    S6.write_data=S5.write_data;
                
					S6.idle = 0;
					S5.idle = 1;
                    S6.opcode=S5.opcode;
                    S6.read_reg1=S5.read_reg1;
                    S6.read_reg2=S5.read_reg2;
            }
            else if (S5.opcode ==3){
                if(acquired[S5.read_reg2]>0){S6.data2=INT_MIN;}
					else{S5.read_data2 = registers[S5.read_reg2];
                        S6.data2 = S5.read_data2;}
                    if (acquired[S5.read_reg1]>0){S6.data1=INT_MIN;}
                    else{
					S5.read_data1 = registers[S5.read_reg1];
					S6.data1 = S5.read_data1;}
                S6.write_reg = S5.write_reg;
                S6.idle = 0;
                S5.idle = 1;
                S6.opcode=S5.opcode;
                S4.idle = 1;
                S3.idle = 1;
                S2.idle = 1;
                S1.idle = 1;
                S6.opcodeAux=S5.opcodeAux;
                S5.branch = S4.branch;
                S6.read_reg1=S5.read_reg1;
                S6.read_reg2=S5.read_reg2;}
            else if (S5.opcode ==4){
				S6.idle = 0;
                S5.idle = 1;
                S6.opcode=S5.opcode;
            }
            }
            if (S1.instr>=commands.size()){
				S1.idle=1;
			}
            else{
                S1.idle=0;
            }
            if (S4.idle!=1){
                if (S4.opcode==4){
                S1.instr=S4.write_reg;
                S5.write_reg = S4.write_reg;
                S5.idle = 1;
                S4.idle = 1;
                S3.idle = 1;
                S2.idle = 1;
                    clockCycles++;
                    printRegisters(clockCycles,b);
                    continue;
            }
                S5.read_reg1=S4.read_reg1;
                S5.read_reg2=S4.read_reg2;
                S5.write_reg=S4.write_reg;
                S5.idle = 0;
                S4.idle = 1;
                S5.opcode=S4.opcode;
                S5.opcodeAux=S4.opcodeAux;
                S4.branch = S3.branch;
            }
            if (S1.instr>=commands.size()){
				S1.idle=1;
			}
			else{S1.idle=0;}

            if (S3.idle!=1){
                S4.read_reg1=S3.read_reg1;
                S4.read_reg2=S3.read_reg2;
                S4.write_reg=S3.write_reg;
                S4.idle = 0;
                S3.idle = 1;
                S4.opcode=S3.opcode;
                S4.opcodeAux=S3.opcodeAux;
                S3.branch = S2.branch;
            }
            if (S1.instr>=commands.size()){
				S1.idle=1;
			}
			else{S1.idle=0;}

			if (S2.idle!=1){
                if (S2.opcode == 0 or S2.opcode == 5){
                    // acquired[S3.write_reg] = 1; 
                    S3.read_reg1 = registerMap[commands[S2.instr][2]];
                    if (S2.opcode ==0){S3.read_reg2 = registerMap[commands[S2.instr][3]];}
                    else{S3.read_reg2 = stoi(commands[S2.instr][3]);}
                    S3.write_reg = registerMap[commands[S2.instr][1]];
                    S3.idle = 0;
                    S2.idle = 1;
                    S3.opcode=S2.opcode;
                    S3.opcodeAux=S2.opcodeAux;
                    S2.instr=S1.instr;
                    }
				else if (0 < S2.opcode && S2.opcode < 3){
                    // if (S2.opcode == 2){acquired[S2.write_reg] = 1;}
                    S3.write_reg = registerMap[commands[S2.instr][1]];
                    S3.read_reg1 = locateAddress(commands[S2.instr][2]);
                    S3.read_reg2 = locateOffset(commands[S2.instr][2]);
                    // acquired[S2.write_reg] = 1;
                    S3.idle = 0;
                    S2.idle = 1;
                    S3.opcode=S2.opcode;
                    S2.opcode=S1.opcode;
                    S2.instr=S1.instr;
			}
                else if (S2.opcode ==3){
                    S3.read_reg1 = registerMap[commands[S2.instr][1]];
                    S3.read_reg2 = registerMap[commands[S2.instr][2]];
                    S3.write_reg = address[commands[S2.instr][3]];
                    S3.idle = 0;
                    S2.idle = 1;
                    S3.opcode=S2.opcode;
                    S3.opcodeAux=S2.opcodeAux;
                    S2.instr=S1.instr;
                    S2.branch=S2.instr;
                }
                else if (S2.opcode ==4){
                    S3.write_reg = address[commands[S2.instr][1]];
                    S3.idle = 0;
                    S2.idle = 1;
                    S3.opcode=S2.opcode;
                    S2.instr=S1.instr;
                }
            }
                
            if (S1.instr>=commands.size()){
				S1.idle=1;
			}
			else{S1.idle=0;}

            if (S1.idle!=1){
                S1.opcode = opcodeDict[commands[S1.instr][0]];
                S1.opcodeAux = opcodeDictAux[commands[S1.instr][0]];
                S2.idle = 0;
                S1.idle = 1;
                S2.opcode=S1.opcode;
                S2.opcodeAux=S1.opcodeAux;
                S2.instr=S1.instr;
                S1.instr++;
            }
			if (S1.instr>=commands.size()){
				S1.idle=1;
			}
			else{S1.idle=0;}

			clockCycles++;
			printRegisters(clockCycles,b);
		}
    }

	// print the register data in hexadecimal
	void printRegisters(int clockCycle, std::vector<int> a)
	{
		
		for (int i = 0; i < 32; ++i){
			std::cout << registers[i] << ' ';}
		std::cout << std::endl;
        for (int i= 0; i<a.size(); i++){
            if (i==a.size()-1){std::cout << a[i];}
            else
            {std::cout << a[i] << ' ';}
        }
				  std::cout<< std::endl;
	}
};
#endif

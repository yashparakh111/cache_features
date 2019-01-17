#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <typeinfo>
#include "CodeObject.h"
#include "InstructionDecoder.h"
#include "CFG.h"

#include <map>
#include <fstream>

using namespace std;
using namespace Dyninst;
using namespace ParseAPI;

using namespace InstructionAPI;

// allows for a generic mapping from function_name to any value defined under this struct
typedef struct feature_map {
	std::map<int, uint8_t> loop_depth_map;	// maps instruction offset to a loop depth
} feature_map_t;

typedef std::map<std::string, feature_map_t> function_map;

// writes map to file
bool mapToFile(const string &filename, const function_map &fileMap) {
	std::ofstream ofile;
	ofile.open(filename.c_str());
	if(!ofile) {
		return false;
	}

	// write the map to ofile
	for(function_map::const_iterator iter = fileMap.begin(); iter != fileMap.end(); ++iter) {
		ofile << iter->first << endl; // write method name
		//cout << iter->first << endl; // print method name
		
		// loop through all offset-loop_depth pairs
		std::map<int, uint8_t>::const_iterator iter2 = ((iter->second).loop_depth_map).begin();
		for(; iter2 != ((iter->second).loop_depth_map).end(); ++iter2) {
			ofile << iter2->first << "|" << unsigned(iter2->second) << endl;		// write offset|loop_depth pairs
			//cout << iter2->first << "|" << unsigned(iter2->second) << endl;		// write offset|loop_depth pairs
		}
		ofile << "-" << endl;
		//cout << "-" << endl;
	}
	
	ofile.close();
	return true;
}

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("Usage: %s <binary path>\n", argv[0]);
		return -1;
	}

	char* binaryPath = argv[1];

	SymtabCodeSource *sts;
	CodeObject *co;
	Instruction::Ptr instr;
	SymtabAPI::Symtab *symTab;
	std::string binaryPathStr(binaryPath);

	// map function to another map from offset to loop depth
	function_map func_map;

	bool isParsable = SymtabAPI::Symtab::openFile(symTab, binaryPathStr);

	if(isParsable == false) {
		const char *error = "error: file cannot be parsed";
		cout << error;
		return -1;
	}

	sts = new SymtabCodeSource(binaryPath);
	co = new CodeObject(sts);
	//parse the binary given as a command line arg
	co->parse();

	// get list of all functions in the binary
	const CodeObject::funclist &all = co->funcs();
	if(all.size() == 0) {
		const char* error = "error: no functions in file";
		cout << error;
		return -1;
	}

	auto fit = all.begin();
	Function *f = *fit;

	// create an Instruction decoder which will convert the binary opcodes to strings
	InstructionDecoder decoder(f->isrc()->getPtrToInstruction(f->addr()),
			InstructionDecoder::maxInstructionLength,
			f->region()->getArch());

	for(;fit != all.end(); ++fit) {
		Function *f = *fit;

		// mapping features
		std::map<int, uint8_t> l_depth_map;	// maps read instruction offsets to loop depth

		// output the address of this function
		cout << "0x" << hex << f->addr() << ": " << f->name() << endl;

		// prepare the first level loops
		vector<LoopTreeNode*> children = f->getLoopTree()->children;

		uint8_t loop_depth = 1;
		uint8_t next_depth_loop_num = 0;		
		uint8_t curr_depth_loop_num = children.size();

		queue<LoopTreeNode*> loops;

		for(int l = 0; l < curr_depth_loop_num; l++) {
			loops.push(children[l]);
		}

		// create an Instruction deocder which will convert the binary opcodes to strings
		InstructionDecoder decoder(f->isrc()->getPtrToInstruction(f->addr()), InstructionDecoder::maxInstructionLength, f->region()->getArch());

		// run BFS and track tree depth (loop depth)
		while(!loops.empty()) {
			LoopTreeNode* loop_node = loops.front();
			loops.pop();
			Loop* loop = loop_node->loop;

			cout << "\t" << unsigned(loop_depth) << ": " << loop_node->name() << "\t" << endl;


			// enqueue all children for current node (these have a loop depth of loop_depth+1)
			children = loop_node->children;
			for(int l = 0; l < children.size(); l++) {
				loops.push(children[l]);
			}

			// get all basic blocks for current loop
			vector<Block*> basic_blocks;
			if(loop->getLoopBasicBlocksExclusive(basic_blocks)) {
				for(int b = 0; b < basic_blocks.size(); b++) {
					Block* curr_block = basic_blocks[b];
					Address curr_addr = curr_block->start();
					Address end_addr = curr_block->last();

					// if current BBL has zero instruction, don't output it
					if(curr_addr == end_addr) {
						continue;
					}

					// check all instructions in the current BBL for readMemory
					while(curr_addr < end_addr) {
						// decode current instruction
						instr = decoder.decode(
								(unsigned char*) f->isrc()->getPtrToInstruction(curr_addr));

						// capture read instructions only
						if(instr->readsMemory()) {
							int offset = (int)(curr_addr - f->addr());
							l_depth_map[offset] = loop_depth;
							cout << "\t\t0x" << hex << curr_addr - f->addr();
							cout << ": \"" << instr->format() << "\"" << endl;
						}
						curr_addr += instr->size();
					}
				}
			}

			curr_depth_loop_num--;
			next_depth_loop_num += children.size();

			// update loop_depth
			if(curr_depth_loop_num == 0) {
				curr_depth_loop_num = next_depth_loop_num;
				loop_depth++;
				next_depth_loop_num = 0;
			}
		}
		feature_map_t f_map = {l_depth_map};
		func_map[f->name()] = f_map;	// map this function's name to its loop_depth map
		cout << endl << endl;

	}

	//cout << unsigned(loop_depth_map["main"]
	string filename = "loop_depth.trace";
	mapToFile(filename, func_map);

	return 0;
}

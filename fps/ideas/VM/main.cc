#include "vm_types.h"


int main(int argc, char* argv[])
{
	VM vm;

	char bytecode[]{0x05, 0, 0x05, 15, 0x00}; //INST_LITERAL, WIZARD_ID, INST_LITERAL, VALUE, SET_HEALTH 


	vm.interpret(bytecode, 5);
}
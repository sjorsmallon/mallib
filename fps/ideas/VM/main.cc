#include "vm_types.h"

int main(int argc, char* argv)
{
	VM vm;

	// set the health of the wizard ID to some value.
	// since the parameters need to be provided earlier,
	// we fill the stack with two literals: one for the wizard id,
	// and one for the actual value for the health.

	// @Note(SJM): should the order  be reversed? since we need to pop them from reserve off the stack?
	
	char bytecode[]{0x05, 0, 0x05, 15, 0x00}; //INST_LITERAL, WIZARD_ID, INST_LITERAL, VALUE, SET_HEALTH 


	vm.interpret(bytecode, sizeof(bytecode) / sizeof(char));
}

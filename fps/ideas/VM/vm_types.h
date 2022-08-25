#ifndef _INCLUDED_VM_
#define _INCLUDED_VM_

#include <iostream>
#include <cassert>
enum Instruction
{
	INST_SET_HEALTH = 0x00,
	INST_SET_WISDOM = 0x01,
	INST_SET_AGILITY = 0x02,
	INST_PLAY_SOUND = 0x03,
	INST_SPAWN_PARTICLES = 0x04,
	INST_LITERAL = 0x05,
	INST_GET_HEALTH = 0x06,
	INST_GET_WISDOM = 0x07,
	INST_GET_AGILITY = 0x08,
	// arithmetic
	INST_ADD = 0x09,
	INST_SUB = 0x0A,
	INST_MUL = 0x0B,
	INST_DIV = 0x0C
};

inline void set_health(int wizard_id, int amount)
{

	std::cerr<< "set_health for wizard:" << wizard_id <<" to " << amount <<"\n";
}
inline void set_wisdom(int wizard_id, int amount)
{
	std::cerr<< "set_wisdom\n";
}
inline void set_agility(int wizard_id, int amount)
{
	std::cerr<< "set_agility\n";

}
inline void play_sound(int sound_id)
{
	std::cerr<< "play_sound\n";

}

inline int get_health(int wizard_id)
{
	std::cerr << "getting the wizard health \n";
	return 100;	
}

inline int get_wisdom(int wizard_id)
{
	std::cerr << "getting the wizard wisdom \n";
	return 50;
}

inline int get_agility(int wizard_id)
{
	std::cerr << "getting the wizard agility\n";
	return 123;
}


inline void spawn_particles(int particle_type)
{	
	std::cerr<< "spawn_particles\n";
}

constexpr size_t MAX_STACK_SIZE = 128;
struct VM
{
	int stack[MAX_STACK_SIZE];
	int stack_size = 0;

	void interpret(char bytecode[], int size)
	{
		for (size_t idx = 0; idx != size; ++idx)
		{
			char instruction = bytecode[idx];

			switch (instruction)
			{
				case INST_SET_HEALTH:
				{
					std::cerr << "first value of stack:" << stack[0];
					int amount = pop();
					int wizard = pop();
					set_health(wizard, amount);

					break;
				}
				case INST_SET_WISDOM:
				{
					int amount = pop();
					int wizard = pop();
					set_wisdom(0, 100);

				  	break;
				}
				case INST_SET_AGILITY:
				{
					int amount = pop();
					int wizard = pop();
					set_agility(0, 100);

				  	break;
				}
				case INST_PLAY_SOUND:
				{
					play_sound(pop());
				  	break;
				}
				case INST_SPAWN_PARTICLES:
				{
					spawn_particles(pop());
				  	break;
				}
				case INST_LITERAL:
				{
					int next_byte_idx = idx + 1;
					idx += 1;
					int value = bytecode[next_byte_idx];
					std::cerr << "got literal " << value << '\n';
					push(value);
				  	break;
				}
				case INST_GET_HEALTH:
				{
					int wizard_id = pop();
					push(get_health(wizard_id));
					break;
				}
				case INST_GET_WISDOM:
				{
					int wizard_id = pop();
					push(get_wisdom(wizard_id));
					break;
				}
				case INST_GET_AGILITY:
				{
					int wizard_id = pop();
					push(get_agility(wizard_id));
					break;
				}

				default:
				{
					std::cerr <<"[error] hitting default. exiting...\n";
					exit(1);
				}

			}
		}
	}

	void push(int value)
	{
		// check for stack overflow (https://...)
		assert(stack_size < MAX_STACK_SIZE);
		stack[stack_size] = value;
		stack_size += 1;
	}
	int pop()
	{
		// make sure the stack is not empty.
		assert(stack_size > 0);
		int value = stack[stack_size -1];
		stack_size -= 1;
		return value;
	}
};



#endif //_INCLUDED_VM_
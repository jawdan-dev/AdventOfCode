#include <common/common.h>

typedef long long ValueType;

ValueType registerA, registerB, registerC;
#define INSTRUCTION_MAX 1000
ValueType instructions[INSTRUCTION_MAX];
int instructionCount = 0;

void runInstructions(const int instructionCounter, const int outputDepth) {
	if (instructionCounter >= instructionCount - 1) return;

	const ValueType opcode = instructions[instructionCounter];
	const ValueType operand = instructions[instructionCounter + 1];
	ValueType comboOperand = operand;
	switch (operand) {
		case 4: comboOperand = registerA; break;
		case 5: comboOperand = registerB; break;
		case 6: comboOperand = registerC; break;
	}

	switch (opcode) {
		case 0: { registerA = registerA >> comboOperand; } break;
		case 1: { registerB = registerB ^ operand; } break;
		case 2: { registerB = comboOperand & 0b111; } break;
		case 4: { registerB ^= registerC; } break;
		case 6: { registerB = registerA >> comboOperand; } break;
		case 7: { registerC = registerA >> comboOperand; } break;

		case 3: {
			if (registerA == 0) break;
			return runInstructions(operand, outputDepth);
		} break;

		case 5: {
			const ValueType output = comboOperand % 8;
			if (outputDepth) printf(",");
			printf("%lli", output);
			return runInstructions(instructionCounter + 2, outputDepth + 1);
		} break;
	}
	return runInstructions(instructionCounter + 2, outputDepth);
}

int main() {
	FileReader fileReader = FileReader_getInput();
	// Read registers.
	FileReader_readUntilDelim(&fileReader, "\n");
	FileReader_readNextLLI(&fileReader, &registerA);
	FileReader_readUntilDelim(&fileReader, "\n");
	FileReader_readNextLLI(&fileReader, &registerB);
	FileReader_readUntilDelim(&fileReader, "\n");
	FileReader_readNextLLI(&fileReader, &registerC);
	//
	FileReader_readUntilDelim(&fileReader, "\n");
	//
	FileReader_readUntilDelim(&fileReader, "\n");
	while (FileReader_readNextLLI(&fileReader, &instructions[instructionCount])) instructionCount++;
	//
	runInstructions(0, 0);
}
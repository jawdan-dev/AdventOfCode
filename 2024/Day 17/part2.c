#include <common/common.h>
// NOTE: This is a manually solved task.

typedef long long ValueType;

ValueType startRegisterA, startRegisterB, startRegisterC;
ValueType registerA, registerB, registerC;
#define INSTRUCTION_MAX 1000
ValueType instructions[INSTRUCTION_MAX];
int instructionCount = 0;

const bool runMatches(const int instructionCounter, const int outputDepth) {
	if (instructionCounter >= instructionCount - 1) return outputDepth == instructionCount;

	// Manual brute force goes here.
	//					v (fiddle with the threshold.)
	if (outputDepth >= 10)
		printf("%llx %llx %llx \n", startRegisterA, startRegisterB, startRegisterC);

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
			return runMatches(operand, outputDepth);
		} break;

		case 5: {
			const ValueType output = comboOperand % 8;
			if (outputDepth >= instructionCount || output != instructions[outputDepth]) return false;
			return runMatches(instructionCounter + 2, outputDepth + 1);
		} break;
	}
	return runMatches(instructionCounter + 2, outputDepth);
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
	FileReader_readUntilDelim(&fileReader, "\n");
	//
	FileReader_readUntilDelim(&fileReader, "\n");
	while (FileReader_readNextLLI(&fileReader, &instructions[instructionCount])) instructionCount++;
	//
	bool matched = false;
	for (ValueType i = 0; i < LONG_LONG_MAX && !matched; i++) {
		for (int j = 0; j <= 5 && !matched; j++) {
			switch (j) {
				// Brute force common table here.
				// Example:
				// case 0: startRegisterA = (i << 12) | 0xe6d; break;
				// case 1: startRegisterA = (i << 12) | 0xebf; break;
				// case 2: startRegisterA = (i << 20) | 0x94e6d; break;
				// case 3: startRegisterA = (i << 20) | 0x94ebf; break;
				// 								Do ^ until ~28th bit.
			}
			//
			registerA = startRegisterA;
			registerB = startRegisterB;
			registerC = startRegisterC;
			//
			matched = runMatches(0, 0);
		}
		if ((i % 10000000) == 0) printf("%lli\r", i);

		if (!matched) continue;
		printf("Matched at try %lli.\n", startRegisterA);
		printf("HEX: %llx.\n", startRegisterA);
		break;
	}
}
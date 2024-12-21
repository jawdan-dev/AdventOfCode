#include <common/common.h>
#include <common/point.h>
#include <common/direction.h>

typedef long long ValueType;

const Point getRobotKeyPosition(const char key) {
	switch (key) {
		case '<': return (Point) {
			.x = 0, .y = 1
		};
		case '^': return (Point) {
			.x = 1, .y = 0
		};
		case '>': return (Point) {
			.x = 2, .y = 1
		};
		case 'v': return (Point) {
			.x = 1, .y = 1
		};
		case 'A': return (Point) {
			.x = 2, .y = 0
		};
	}
	ASSERT(false, "Wrong keypad used!");
}
const Point getKeypadKeyPosition(const char key) {
	switch (key) {
		case '7': return (Point) {
			.x = 0, .y = 0
		};
		case '8': return (Point) {
			.x = 1, .y = 0
		};
		case '9': return (Point) {
			.x = 2, .y = 0
		};

		case '4': return (Point) {
			.x = 0, .y = 1
		};
		case '5': return (Point) {
			.x = 1, .y = 1
		};
		case '6': return (Point) {
			.x = 2, .y = 1
		};

		case '1': return (Point) {
			.x = 0, .y = 2
		};
		case '2': return (Point) {
			.x = 1, .y = 2
		};
		case '3': return (Point) {
			.x = 2, .y = 2
		};

		case '0': return (Point) {
			.x = 1, .y = 3
		};
		case 'A': return (Point) {
			.x = 2, .y = 3
		};
	}
	ASSERT(false, "Wrong keypad used!");
}

const bool pathOptimized(const char start, const char end, const bool isRobot) {
	if (isRobot) {
	switch (start) {
		case 'A': {
			switch (end) {
				case '<': return true;
			}
		}
		case '^': {
			switch (end) {
				case '<': return true;
			}
		} break;
		case '<': {
			switch (end) {
				case 'A': return true;
				case '^': return true;
			}
		} break;
	}
	} else {
		const Point startPos = getKeypadKeyPosition(start), endPos = getKeypadKeyPosition(end);
		const int cx = endPos.x - startPos.x, cy = endPos.y - startPos.y;
		return (endPos.x == 0 && startPos.y == 3) || (startPos.x == 0 && endPos.y == 3);
	}
	return false;
}
const char* pathDirectionalKeypad(const char start, const char end) {
	switch (start) {
		case 'A': {
			switch (end) {
				case '^': return "<A";
				case '<': return "v<<A"; //
				case 'v': return "<vA";
				case '>': return "vA";
			}
		} break;
		case '^': {
			switch (end) {
				case 'A': return ">A";
				case '<': return "v<A"; // 
				case 'v': return "vA";
				case '>': return "v>A";
			}
		} break;
		case '<': {
			switch (end) {
				case 'A': return ">>^A"; // 
				case '^': return ">^A"; //
				case 'v': return ">A";
				case '>': return ">>A";
			}
		} break;
		case 'v': {
			switch (end) {
				case 'A': return ">^A";
				case '^': return "^A";
				case '<': return "<A";
				case '>': return ">A";
			}
		} break;
		case '>': {
			switch (end) {
				case 'A': return "^A";
				case '^': return "^<A";
				case '<': return "<<A";
				case 'v': return "<A";
			}
		} break;
	}
	return "A";
}

const char* pathNumberKeypad(const char start, const char end) {
	const Point startPos = getKeypadKeyPosition(start), endPos = getKeypadKeyPosition(end);
	const int cx = endPos.x - startPos.x, cy = endPos.y - startPos.y;

	const char* horizontalPath, * verticalPath;
	switch (cy) {
		default: verticalPath = ""; break;
		case -3: verticalPath = "^^^"; break;
		case -2: verticalPath = "^^"; break;
		case -1: verticalPath = "^"; break;
		case 1: verticalPath = "v"; break;
		case 2: verticalPath = "vv"; break;
		case 3: verticalPath = "vvv"; break;
	}
	switch (cx) {
		default: horizontalPath = ""; break;
		case -2: horizontalPath = "<<"; break;
		case -1: horizontalPath = "<"; break;
		case 1: horizontalPath = ">"; break;
		case 2: horizontalPath = ">>"; break;
	}

	static char keyPadPath[8];
	if (endPos.x == 0 && startPos.y == 3) {
		// Vertical first.
		strcpy(keyPadPath, verticalPath);
		strcpy(keyPadPath + strlen(verticalPath), horizontalPath);
	} else {
		// Horizontal first.
		strcpy(keyPadPath, horizontalPath);
		strcpy(keyPadPath + strlen(horizontalPath), verticalPath);
	}
	const int pathLen = strlen(horizontalPath) + strlen(verticalPath);
	keyPadPath[pathLen] = 'A';
	keyPadPath[pathLen + 1] = '\0';
	return keyPadPath;
}

#define ROBOT_MAX 2
const ValueType getRobotPathLength(const char* input, const int robotDepth, const bool preOptimized) {
	if (robotDepth > ROBOT_MAX) return strlen(input);

	const int checkRobotDepth = robotDepth + 1;
	if (robotDepth == 0) {
		ValueType totalPathCost = getRobotPathLength(pathNumberKeypad('A', input[0]), checkRobotDepth, pathOptimized('A', input[0], false));
		for (int i = 1; i < strlen(input); i++)
			totalPathCost += getRobotPathLength(pathNumberKeypad(input[i - 1], input[i]), checkRobotDepth, pathOptimized(input[i - 1], input[i], false));
		return totalPathCost;
	}

	if (preOptimized) {
		ValueType totalPathCost = getRobotPathLength(pathDirectionalKeypad('A', input[0]), checkRobotDepth, pathOptimized('A', input[0], true));
		for (int i = 1; i < strlen(input); i++)
			totalPathCost += getRobotPathLength(pathDirectionalKeypad(input[i - 1], input[i]), checkRobotDepth, pathOptimized(input[i - 1], input[i], true));
		return totalPathCost;
	}

	int hx = 0, hy = 0;
	for (int i = 0; i < strlen(input); i++)
		switch (input[i]) {
			case '<': hx--; break;
			case '>': hx++; break;
			case '^': hy--; break;
			case 'v': hy++; break;
		}

	char xyPath[10], yxPath[10];
	for (int i = 0; i < abs(hx); i++) xyPath[i] = yxPath[abs(hy) + i] = hx < 0 ? '<' : '>';
	for (int i = 0; i < abs(hy); i++) xyPath[abs(hx) + i] = yxPath[i] = hy < 0 ? '^' : 'v';
	const int pathCount = abs(hx) + abs(hy);
	xyPath[pathCount] = yxPath[pathCount] = 'A';
	xyPath[pathCount + 1] = yxPath[pathCount + 1] = '\0';

	ValueType xyPathCost = 0, yxPathCost = 0;
	xyPathCost += getRobotPathLength(pathDirectionalKeypad('A', xyPath[0]), checkRobotDepth, pathOptimized('A', xyPath[0], true));
	yxPathCost += getRobotPathLength(pathDirectionalKeypad('A', yxPath[0]), checkRobotDepth, pathOptimized('A', yxPath[0], true));
	for (int i = 1; i <= pathCount; i++) {
		xyPathCost += getRobotPathLength(pathDirectionalKeypad(xyPath[i - 1], xyPath[i]), checkRobotDepth, pathOptimized(xyPath[i - 1], xyPath[i], true));
		yxPathCost += getRobotPathLength(pathDirectionalKeypad(yxPath[i - 1], yxPath[i]), checkRobotDepth, pathOptimized(yxPath[i - 1], yxPath[i], true));
	}

	return min(xyPathCost, yxPathCost);
}

int main() {
	FileReader fileReader = FileReader_getInput();
	ValueType total = 0;
	while (!fileReader.endReached) {
		FileReader_readUntilDelim(&fileReader, "\n");
		//
		ValueType value;
		FileReader_readNextLLI(&fileReader, &value);
		//
		const ValueType pathLength = getRobotPathLength(fileReader.readBuffer, 0, false);
		printf(" || %lli x %lli\n", pathLength, value);
		//
		total += pathLength * value;
	}
	printf("Pad cost: %lli\n", total);
}
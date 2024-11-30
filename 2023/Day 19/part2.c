// Load FileBuffer.
#define FILEBUFFER_READBUFFERSIZE 100
enum ReadState {
	ReadState_Line,
};
#include "../Common/FileBuffer.h"

typedef enum Category Category;
enum Category {
	Category_ExtremelyCoolLooking,
	Category_Musical,
	Category_Aerodynamic,
	Category_Shiny,

	Category_COUNT,
};

typedef enum Condition Condition;
enum Condition {
	Condition_None,
	Condition_LessThan,
	Condition_GreaterThan,
};

#define LABEL_MAX 20
#define CONDITION_MAX 10
#define WORKFLOW_MAX 600

typedef struct Part Part;
struct Part {
	size_t partRatings[Category_COUNT];
};

typedef struct Workflow_Condition Workflow_Condition;
struct Workflow_Condition {
	char label[LABEL_MAX];
	Category targetRating;
	size_t conditionValue;
	Condition conditionType;
};

typedef struct Workflow Workflow;
struct Workflow {
	char label[LABEL_MAX];
	Workflow_Condition conditions[CONDITION_MAX];
	size_t conditionCount;
};

const size_t findWorkflow(const Workflow* const workflows, const size_t workflowCount, const char* label);
const size_t getRangePossibilities(const Part minRemaining, const Part maxRemaining);
const size_t getMaxPossibilities(
	const Workflow* const workflows, const size_t workflowCount,
	const size_t currentWorkflow,
	Part minRemaining, Part maxRemaining
);


// Entry point.
int main() {
	// Create file buffer.
	struct FileBuffer fileBuffer;
	FileBuffer_OpenFile(&fileBuffer, "input.txt");

	// File delims.
	struct FileDelim delims[] = {
		{ '\n', ReadState_Line }
	};
	FileBuffer_SetDelims(&fileBuffer, delims, sizeof(delims) / sizeof(*delims), ReadState_Line);

	// Interpretation variables.
	Workflow workflows[WORKFLOW_MAX];
	size_t workflowCount = 0;
	bool readingRatings = false;

	// Interpret file.
	while (!readingRatings && FileBuffer_ReadUntilNextDelim(&fileBuffer)) {
		// Processing.
		switch (fileBuffer.readState) {
			case ReadState_Line: {
				if (strlen(fileBuffer.readBuffer) <= 0) {
					readingRatings = true;
					break;
				}

				// Remove last buffer character.
				fileBuffer.readBuffer[strlen(fileBuffer.readBuffer) - 1] = '\0';
				if (!readingRatings) {
					if (workflowCount >= WORKFLOW_MAX) {
						fprintf(stderr, "Workflow max exceeded.\n");
						abort();
					}

					// Read workflows.
					size_t labelLength = 0;
					while (labelLength < strlen(fileBuffer.readBuffer) && fileBuffer.readBuffer[labelLength] != '{') labelLength++;
					if (labelLength + 1 >= LABEL_MAX) {
						fprintf(stderr, "Label max exceeded.\n");
						abort();
					}

					// Get label.
					char label[LABEL_MAX];
					strncpy(label, fileBuffer.readBuffer, labelLength);
					label[labelLength] = '\0';

					// Update workflow details.
					Workflow* const currentWorkflow = &workflows[workflowCount];
					strcpy(currentWorkflow->label, label);
					currentWorkflow->conditionCount = 0;

					// Read values.
					char* valueStr = strtok(fileBuffer.readBuffer + labelLength + 1, ",");
					while (valueStr != nullptr) {
						if (currentWorkflow->conditionCount >= CONDITION_MAX) {
							fprintf(stderr, "Condition max exceeded.\n");
							abort();
						}


						// Read data.
						size_t conditionValue;
						char targetRating, condition = Condition_None;
						if (strlen(fileBuffer.readBuffer) > 1) {
							sscanf(valueStr, "%c%c%zu:", &targetRating, &condition, &conditionValue);
						}

						// Find final colon
						size_t labelOffset = 0;
						while (labelOffset < strlen(valueStr) && valueStr[labelOffset] != ':') labelOffset++;
						labelOffset++;
						if (labelOffset >= strlen(valueStr)) labelOffset = 0;

						// Read condition label.
						char conditionLabel[LABEL_MAX];
						sscanf(valueStr + labelOffset, "%s", conditionLabel);

						// Get condition.
						Workflow_Condition* const currentCondition =
							&currentWorkflow->conditions[currentWorkflow->conditionCount];

						// Set condition details.
						switch (targetRating) {
							case 'x': currentCondition->targetRating = Category_ExtremelyCoolLooking; break;
							case 'm': currentCondition->targetRating = Category_Musical; break;
							case 'a': currentCondition->targetRating = Category_Aerodynamic; break;
							case 's': currentCondition->targetRating = Category_Shiny; break;
						}
						switch (condition) {
							default: currentCondition->conditionType = Condition_None; break;
							case '<': currentCondition->conditionType = Condition_LessThan; break;
							case '>': currentCondition->conditionType = Condition_GreaterThan; break;
						}
						currentCondition->conditionValue = conditionValue;
						strcpy(currentCondition->label, conditionLabel);
						currentCondition->label[strlen(conditionLabel)] = '\0';

						// Update condition count.
						currentWorkflow->conditionCount++;

						// Increment.
						valueStr = strtok(nullptr, ",");
					}

					workflowCount++;
				}
			} break;
		}
	}

	// Min/max intial.
	Part minRemaining, maxRemaining;
	for (size_t i = 0; i < Category_COUNT; i++) {
		minRemaining.partRatings[i] = 1;
		maxRemaining.partRatings[i] = 4000;
	}

	// Get total possibilities.
	const size_t totalPossibilities = getMaxPossibilities(
		workflows, workflowCount,
		findWorkflow(workflows, workflowCount, "in"),
		minRemaining, maxRemaining
	);

	// Output answer.
	printf("   Max: %20.1zu\n", getRangePossibilities(minRemaining, maxRemaining));
	printf("Output: %20.1zu\n", totalPossibilities);
	return 0;
}

const size_t findWorkflow(const Workflow* const workflows, const size_t workflowCount, const char* label) {
	// Find label.
	for (size_t i = 0; i < workflowCount; i++)
		if (strcmp(label, workflows[i].label) == 0)
			return i;

	// No workflow with label found.
	printf("Failed to find workflow '%s'.\n", label);
	return SIZE_MAX;
}

const size_t getRangePossibilities(const Part minRemaining, const Part maxRemaining) {
	size_t totalRemainingPossibilities = 1;
	for (size_t i = 0; i < Category_COUNT; i++) {
		if (minRemaining.partRatings[i] >= maxRemaining.partRatings[i] + 1) {
			return 0;
		}
		totalRemainingPossibilities *=
			(maxRemaining.partRatings[i] + 1) - minRemaining.partRatings[i];
	}
	return totalRemainingPossibilities;
}
const size_t getMaxPossibilities(
	const Workflow* const workflows, const size_t workflowCount,
	const size_t currentWorkflow,
	Part minRemaining, Part maxRemaining
) {
	// Error check.
	if (currentWorkflow >= workflowCount) {
		fprintf(stderr, "Invalid workflow index given.\n");
		return 0;
	}

	size_t totalPossibilites = 0;
	const Workflow* const workflow = &workflows[currentWorkflow];
	for (size_t i = 0; i < workflow->conditionCount; i++) {
		const Workflow_Condition* const condition = &workflow->conditions[i];

		switch (condition->conditionType) {
			case Condition_None: {
				if (strcmp(condition->label, "A") == 0) {
					totalPossibilites += getRangePossibilities(minRemaining, maxRemaining);
				} else if (strcmp(condition->label, "R") != 0) {
					totalPossibilites +=
						getMaxPossibilities(
							workflows, workflowCount,
							findWorkflow(workflows, workflowCount, condition->label),
							minRemaining, maxRemaining
						);
				}
				minRemaining = maxRemaining;
			} return totalPossibilites;

			case Condition_LessThan: {
				if (minRemaining.partRatings[condition->targetRating] < condition->conditionValue) {
					// Get lowered max.
					Part tempMax = maxRemaining;
					if (tempMax.partRatings[condition->targetRating] > condition->conditionValue - 1)
						tempMax.partRatings[condition->targetRating] = condition->conditionValue - 1;

					if (strcmp(condition->label, "A") == 0) {
						totalPossibilites += getRangePossibilities(minRemaining, tempMax);
					} else if (strcmp(condition->label, "R") != 0) {
						// Add to total possibilities.
						totalPossibilites +=
							getMaxPossibilities(
								workflows, workflowCount,
								findWorkflow(workflows, workflowCount, condition->label),
								minRemaining, tempMax
							);

					}
					// Update remaining.
					minRemaining.partRatings[condition->targetRating] = condition->conditionValue;
				}
			} break;

			case Condition_GreaterThan: {
				if (maxRemaining.partRatings[condition->targetRating] > condition->conditionValue) {
					// Get lowered max.
					Part tempMin = minRemaining;
					if (tempMin.partRatings[condition->targetRating] < condition->conditionValue + 1)
						tempMin.partRatings[condition->targetRating] = condition->conditionValue + 1;

					if (strcmp(condition->label, "A") == 0) {
						totalPossibilites += getRangePossibilities(tempMin, maxRemaining);
					} else if (strcmp(condition->label, "R") != 0) {
						// Add to total possibilities.
						totalPossibilites +=
							getMaxPossibilities(
								workflows, workflowCount,
								findWorkflow(workflows, workflowCount, condition->label),
								tempMin, maxRemaining
							);

					}
					// Update remaining.
					maxRemaining.partRatings[condition->targetRating] = condition->conditionValue;
				}
			} break;
		}
	}

	return totalPossibilites;
}
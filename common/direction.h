#include <common/point.h>

typedef enum {
	Direction_None,

	Direction_North,
	Direction_East,
	Direction_South,
	Direction_West,

	Direction_Count,
} Direction;

const Direction Direction_flip(const Direction direction) {
	switch (direction) {
		case Direction_North: return Direction_South;
		case Direction_East: return Direction_West;
		case Direction_South: return Direction_North;
		case Direction_West: return Direction_East;
	}
	return Direction_None;
}
const Direction Direction_rotate(const Direction direction, const int steps) {
	Direction newDirection = direction;

	for (int i = 0; i < steps; i++) {
		switch (newDirection) {
			default: return Direction_None;

			case Direction_North: newDirection = Direction_East; break;
			case Direction_East: newDirection = Direction_South; break;
			case Direction_South: newDirection = Direction_West; break;
			case Direction_West: newDirection = Direction_North; break;
		}
	}
	return newDirection;
}
const unsigned char Direction_getFlag(const Direction direction) {
	switch (direction) {
		case Direction_North: return 0b1;
		case Direction_East: return 0b10;
		case Direction_South: return 0b100;
		case Direction_West: return 0b1000;
	}
	return 0;
}
const Point Direction_toPoint(const Direction direction) {
	switch (direction) {
		case Direction_North: return (Point) {
			.x = 0, .y = -1
		};
		case Direction_East: return (Point) {
			.x = 1, .y = 0
		};
		case Direction_South: return (Point) {
			.x = 0, .y = 1
		};
		case Direction_West: return (Point) {
			.x = -1, .y = 0
		};
	}
	return (Point) {
		.x = 0, .y = 0
	};
}
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
	if (direction == Direction_None)
		return Direction_None;

	return (Direction)((direction - 1) + steps % (Direction_Count - 1) + 1);
}
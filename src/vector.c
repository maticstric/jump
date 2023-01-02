#include "vector.h"

Vector2 vector2_add(Vector2 v1, Vector2 v2) {
    return (Vector2) { .x = v1.x + v2.x, .y = v1.y + v2.y };
}

#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    float x;
    float y;
} Vector2;

extern Vector2 vector2_add(Vector2 v1, Vector2 v2);

#endif

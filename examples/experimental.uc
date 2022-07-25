/* Vectors */
include stdio

struct Vector2
  int32 x
  int32 y
end

// Specifies that its a constructor and also mallocs (not sure abt this)
// The latter basically tells the compiler that this function MUST return
// a heap-allocated pointer. GCC compiler has this too if you look it up
@[Overload(Constructor), Malloc]
allocVector2: int32 x, int32 y -> struct Vector2* ... end

@[Overload(Add)] // Lets me do vec1 + vec2
addVector2: struct Vector2 vec1, struct Vector2 vec2 -> struct Vector2
  struct Vector2 result
  result.x = vec1.x + vec2.x
  result.y = vec1.y + vec2.y
  return result
end

// Not sure why some compilers have this, but it just tells it that this
@[Main] // function is the program's entry point. Just an example of the feature
bruhFunction: void -> int32
  struct Vector2* vec1 = Vector2(34, 210)   // Cool constructor B)
  struct Vector2* vec2 = Vector2(35, 210)
  struct Vector2* vec3 = &(vec1 + vec2)        // Operator overloading B)))

  println "(%d, %d) + (%d, %d) = (%d, %d)"
  \  vec1->x vec1->y     // Forgot which language continues lines like this,
  \  vec2->x vec2->y     // but I kinda like it.
  \  vec3->x vec3->y     // I know C macros and bash put the \ after the line
  free vec1. free vec2. free vec3. return 0
end

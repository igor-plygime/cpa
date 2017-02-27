# cpa

С89.

Using: `./normalize path`

Testing: `./normalize`

Memory allocated for 256 subdirectories in normalized path.

`char* normalize(const char* path, Level* levels)`

`levels` - auxiliary buffer allocated by client or NULL (will be allocated by function)

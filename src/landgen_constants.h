
const unsigned short SEA_LEVEL= 58;
const unsigned short SEABED_LEVEL= 48;
const unsigned short PLANE_LEVEL= 64;
const unsigned short MOUNTAIN_LEVEL= 88;

const float SEABED_AMPLITUDE= 12.0f;
const float PLANE_AMPLITUDE= 8.0f;
const float MOUNTAIN_AMPLITUDE= 50.0f;

//[ level - amplitude; level + amplitude ]   faster- [ level - amplitude/2; level + amplitude/2 ]
//perlin [ -1; 1]
//h= level + amplitude * perlin

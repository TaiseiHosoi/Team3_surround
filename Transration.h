#pragma once
class Transration
{public:

	float p[4][3] = {
	  {0.0f, 0.0f, 0.0f},
	  {0.0f, 0.0f, 0.0f},
	  {0.0f, 0.0f, 0.0f},
	  {0.0f, 0.0f, 0.0f}
	};


	
	float affinMove[3][3] = {
	  {1.0f, 0.0f, 10.0f},
	  {0.0f, 1.0f, 10.0f},
	  {0.0f, 0.0f, 1.0f  }
	};

	


	void TransCalculation(float *vert);
};


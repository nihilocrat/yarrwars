#if !defined SPACELIB_INCLUDED
#define SPACELIB_INCLUDED

//#include <string.h>
typedef char* string;

#define LEFT 1
#define RIGHT 2

GLfloat rad(GLfloat deg);

/*class spaceColor
{
	public:
		GLfloat r, g, b;

		spaceColor(){};
		spaceColor(GLfloat nr, GLfloat ng, GLfloat nb)
		{
			r = nr; g = ng; b = nb;
		};
};*/

class simpleShip
{
	public:	
		GLfloat x, y, xvel, yvel;
		GLfloat heading, impulse, turnrate, size;
		spaceColor color;

		simpleShip();
		simpleShip(GLfloat nx, GLfloat ny, GLfloat nhead, GLfloat nimp, GLfloat ntr, spaceColor ncolor);

		
		void draw();
		void turn(int direction);

		void move();
		void thrust();
};

//void spaceDrawSEO(string filename);

#endif // if !defined SPACELIB_INCLUDED


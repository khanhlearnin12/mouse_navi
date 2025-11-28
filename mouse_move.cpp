#include<iostream> 
#include <unistd.h>  
#include <fcntl.h> 
#include <sys/stat.h> 
#include <cmath>
using namespace std;

void mouseMove(int x, int y); 

int main (){
	int Height = GetSystemMatrics(SM_CYSCREEN);
	int Width = GetSystemMatrics(SM_CXSCREEN); 
	mouseMove(Width,Height); 
}

void mouseMove(int x, int y){
	int count = 800;
	int movex,movey;
	float angle = 0.0f;

	//set mouse as center screen 
	SetCursorPos(x/2,y/2); 
	
	//begin Spiral 
	for(int i = 0 ; i <= count ; i++){
		angle = .3*1;
		movex = (angle + cos(angle)) + x/2;
		movey = (angle + sin(angle)) + y/2;
		SetCursorPos(movex,movey);
		sleep(1);
	} 
} 

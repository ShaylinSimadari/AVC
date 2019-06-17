#include <iostream>
#include <cstdlib>
#include "E101.h"
 
using namespace std;

int W = 320;
int H = 240;
double whiteb; 
double blackb;
int t[7] = {90,90,-90,-90,90,-90,90};

//NEW VERSION

/*
 * opens gate, returns 1 if an error occurs. returns 0 otherwise
 * 
 */
int openGate(){
	char address[15]={'1','3','0','.','1','9','5','.','0','0','6','.','1','9','6'};
	char message[6]={'P','l','e','a','s','e'};
	int num1 = connect_to_server(address,1024);
	int num2 = send_to_server(message);
	int num3 = receive_from_server(message);
	int num4 = send_to_server(message);
	if(num1==0&&num2==0&&num3==0&&num4==0){
		return 0;
	}
	return 1;
}
void lookUp(){
	 set_motors(3,48-13);
	 hardware_exchange();
}
void lookDown(){
	 set_motors(3,48+17);
	 hardware_exchange();
}
void move(double speedL, double speedR){
     set_motors(1,48-speedR*1.384); 
	 set_motors(5,48+speedL);
	 hardware_exchange();
}
void stop(){
	move(0,0);
}
/*
 * draws a dot on the screen
 * 
 */
void dot(int y, int x,int r, int g, int b){
	if(x>0){
		set_pixel(y, x-1, r, g, b);
	}
	set_pixel(y, x, r, g, b);
	if(x<W-1){
		set_pixel(y, x+1, r, g, b);
	}
	if(y>0){
		set_pixel(y-1, x, r, g, b);
	}
	if(y<H-1){
		set_pixel(y+1, x, r, g, b);
	}
}
bool isRed(int y, int x){
	double thres = 3;
	double r = get_pixel(y,x,0);
	double g = get_pixel(y,x,1);
	double b = get_pixel(y,x,2);
	//cout<<r/g <<"  "<<r/b<<endl;
	if(r/g > thres && r/b > thres){
		return true;
	}
	return false;
}
bool onRed(){
	int i = 5;
	int numRed =0;
	for(int x = 0; x < W; x+=i){
		for(int y = 0; y < H; y+=i){
			if(isRed(y,x)){
				numRed++;
			}
		}
	}
	bool b = false;
	//cout<<numRed/(double)((W/i)*(H/i))<<endl;
	if(numRed/(double)((W/i)*(H/i))*100 > 30){
		b=true;
	}
	//cout< <"onRed: "<<b<<endl;
	return b;
}
bool atJunc(){
	int i = 5;
	int numBlack =0;
	for(int x = 0; x < W; x+=i){
		for(int y = 0; y < H; y+=i){
			if(get_pixel(y,x,3) < 50){
				numBlack++;
			}
		}
	}
	bool b = false;
	//cout<<numRed/(double)((W/i)*(H/i))<<endl;
	if(numBlack < 5){
		b=true;
	}
	//cout<<"isJ "<<b<<" nmb "<<numBlack<<endl;
	//cout<<"onRed: "<<b<<endl;
	return b;
}
/*
 * returns the darkest point in a line of pixels y
 * 
 */
int getDarkest(int scan){
	double maxdiff=0;  
	int lx=0;
	double darkest =1;
	take_picture();
	double prevb = (get_pixel(scan,0,0)+get_pixel(scan,0,1)+get_pixel(scan,0,2))/(double)(255*3);
	for(int x = 0; x< W; x++){
		
		double b = (get_pixel(scan,x,0)+get_pixel(scan,x,1)+get_pixel(scan,x,2))/(double)(255*3);
		if(b<darkest){
			darkest = b;
			lx=x;
		}
	}
	//dot(scan, lx, 255, 0, 0);
	
	update_screen();
	return lx;
}
double getDarkestB(int scan){
	double maxdiff=0;  
	int lx=0;
	double darkest =1;
	double prevb = (get_pixel(scan,0,0)+get_pixel(scan,0,1)+get_pixel(scan,0,2))/(double)(255*3);
	for(int x = 0; x< W; x++){
		
		double b = (get_pixel(scan,x,0)+get_pixel(scan,x,1)+get_pixel(scan,x,2))/(double)(255*3);
		if(b<darkest){
			darkest = b;
			lx=x;
		}
	}
	return darkest;
}
int getCentre(int scan){
	double maxdiff=0; 
	double mindiff = 0; 
	int lx=0;
	int rx=W-1;
	
	take_picture();
	//double prevb = (get_pixel(scan,0,0)+get_pixel(scan,0,1)+get_pixel(scan,0,2))/(double)(255*3);
	double prevb = get_pixel(scan,0,3)/(255.0);
	for(int x = 0; x< W; x++){
		//double b = (get_pixel(scan,x,0)+get_pixel(scan,x,1)+get_pixel(scan,x,2))/(double)(255*3);
		double b = get_pixel(scan,x,3)/(255.0);
		
		double diff = b-prevb;
		if(diff < 0.02 && diff > -0.02){
			diff = 0;
		}
		if(diff < mindiff){
			mindiff = diff;
			lx=x;
		}
		else if(diff > maxdiff){
			maxdiff = diff;
			rx=x;
		}
	//set_pixel(scan, x, 255, 255, 255);
	set_pixel(H/2.0-(b*100), x, 255, 255, 255);
	set_pixel(H/2.0-(diff*100), x, 255, 100, 100);
	cout<<diff<<endl;
	prevb=b;
	}
	dot(scan, lx, 0, 200, 255);
	dot(scan, rx, 0, 255, 0);
	dot(scan, (lx+rx)/2.0, 255, 255, 0);
	
	update_screen();
	return (lx+rx)/2.0;
}


int getCentre2(int scan){
	int lx=0;
	int rx=W-1;
	
	take_picture();
	int sx = getDarkest(scan);
	
	double prevb = get_pixel(scan,sx,3)/(255.0);
	for(int x = sx; x< W; x++){
		double b = get_pixel(scan,x,3)/(255.0);
		double diff = b-prevb;
		if(diff > 0.025 || diff < -0.025){
			rx=x;
			break;
		}
	set_pixel(H/2.0-(b*100), x, 255, 255, 255);
	set_pixel(H/2.0-(diff*100), x, 255, 100, 100);
	prevb=b;
	}
	prevb = get_pixel(scan,sx,3)/(255.0);
	for(int x = sx; x>0; x--){
		double b = get_pixel(scan,x,3)/(255.0);
		double diff = b-prevb;
		if(diff > 0.025 || diff < -0.025){
			lx=x;
			break;
		}
	set_pixel(H/2.0-(b*100), x, 255, 255, 255);
	set_pixel(H/2.0-(diff*100), x, 255, 100, 100);
	prevb=b;
	}
	
	dot(scan, lx, 0, 200, 255);
	dot(scan, rx, 0, 255, 0);
	dot(scan, (lx+rx)/2.0, 255, 255, 0);
	
	update_screen();
	return (lx+rx)/2.0;
}


void followLine(){
	int scan = H-1;
	int lx = getCentre2(scan);
	int dx = (W/2.0)-lx;
	double d = dx/(W/2.0);
	d = d*5; 
	//cout<<"distance from centre: "<<d<<endl;
	//cout<<"b: "<<getDarkestB(scan)<<endl;
	cout<<7.5 - d<<" | "<<7.5 + d<<endl;
	move(5 - d,5 + d);
}
void followLine1(){
	int scan = H-1;
	int lx = getCentre2(scan);
	int dx = (W/2.0)-lx;
	double d = dx/(W/2.0);
	d = d*6 ;   
	//cout<<"distance from centre: "<<d<<endl;
	//cout<<"b: "<<getDarkestB(scan)<<endl;
	if(d <-2){
		move(-d+1,0);
	}
	else if (d>2){
		move(0,  d+1);
	}else{
		move(4,4); 
	}
	//move(7.5 - d,7.5 + d);
}
void followLineM(){
	int scan = H-1;
	int lx = getCentre2(scan);
	int dx = (W/2.0)-lx;
	double d = dx/(W/2.0);
	d = d*6.5; 
	//cout<<"distance from centre: "<<d<<endl;
	//cout<<"b: "<<getDarkestB(scan)<<endl;
	if(d <-3){
		move(3,0);
	}
	else if (d>3){
		move(0,  3); 
	}else{
		move(6,6); 
	}
}

void followLineSlow(){
	int scan = H-1;
	int lx = getCentre2(scan);
	int dx = (W/2.0)-lx;
	double d = dx/(W/2.0);
	d = d*6.5;   
	//cout<<"distance from centre: "<<d<<endl;
	//cout<<"b: "<<getDarkestB(scan)<<endl;
	if(d <-1.5){
		if(d>-3){
			d=-3; 
		}
		move(-d+0.2,0);
		//cout<<"going Right"<<-d;
	}
	else if (d>1.5){
		if(d<3){
			d=3;
		}
		move(0,  d+0.2);
		//cout<<"going Left"<<d;
	}else{
		move(4,4); 
		//cout<<"going Straight";
	}
	//move(7.5 - d,7.5 + d);
}


void four(int scan){
	
	take_picture();
	for(int x = 0; x< W; x++){
		double r = get_pixel(scan,x,0)/(255.0);
		double g = get_pixel(scan,x,1)/(255.0);
		double b = get_pixel(scan,x,2)/(255.0);

	//set_pixel(scan, x, 255, 255, 255);
	set_pixel(scan-(r*100), x, 255, 0, 0);
	set_pixel(scan-(g*100), x, 0, 255, 0);
	set_pixel(scan-(b*100), x, 0, 0, 255);
	set_pixel(scan, x, 255, 255, 255);
	}
	
	update_screen();
}

void turnRight(){
	move(7,-7);
	 hardware_exchange();
	sleep1(475);
	stop();
	
}
void turnLeft(){
	move(-7,7);
	 hardware_exchange();
	sleep1(475);
	stop();
	
}
void turn(int deg){
	if(deg>0){
		move(7,-7);
	}else{
		move(-7,7);
	}
	 hardware_exchange();
	sleep1(abs(deg)*475/90.0);
	stop();
}
void forward(double cm){
	if(cm>0){
		move(10,10);
	}else{
		move(-10,-10);
	}
	 hardware_exchange();
	sleep1(abs((int)cm)*425/7.0);
	stop();
}
void maze(){
double d[7] = {27.5,20,19.2,13.7,19,21,17};//28.5
int t[7] = {90,90,-70,-70,90,-70,90};
	for(int i = 0; i < 7; i++){
		forward(d[i]); 
		sleep1(250);
		turn(t[i]);
		sleep1(250);
	}
}
void yee(){
	forward(40);
	turn(45);
	forward(20);
	turn(-90);
	forward(20); 
	turn(45);
	forward(40);
}
void dance(int sec){
	for(int i = 0 ; i < sec; i++){
		turn(45);
		lookUp();
		lookDown();
		turn(-45);
		lookUp();
		lookDown();
		sleep1(1000);
	}
	
}


int main()
{   
	int err;
	err = init(0);
	cout<<"After init() error="<<err<<endl;
	open_screen_stream();
	
	//Quad 1
	cout<<"QUADRANT 1"<<endl;
	stop(); 
	lookDown();
	openGate(); 
	
	sleep1(500);
	
	forward(5);
	
	//Quad 2 
	cout<<"QUADRANT 2"<<endl;
	for( int i = 0; i < 250  ; i++){
		//cout<<"i:"<<i<<endl;
		followLine1();
		sleep1(5);
		if(onRed()){
			stop();
			break;
		}
	}   
	cout<<"slowdown"<<endl;  
	for(int i = 0; i < 500  ; i++){
		//cout<<"i:"<<i+300<<endl;
		followLineSlow();
		sleep1(2);
		if(onRed()){
			stop();
			break;
		}
	}   
	
	sleep1(1000);
	forward(6);
	sleep1(1000);
	
	
	//Quad 3
	cout<<"QUADRANT 3"<<endl;
	int c = 0;
	for(int i = 0; i < 5; i++){
		for(int j = 0; j < 800; j++){
			followLineM();
			sleep1(5);
			if(atJunc()){
				cout<<"Junction: "<<c<<endl;
				stop();
				forward(-2);  
				sleep1(1500);
				turn(t[c]);
				c++;
				sleep1(1500);
				break;
			}
		}
	}
	/*
	cout<<"Special Junction: "<<c<<endl;
	forward(18);
	 
	for(int i = 0; i < 10; i++){
		cout<<i<<endl;
		followLineM ();
		sleep1(5);
	}
	forward(-2);
	sleep1(1500);
		cout<<"turn"<<endl;
	turn(-90);
	c++;  
	sleep1(1500);
	
	for(int i = 0; i < 1; i++){
		for(int j = 0; j < 300; j++){
			followLineM();
			sleep1(5);
			if(atJunc()){
				cout<<"Junction: "<<c<<endl;
				stop();
				forward(-2); 
				sleep1(1500);
				turn(t[c]);
				c++; 
				sleep1(1500);
				break;
			}
		}
	}
	for(int j = 0; j < 300; j++){
		followLineM();
		sleep1(5);
		if(atJunc()){
			cout<<"Junction: "<<c<<endl;
			stop();
			forward(-2); 
			sleep1(1500);
			c++; 
			sleep1(1500);
			break;
		}
	}
	*/ 
	cout<<"QUADRANT 4"<<endl;
	 
	/*
	maze();  
	for(int i = 0; i < 10  ; i++){
		followLine1();
		sleep1(5);
	}
	
	sleep1(1000);
	
	//Quad 4
	yee(); 
	
	move(-10,-10);
	hardware_exchange();
	sleep1(10*425/7.0);
	stop();
	 
	dance(5);
	*/
	//Stop
	stop();
	cout<<"COMPLETE"<<endl;
   
	close_screen_stream();
	stoph();
	return 0;
}
 

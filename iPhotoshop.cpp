/*

    Developed by->

        Md. Mehrab Haque
        Dept-> CSE
        Level-> 1, Term-> 1
        Student Id-> 1805001
        1805001@ugrad.cse.buet.ac.bd

        Rabib Jahin
        Dept-> CSE
        Level-> 1, Term-> 1
        Student Id-> 1805003
        1805003@ugrad.cse.buet.ac.bd

        Bangladesh University of Engineering and Technology
        (BUET)

        06.09.2019

*/

//****************Libraries****************
# include "iGraphics.h"
# include "gl.h"
# include<string.h>
#include<conio.h>
#include<stdio.h>
#include<stdlib.h>
#include<winuser.h>

//****************Macros for InsidePolygon() function****************
#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)
#define INSIDE 0
#define OUTSIDE 1

//****************Global variables****************
//********Archives for undo-redo, restore and blur function********
int archive[200][601][600][3],archiveLevel=1,archiveMax=1;//For undo-redo function
int blurArchive[50][600][600][3][1],blurArchiveLevel=0,blurArchiveMax=0,blurPixels[50][600][600];//For full and crop editing mode
int blurArchivePoints[50][360000][3],blurPixelPoints[50][360000];//For poly editing mode

//********Main image arrays, coordinates and dimensions********
int imgOriginal[600][600][3];//Initial image, used for restore option.
int img[600][600][3];//Image displayed.
int imgTmp[600][600][3];//Back-end image.
int realHeight,realWidth;//Image initial dimensions
int height,width,oriX,oriY;//Image realtime lower-left coordinates and dimensions

//****************Variables for tooltips and hover effects****************
char tooltip[100];
int tipX,tipY,tipWidth;
int colorX,colorY,mX,mY;
int hoverState=0;

//********Variables for polygon editing mode********
struct Point
{
    int x;
    int y;
};
Point poly[1000];
int vertices=0;
double polyX[1000],polyY[1000];

//********Intro video and background animation variables********
int intro=1,nSlide=0,frameDelayMillis=1;//Intro frames
int bgColor=0,bgSign=1,showCursor=1;//Background animation
int editorMode=2;//Editor theme

//********Start page variables********
int startPage=1,startPageMode=0,tutorialPage=0;

//********Import and save option variables********
int isOutput=0; //save
int importMode=0,importCounter=0; //import
char file_name[1000]; //File name while importing
char *loc="assets/images/img1.bmp"; //Deafult image filename
char outputName[100],outputFullName[104]; //Save filename

//********Mode and function state variables********
int mode=1;//mode
int s=0;//function state
int swapMode=1;//RGB-swap state
int knobPos=200;//Main slider knob-position

//********Crop editing mode and copy function variables********
//****Crop window lower-left coordinates
int startX,startY,cStartX,cStartY,gStartX,gStartY;
//Crop window heights and widths
int dx,dy,cDx,cDy,gDx,gDy;
//Crop window anchor point selector and previous coordinates
int selector=0,mouseX,mouseY;

//********Polygon editing mode states and variables********
int nPoints=0,points[360000][2];//Vertices of polygon.

//********Paint mode states and variables********
int isPainting=0;
int paintR,paintG,paintB;
int rPos=150,gPos=150,bPos=50,tPos=100,pickMode=0;
double radius=16;
int headerX,fontList=0;
int textMoveX,textMoveY,textMove=0;
int textInput=0,textX=0,textY=0,font=7,saveText=0;
char text[200];

//****************itoa() function definition****************
char *itoa(int num, char *str)
{
        if(str == NULL)
        {
                return NULL;
        }
        sprintf(str, "%d", num);
        return str;
}

//****************Function for importing a BMP file****************
void import(){
    radius=16,s=0,knobPos=200,mode=1,selector=0,vertices=0,nPoints=0,rPos=150,gPos=150,bPos=50,tPos=100,pickMode=0;
    AUX_RGBImageRec *TextureImage;
    TextureImage = auxDIBImageLoad(file_name);
    int i,j,k;
    int realwidth = TextureImage->sizeX;
    int realheight = TextureImage->sizeY;
    realwidth>596?width=596:width=realwidth;
    realheight>596?height=596:height=realheight;
    realHeight=height;
    realWidth=width;
    oriX=410-width/2;
    oriY=310-height/2;
    for(i=0;i<height;i++)
        for(j=0;j<width;j++)
            for(k=2;k>=0;k--){
                imgOriginal[i][j][k]=TextureImage->data[3*i*realwidth+3*j+k];
                img[i][j][k]=imgOriginal[i][j][k];
                archive[0][i][j][k]=imgOriginal[i][j][k];
                imgTmp[i][j][k]=imgOriginal[i][j][k];
            }
    free(TextureImage->data);
    free(TextureImage);
    textX=width/2;
    textY=height/2;
    startX=width*0.25;
    startY=height*0.25;
    dx=width*0.5;
    dy=height*0.5;
    gStartX=0;
    gStartY=0;
    gDx=width;
    gDy=height;
}

//Function for importing interface
void input(){
    int pos=0;
    system("cls");
    printf("\n\n\n\n");
    printf( "    -----------------------------------\n" );
    printf( "    -                                 -\n" );
    printf( "    -                                 -\n" );
    printf( "    -  Drag                           -\n" );
    printf( "    -         &                       -\n" );
    printf( "    -            Drop                 -\n" );
    printf( "    -                  BMP File Here  -\n" );
    printf( "    -                                 -\n" );
    printf( "    -                                 -\n" );
    printf( "    -----------------------------------\n" );
    int ch = _getch();
	    if( ch == '\"' ) {
			while( ( ch = _getch() ) != '\"' )
                file_name[pos++] = ch;
        }else{
			file_name [pos++] =ch;
			while( _kbhit() )
                file_name [pos++] =_getch();
        }
        system("cls");
	file_name[pos]='\0';
    int len=strlen(file_name);
    importMode=0;
    importCounter=0;
    if(file_name[len-4]=='.' && (file_name[len-3]=='b'||file_name[len-3]=='B') && (file_name[len-2]=='m' || file_name[len-2]=='M') && (file_name[len-1]=='p'||file_name[len-1]=='P')){
        printf("Image imported successfully\nNow go to the editor");
        import();
    }else
        MessageBox(0,"Import failed, try to import again. PLease do not import anything other than a valid BMP file.","Warning",0);
}

//****************Save BMP function****************
void outputBMP()
{
    typedef struct                       /**** BMP file header structure ****/
        {
        unsigned int   bfSize;           /* Size of file */
        unsigned short bfReserved1;      /* Reserved */
        unsigned short bfReserved2;      /* ... */
        unsigned int   bfOffBits;        /* Offset to bitmap data */
        } BITMAPFILEHEADER;

    typedef struct                       /**** BMP file info structure ****/
        {
        unsigned int   biSize;           /* Size of info header */
        int            biWidth;          /* Width of image */
        int            biHeight;         /* Height of image */
        unsigned short biPlanes;         /* Number of color planes */
        unsigned short biBitCount;       /* Number of bits per pixel */
        unsigned int   biCompression;    /* Type of compression to use */
        unsigned int   biSizeImage;      /* Size of image data */
        int            biXPelsPerMeter;  /* X pixels per meter */
        int            biYPelsPerMeter;  /* Y pixels per meter */
        unsigned int   biClrUsed;        /* Number of colors used */
        unsigned int   biClrImportant;   /* Number of important colors */
        } BITMAPINFOHEADER;
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
    if(height%4!=0)height-=height%4;
    if(width%4!=0)width-=width%4;
    /* Magic number for file. It does not fit in the header structure due to alignment requirements, so put it outside */
    unsigned short bfType=0x4d42;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfSize = 2+sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+640*480*3;
    bfh.bfOffBits = 0x36;
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;
    bih.biSizeImage = 0;
    bih.biXPelsPerMeter = 5000;
    bih.biYPelsPerMeter = 5000;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;
    FILE *file = fopen(outputFullName, "wb");
    if (!file)
        {
            printf("Could not write file\n");
            return;
        }
    PlaySound("assets/audio/click.wav",NULL,SND_ASYNC);
    /*Write headers*/
    fwrite(&bfType,1,sizeof(bfType),file);
    fwrite(&bfh, 1, sizeof(bfh), file);
    fwrite(&bih, 1, sizeof(bih), file);
    /*Write bitmap*/
    for (int y = 0; y<bih.biHeight; y++) /*Scanline loop backwards*/
        {
        for (int x = 0; x < bih.biWidth; x++) /*Column loop forwards*/
            {
            /*compute some pixel values*/
            unsigned char r = img[y][x][0];
            unsigned char g = img[y][x][1];
            unsigned char b = img[y][x][2];
            fwrite(&b, 1, 1, file);
            fwrite(&g, 1, 1, file);
            fwrite(&r, 1, 1, file);
            }
        }
    fclose(file);
    char command[256];
    char msg[1000];
    strcpy(msg,"Image saved as ");
    strcat(msg,outputFullName);
    strcat(msg," in the same folder.\nDo you want to open the image ?");
    int code=MessageBox(0,msg,"Saved",4);
    char cmd[1000];
    strcpy(cmd,"cmd /c ");
    strcat(cmd,outputFullName);
    if(code==6)system(cmd);
}
//****************This function determines if a point is inside a polygon, returns 0 if inside and 1 if outside.****************
int InsidePolygon(Point *polygon,int N,Point p)
{
  int counter = 0;
  int i;
  double xinters;
  Point p1,p2;
  p1 = polygon[0];
  for (i=1;i<=N;i++) {
    p2 = polygon[i % N];
    if (p.y > MIN(p1.y,p2.y)) {
      if (p.y <= MAX(p1.y,p2.y)) {
        if (p.x <= MAX(p1.x,p2.x)) {
          if (p1.y != p2.y) {
            xinters = (p.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
            if (p1.x == p2.x || p.x <= xinters)
              counter++;
          }
        }
      }
    }
    p1 = p2;
  }
  if (counter % 2 == 0)
    return(OUTSIDE);
  else
    return(INSIDE);
}
//****************This function saves those points inside a polygon in polygon editing mode****************
void polyBuilder(){
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            Point p;
            p.x=j+oriX;
            p.y=i+oriY;
            if(!InsidePolygon(poly,vertices,p)){
                points[nPoints][0]=j;
                points[nPoints][1]=i;
                nPoints++;
            }
        }
    }
}
////****************Archiving current state of image for undo-redo function****************
void archiving(){
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++)
            for(int k=0;k<3;k++)
                archive[archiveLevel][i][j][k]=imgTmp[i][j][k];
    archive[archiveLevel][600][0][0]=height;
    archive[archiveLevel][600][0][1]=width;
    if(archiveLevel==199)archiveLevel=0;
    archiveLevel++;
    if(archiveMax<archiveLevel)archiveMax=archiveLevel;
}
//****************Replicating display image to back-end image****************
void tmp(){
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++)
            for(int k=0;k<3;k++)
                imgTmp[i][j][k]=img[i][j][k];
}
//****************Replicating back-end image to display image****************
void revTmp(){
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++)
            for(int k=0;k<3;k++)
                img[i][j][k]=imgTmp[i][j][k];
}
//iGraphics iDraw() function
void iDraw(){
    if(isOutput==0){
    iClear();
        if(intro==0){
            if(startPage==0){
                if(importMode==0){
                    if(editorMode==1){
                        iSetColor(255,255,255);
                        iFilledRectangle(0,0,720,670);
                    }
                    for(int i=0;i<width;i++)
                        for(int j=0;j<height;j++){
                            iSetColor(img[j][i][0],img[j][i][1],img[j][i][2]);
                            iPoint(oriX+i,oriY+j,1);
                        }
                    iSetColor(255,255,255);
                    if(textInput==0){
                        if(editorMode==1)iShowBMP(10,10,"assets/icons/full.bmp");
                        else iShowBMP(10,10,"assets/icons/dark/full.bmp");
                        if(editorMode==1)iShowBMP(60,10,"assets/icons/partial.bmp");
                        else iShowBMP(60,10,"assets/icons/dark/partial.bmp");
                        if(editorMode==1)iShowBMP(10,60,"assets/icons/polygon.bmp");
                        else iShowBMP(10,60,"assets/icons/dark/polygon.bmp");
                        if(editorMode==1)iShowBMP(60,60,"assets/icons/paint.bmp");
                        else iShowBMP(60,60,"assets/icons/dark/paint.bmp");
                    }
                    if(editorMode==1){
                        if(mode==1)iSetColor(0,120,215);
                        else iSetColor(255,255,255);
                        iRectangle(10,10,40,40);
                        if(mode==2)iSetColor(0,120,215);
                        else iSetColor(255,255,255);
                        iRectangle(60,10,40,40);
                        if(mode==3)iSetColor(0,120,215);
                        else iSetColor(255,255,255);
                        iRectangle(10,60,40,40);
                        if(mode==4)iSetColor(0,120,215);
                        else iSetColor(255,255,255);
                        if(textInput==0)iRectangle(60,60,40,40);
                    }
                    else{
                        if(mode==1)iSetColor(255,255,255);
                        else iSetColor(0,0,0);
                        iRectangle(10,10,40,40);
                        if(mode==2)iSetColor(255,255,255);
                        else iSetColor(0,0,0);
                        iRectangle(60,10,40,40);
                        if(mode==3)iSetColor(255,255,255);
                        else iSetColor(0,0,0);
                        iRectangle(10,60,40,40);
                        if(mode==4)iSetColor(255,255,255);
                        else iSetColor(0,0,0);
                        if(textInput==0)iRectangle(60,60,40,40);
                    }
                    int y=140;
                    if(mode!=4){
                        int iconCode=16;
                        for(int i=0;i<8;i++){
                            if(iconCode==2){
                                if(editorMode==1)iShowBMP(60,y,"assets/icons/contrast.bmp");
                                else iShowBMP(60,y,"assets/icons/dark/contrast.bmp");
                            }
                            if(iconCode==4){
                                iSetColor(0,255,0);
                                if(editorMode==1)iSetColor(0,150,0);
                                iFilledCircle(80,y+20,15);
                            }
                            if(iconCode==6){
                                if(editorMode==1)iShowBMP(60,y,"assets/icons/bw.bmp");
                                else iShowBMP(60,y,"assets/icons/dark/bw.bmp");
                            }
                            if(iconCode==8){
                                if(editorMode==1)iShowBMP(60,y,"assets/icons/negative.bmp");
                                else iShowBMP(60,y,"assets/icons/dark/negative.bmp");
                            }
                            if(iconCode==10){
                                if(editorMode==1)iShowBMP(60,y,"assets/icons/xmirror.bmp");
                                else iShowBMP(60,y,"assets/icons/dark/xmirror.bmp");
                            }
                            if(iconCode==12){
                                if(editorMode==1)iShowBMP(60,y,"assets/icons/pixel.bmp");
                                else iShowBMP(60,y,"assets/icons/dark/pixel.bmp");
                            }
                            if(iconCode==14){
                                if(editorMode==1)iShowBMP(60,y,"assets/icons/cw.bmp");
                                else iShowBMP(60,y,"assets/icons/dark/cw.bmp");
                            }
                            if(iconCode==16){
                                if(editorMode==1)iShowBMP(60,y,"assets/icons/sketch.bmp");
                                else iShowBMP(60,y,"assets/icons/dark/sketch.bmp");
                            }
                            if(editorMode==1){
                                if(iconCode==s)iSetColor(0,120,215);
                                else iSetColor(255,255,255);
                                iRectangle(60,y,40,40);
                            }else{
                                if(iconCode==s)iSetColor(255,255,255);
                                else iSetColor(0,0,0);
                                iRectangle(60,y,40,40);
                            }
                            iconCode--;
                            if(iconCode==1){
                                if(editorMode==1)iShowBMP(10,y,"assets/icons/brightness.bmp");
                                else iShowBMP(10,y,"assets/icons/dark/brightness.bmp");
                            }
                            if(iconCode==3){
                                iSetColor(255,0,0);
                                iFilledCircle(30,y+20,15);
                            }
                            if(iconCode==5){
                                iSetColor(0,0,255);
                                if(editorMode==2)iSetColor(100,100,255);
                                iFilledCircle(30,y+20,15);
                            }
                            if(iconCode==7){
                                if(editorMode==1)iShowBMP(10,y,"assets/icons/crop.bmp");
                                else iShowBMP(10,y,"assets/icons/dark/crop.bmp");
                            }
                            if(iconCode==9){
                                if(editorMode==1)iShowBMP(10,y,"assets/icons/ymirror.bmp");
                                else iShowBMP(10,y,"assets/icons/dark/ymirror.bmp");
                            }
                            if(iconCode==11){
                                if(editorMode==1)iShowBMP(10,y,"assets/icons/blur.bmp");
                                else iShowBMP(10,y,"assets/icons/dark/blur.bmp");
                            }
                            if(iconCode==13){
                                if(editorMode==1)iShowBMP(10,y,"assets/icons/ccw.bmp");
                                else iShowBMP(10,y,"assets/icons/dark/ccw.bmp");
                            }
                            if(iconCode==15){
                                if(editorMode==1)iShowBMP(10,y,"assets/icons/rgbswap.bmp");
                                else iShowBMP(10,y,"assets/icons/dark/rgbswap.bmp");
                            }
                            if(editorMode==1){
                                if(iconCode==s)iSetColor(0,120,215);
                                else iSetColor(255,255,255);
                            }else{
                                if(iconCode==s)iSetColor(255,255,255);
                                else iSetColor(0,0,0);
                            }
                            iRectangle(10,y,40,40);
                            iconCode--;
                            y+=50;
                        }
                    }
                    else{
                        iSetColor(0,120,215);
                        if(editorMode==2)iSetColor(255,255,255);
                        iLine(17,120,17,320);
                        iLine(54,120,54,320);
                        iLine(91,120,91,320);
                        iLine(17,350,17,550);
                        iSetColor(0,120,215);
                        if(editorMode==2)iSetColor(255,255,255);
                        iCircle(72,380,30);
                        iSetColor(rPos*255/200,gPos*255/200,bPos*255/200);
                        iFilledCircle(72,380,radius);
                        iSetColor(255,255,255);
                        if(editorMode==2)iSetColor(0,0,0);
                        if(editorMode==1)iShowBMP(54,419,"assets/icons/picker.bmp");
                        else iShowBMP(54,419,"assets/icons/dark/picker.bmp");
                        if(pickMode==1){
                            iSetColor(0,120,215);
                            if(editorMode==2)iSetColor(255,255,255);
                        }
                        iRectangle(54,419,37,37);
                        if(textInput!=1){
                            if(editorMode==1){
                                iShowBMP(54,463,"assets/icons/eraser.bmp");
                                iShowBMP(54,513,"assets/icons/letter.bmp");
                            }
                            else{
                                iShowBMP(54,463,"assets/icons/dark/eraser.bmp");
                                iShowBMP(54,513,"assets/icons/dark/letter.bmp");
                            }
                        }
                        iSetColor(255,0,0);
                        iFilledCircle(17,120+rPos,8);
                        iSetColor(0,255,0);
                        if(editorMode==1)iSetColor(0,150,0);
                        iFilledCircle(54,120+gPos,8);
                        iSetColor(0,0,255);
                        if(editorMode==2)iSetColor(100,100,255);
                        iFilledCircle(91,120+bPos,8);
                        iSetColor(0,120,215);
                        if(editorMode==2) iSetColor(255,255,255);
                        iFilledCircle(17,350+tPos,8);
                    }
                    iSetColor(255,255,255);
                    if(editorMode==1)iShowBMP(60,620,"assets/icons/redo.bmp");
                    else iShowBMP(60,620,"assets/icons/dark/redo.bmp");
                    if(s==8 || s==15 || s==17 || s==18 || s==0 || s==9 || s==10 || s==13 || s==14 || mode==4){
                        if(editorMode==1)iShowBMP(160,620,"assets/icons/copy.bmp");
                        else iShowBMP(160,620,"assets/icons/dark/copy.bmp");
                        iSetColor(255,255,255);
                        if(editorMode==1) iSetColor(0,120,215);
                        if(s==17 || s==18) iRectangle(160,620,40,40);
                        if(s!=17 && s!=18){
                            if(editorMode==1){
                                iShowBMP(260,620,"assets/icons/lock.bmp");
                                iShowBMP(310,620,"assets/icons/lock.bmp");
                                iShowBMP(360,620,"assets/icons/key.bmp");
                                iShowBMP(410,620,"assets/icons/key.bmp");
                            }
                            else{
                                iShowBMP(260,620,"assets/icons/dark/lock.bmp");
                                iShowBMP(310,620,"assets/icons/dark/lock.bmp");
                                iShowBMP(360,620,"assets/icons/dark/key.bmp");
                                iShowBMP(410,620,"assets/icons/dark/key.bmp");
                            }
                        }
                        if(s!=17 && s!=18){
                            if(editorMode==1){
                                iShowBMP(520,620,"assets/icons/import.bmp");
                                iShowBMP(570,620,"assets/icons/save.bmp");
                            }
                            else{
                                iShowBMP(520,620,"assets/icons/dark/import.bmp");
                                iShowBMP(570,620,"assets/icons/dark/save.bmp");
                            }
                        }
                        if(editorMode==1){
                            iShowBMP(620,620,"assets/icons/restore.bmp");
                            iShowBMP(670,620,"assets/icons/exit.bmp");
                        }
                        else{
                            iShowBMP(620,620,"assets/icons/dark/restore.bmp");
                            iShowBMP(670,620,"assets/icons/dark/exit.bmp");
                        }
                    }
                    else{
                        if(s!=7 && s!=11){
                            iSetColor(0,120,215);
                            if(editorMode==2)iSetColor(255,255,255);
                            iLine(160,640,560,640);
                            iFilledCircle(knobPos+160,640,8);
                        }
                        if(s==11){
                            if(editorMode==1){
                                iShowBMP(370,620,"assets/icons/negative.bmp");
                                iShowBMP(320,620,"assets/icons/negative.bmp");
                                iSetColor(255,255,255);
                                iFilledRectangle(386,630,7,20);
                            }else{
                                iShowBMP(370,620,"assets/icons/dark/negative.bmp");
                                iShowBMP(320,620,"assets/icons/dark/negative.bmp");
                                iSetColor(0,0,0);
                                iFilledRectangle(386,630,7,20);
                            }
                        }
                        if(editorMode==1){
                            iShowBMP(620,620,"assets/icons/ok.bmp");
                            iShowBMP(670,620,"assets/icons/cancel.bmp");
                        }else{
                            iShowBMP(620,620,"assets/icons/dark/ok.bmp");
                            iShowBMP(670,620,"assets/icons/dark/cancel.bmp");
                        }
                    }
                    if(editorMode==1){
                        if(mode!=4)
                            iShowBMP(36,556,"assets/icons/moon.bmp");
                        else iShowBMP(36,570,"assets/icons/moon.bmp");
                    }
                    else{
                        iSetColor(0,0,0);
                        if(mode!=4){
                            iShowBMP(36,556,"assets/icons/light.bmp");
                            iLine(37,556,37,692);
                        }
                        else{
                            iShowBMP(36,570,"assets/icons/light.bmp");
                            iLine(37,570,37,606);
                        }
                    }
                    if(mode==4){
                        iSetColor(255,255,255);
                        if(editorMode==2)iSetColor(0,0,0);
                        iFilledRectangle(0,610,720,60);
                        for(int i=0;i<17;i++){
                            switch(i){
                                case 0:iSetColor(255,255,255);break;
                                case 1:iSetColor(200,200,200);break;
                                case 2:iSetColor(150,150,150);break;
                                case 3:iSetColor(100,100,100);break;
                                case 4:iSetColor(0,0,0);break;
                                case 5:iSetColor(104,72,0);break;
                                case 6:iSetColor(160,0,0);break;
                                case 7:iSetColor(255,0,0);break;
                                case 8:iSetColor(255,0,255);break;
                                case 9:iSetColor(226,134,38);break;
                                case 10:iSetColor(254,227,134);break;
                                case 11:iSetColor(255,255,0);break;
                                case 12:iSetColor(0,255,0);break;
                                case 13:iSetColor(0,255,255);break;
                                case 14:iSetColor(85,156,135);break;
                                case 15:iSetColor(0,0,255);break;
                                case 16:iSetColor(0,0,160);break;
                            }
                            iFilledRectangle(110+i*24,625,20,30);
                            iSetColor(0,120,215);
                            if(editorMode==2)iSetColor(255,255,255);
                            iRectangle(110+i*24,625,20,30);
                            if(textInput==0){
                                if(editorMode==1)iShowBMP(60,620,"assets/icons/redo.bmp");
                                else iShowBMP(60,620,"assets/icons/dark/redo.bmp");
                                if(editorMode==1){
                                    iShowBMP(520,620,"assets/icons/import.bmp");
                                    iShowBMP(570,620,"assets/icons/save.bmp");
                                }
                                else{
                                    iShowBMP(520,620,"assets/icons/dark/import.bmp");
                                    iShowBMP(570,620,"assets/icons/dark/save.bmp");
                                }
                            }
                            if(editorMode==1){
                                if(textInput==0)
                                    iShowBMP(620,620,"assets/icons/restore.bmp");
                                iShowBMP(670,620,"assets/icons/exit.bmp");
                            }
                            else{
                                if(textInput==0)
                                    iShowBMP(620,620,"assets/icons/dark/restore.bmp");
                                iShowBMP(670,620,"assets/icons/dark/exit.bmp");
                            }
                        }
                        if(textInput==1){
                            if(saveText==0){
                                if(editorMode==1)iSetColor(255,255,255);
                                else iSetColor(0,0,0);
                                iFilledRectangle(670,620,40,40);
                                iSetColor(255,255,255);
                                headerX=110+350;
                                iFilledRectangle(headerX+30,10+543+22,220,35);
                                iShowBMP(oriX+textX,oriY+textY+25,"assets/icons/move.bmp");
                                iSetColor(0,0,0);
                                iRectangle(headerX+35,10+543+27,100,25);
                                iShowBMP(headerX+116,10+543+35,"assets/icons/dropdown.bmp");
                                int fontX,fontY;
                                switch(font){
                                    case 1: fontX=headerX+45;
                                            fontY=10+543+34;
                                            iText(fontX,fontY,"Font",GLUT_BITMAP_8_BY_13);
                                            break;
                                    case 2: fontX=headerX+45;
                                            fontY=10+543+34;
                                            iText(fontX,fontY,"Font",GLUT_BITMAP_9_BY_15);
                                            break;
                                    case 3: fontX=headerX+45;
                                            fontY=10+543+35;
                                            iText(fontX,fontY,"Font",GLUT_BITMAP_TIMES_ROMAN_10);
                                            break;
                                    case 4: fontX=headerX+45;
                                            fontY=10+543+32;
                                            iText(fontX,fontY,"Font",GLUT_BITMAP_TIMES_ROMAN_24);
                                            break;
                                    case 5: fontX=headerX+45;
                                            fontY=10+543+35;
                                            iText(fontX,fontY,"Font",GLUT_BITMAP_HELVETICA_10);
                                            break;
                                    case 6: fontX=headerX+45;
                                            fontY=10+543+35;
                                            iText(fontX,fontY,"Font",GLUT_BITMAP_HELVETICA_12);
                                            break;
                                    case 7: fontX=headerX+45;
                                            fontY=10+543+32;
                                            iText(fontX,fontY,"Font",GLUT_BITMAP_HELVETICA_18);
                                            break;
                                }
                                iSetColor(0,0,0);
                                iRectangle(headerX+140,10+543+27,50,25);
                                iText(headerX+155,10+543+36,"Ok");
                                iSetColor(200,0,0);
                                iRectangle(headerX+195,10+543+27,50,25);
                                iText(headerX+196,10+543+36,"Cancel");
                                iSetColor(0,0,0);
                                if(showCursor==1)iSetColor(0,0,0);
                                else iSetColor(255,255,255);
                                if(strlen(text)==0)iLine(oriX+textX,oriY+textY,oriX+textX,oriY+textY+18);
                            }
                            switch(font){
                                case 1: iSetColor(rPos*255/200,gPos*255/200,bPos*255/200);
                                        iText(oriX+textX,oriY+textY,text,GLUT_BITMAP_8_BY_13);
                                        break;
                                case 2: iSetColor(rPos*255/200,gPos*255/200,bPos*255/200);
                                        iText(oriX+textX,oriY+textY,text,GLUT_BITMAP_9_BY_15);
                                        break;
                                case 3: iSetColor(rPos*255/200,gPos*255/200,bPos*255/200);
                                        iText(oriX+textX,oriY+textY,text,GLUT_BITMAP_TIMES_ROMAN_10);
                                        break;
                                case 4: iSetColor(rPos*255/200,gPos*255/200,bPos*255/200);
                                        iText(oriX+textX,oriY+textY,text,GLUT_BITMAP_TIMES_ROMAN_24);
                                        break;
                                case 5: iSetColor(rPos*255/200,gPos*255/200,bPos*255/200);
                                        iText(oriX+textX,oriY+textY,text,GLUT_BITMAP_HELVETICA_10);
                                        break;
                                case 6: iSetColor(rPos*255/200,gPos*255/200,bPos*255/200);
                                        iText(oriX+textX,oriY+textY,text,GLUT_BITMAP_HELVETICA_12);
                                        break;
                                case 7: iSetColor(rPos*255/200,gPos*255/200,bPos*255/200);
                                        iText(oriX+textX,oriY+textY,text,GLUT_BITMAP_HELVETICA_18);
                                        break;
                            }
                            if(fontList==1 && saveText==0){
                                iSetColor(255,255,255);
                                iFilledRectangle(491,365,160,210);
                                iSetColor(0,0,0);
                                iRectangle(491,365,160,210);
                                for(int i=1;i<7;i++)iLine(491,365+i*30,651,365+i*30);
                                iText(241+270,115+440,"Hello World !!!",GLUT_BITMAP_8_BY_13);
                                iText(241+260,115+412,"Hello World !!!",GLUT_BITMAP_9_BY_15);
                                iText(241+290,115+382,"Hello World !!!",GLUT_BITMAP_TIMES_ROMAN_10);
                                iText(241+255,115+347,"Hello World !!!",GLUT_BITMAP_TIMES_ROMAN_24);
                                iText(241+295,115+322,"Hello World !!!",GLUT_BITMAP_HELVETICA_10);
                                iText(241+285,115+292,"Hello World !!!",GLUT_BITMAP_HELVETICA_12);
                                iText(241+270,115+258,"Hello World !!!",GLUT_BITMAP_HELVETICA_18);
                            }
                        }
                    }
                    if(mode==2 || s==7 || s==17 || s==18){
                        if(s==18){
                            iSetColor(0,255,0);
                            iRectangle(cStartX+oriX,cStartY+oriY,cDx,cDy);
                        }
                        iSetColor(0,0,0);
                        iRectangle(startX+oriX,startY+oriY-1,dx,dy);
                        if(s==17 || s==18){
                            iSetColor(255,255,255);
                            iFilledRectangle(startX+oriX+dx/2-65,startY+oriY+dy+10,60,25);
                            iSetColor(0,120,215);
                            iRectangle(startX+oriX+dx/2-65,startY+oriY+dy+10,60,25);
                            iSetColor(0,120,215);
                            if(s==17)iText(startX+oriX+dx/2-50,startY+oriY+dy+20,"Copy");
                            else iText(startX+oriX+dx/2-55,startY+oriY+dy+20,"Paste");
                            iSetColor(255,255,255);
                            iFilledRectangle(startX+oriX+dx/2+5,startY+oriY+dy+10,60,25);
                            iSetColor(255,0,0);
                            iRectangle(startX+oriX+dx/2+5,startY+oriY+dy+10,60,25);
                            iText(startX+oriX+dx/2+10,startY+oriY+dy+20,"Cancel");
                        }
                        iSetColor(255,255,255);
                        if(s!=18){
                            iFilledCircle(startX+oriX,startY+oriY,5);
                            iFilledCircle(startX+oriX+dx,startY+oriY,5);
                            iFilledCircle(startX+oriX,startY+oriY+dy,5);
                            iFilledCircle(startX+oriX+dx,startY+oriY+dy,5);
                        }
                        iSetColor(0,0,0);
                        if(s!=18){
                            iCircle(startX+oriX,startY+oriY,5);
                            iCircle(startX+oriX+dx,startY+oriY,5);
                            iCircle(startX+oriX,startY+oriY+dy,5);
                            iCircle(startX+oriX+dx,startY+oriY+dy,5);
                        }
                        iLine(startX+oriX+dx*0.375,startY+oriY+dy/2,startX+oriX+dx*0.625,startY+oriY+dy/2);
                        iLine(startX+oriX+dx/2,startY+oriY+dy*0.375,startX+oriX+dx/2,startY+oriY+dy*0.625);
                    }
                    if(mode==3){
                        iSetColor(255,255,255);
                        for(int i=0;i<vertices;i++)
                            iFilledCircle(polyX[i],polyY[i],4);
                            iSetColor(0,0,0);
                        for(int i=0;i<vertices;i++)
                            iCircle(polyX[i],polyY[i],4);
                        iPolygon(polyX,polyY,vertices);
                    }
                    if(textInput==0){
                        if(editorMode==1)iShowBMP(10,620,"assets/icons/undo.bmp");
                        else iShowBMP(10,620,"assets/icons/dark/undo.bmp");
                    }
                    iSetColor(255,255,255);
                    iFilledRectangle(tipX-5,tipY-5,tipWidth,22);
                    iSetColor(0,0,0);
                    iText(tipX,tipY,tooltip,GLUT_BITMAP_HELVETICA_18);
                    if(pickMode==1){
                        if(colorX>0){
                                iSetColor(255,255,255);
                                iFilledRectangle(colorX,colorY,182,30);
                                int rgb[3];
                                iGetPixelColor(mX,mY,rgb);
                                iSetColor(rgb[0],rgb[1],rgb[2]);
                                iFilledRectangle(colorX+3,colorY+3,24,24);
                                iSetColor(0,0,0);
                                char colorStr[17]="rgb(",snum[4];
                                itoa(rgb[0],snum);
                                strcat(colorStr,snum);
                                strcat(colorStr,",");
                                itoa(rgb[1],snum);
                                strcat(colorStr,snum);
                                strcat(colorStr,",");
                                itoa(rgb[2],snum);
                                strcat(colorStr,snum);
                                strcat(colorStr,")");
                                iText(colorX+35,colorY+8,colorStr,GLUT_BITMAP_HELVETICA_18);
                            }
                    }
                }
                else{
                    iSetColor(255,255,255);
                    iFilledRectangle(0,0,720,670);
                    iShowBMP(220,420,"assets/icons/logo.bmp");
                    iShowBMP(40,170,"assets/icons/importmessage.bmp");
                    importCounter++;
                    if(importCounter==10)input();
                }
            }
            else{
                if(startPageMode==0){
                    double trapX[4],trapY[4];
                    trapX[0]=0;
                    trapY[0]=0;
                    trapX[1]=720;
                    trapY[1]=0;
                    trapX[2]=720;
                    trapY[2]=435;
                    trapX[3]=0;
                    trapY[3]=235;
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledRectangle(0,0,720,670);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledPolygon(trapX,trapY,4);
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledRectangle(0,0,200,670);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledCircle(300,-97,400);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledRectangle(520,0,200,670);
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledCircle(420,767,400);
                    iSetColor(255,255,255);
                    iFilledRectangle(185,35,350,600);
                    iSetColor(255,255,255);
                    iShowBMP(210,495,"assets/icons/logo.bmp");
                    if(hoverState==1)iShowBMP(220,395,"assets/icons/hover/startbtn.bmp");
                    else iShowBMP(220,395,"assets/icons/startbtn.bmp");
                    if(hoverState==2)iShowBMP(224,310,"assets/icons/hover/helpbtn.bmp");
                    else iShowBMP(224,310,"assets/icons/helpbtn.bmp");
                    if(hoverState==3)iShowBMP(249,225,"assets/icons/hover/aboutusbtn.bmp");
                    else iShowBMP(249,225,"assets/icons/aboutusbtn.bmp");
                    if(hoverState==4)iShowBMP(202,140,"assets/icons/hover/aboutprogrambtn.bmp");
                    else iShowBMP(202,140,"assets/icons/aboutprogrambtn.bmp");
                    if(hoverState==5)iShowBMP(282,55,"assets/icons/hover/exitbtn.bmp");
                    else iShowBMP(282,55,"assets/icons/exitbtn.bmp");




                }
                else if(startPageMode==1){
                    double trapX[4],trapY[4];
                    trapX[0]=0;
                    trapY[0]=0;
                    trapX[1]=720;
                    trapY[1]=0;
                    trapX[2]=720;
                    trapY[2]=435;
                    trapX[3]=0;
                    trapY[3]=235;
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledRectangle(0,0,720,670);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledPolygon(trapX,trapY,4);
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledRectangle(0,0,200,670);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledCircle(300,-97,400);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledRectangle(520,0,200,670);
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledCircle(420,767,400);
                    iSetColor(255,255,255);
                    iFilledRectangle(60,85,600,500);
                    iShowBMP(435,496,"assets/icons/logo_small.bmp");
                    if(hoverState==7)iShowBMP2(100,516,"assets/icons/hover/back.bmp",0);
                    else iShowBMP2(100,516,"assets/icons/back.bmp",0);
                    if(tutorialPage>0){
                        if(hoverState==8)iShowBMP2(100,110,"assets/icons/hover/previous.bmp",0);
                        else iShowBMP2(100,110,"assets/icons/previous.bmp",0);
                    }
                    if(tutorialPage<15){
                        if(hoverState==9)iShowBMP2(580,110,"assets/icons/hover/next.bmp",0);
                        else iShowBMP2(580,110,"assets/icons/next.bmp",0);
                    }
                    for(int i=0;i<16;i++){
                        int radius,color;
                        if(i==tutorialPage){
                            radius=8;
                            color=50;
                        }
                        else{
                            radius=5;
                            color=150;
                        }
                        iSetColor(color,color,color);
                        iFilledCircle(172+i*25,130,radius);
                    }
                    switch(tutorialPage){
                        case 0:
                            iShowBMP(80,200,"assets/icons/tutorial/img1.bmp");
                            iShowBMP(390,220,"assets/icons/tutorial/txt1.bmp");
                            break;
                        case 1:
                            iShowBMP(250,430,"assets/icons/tutorial/img2.bmp");
                            iShowBMP(110,180,"assets/icons/tutorial/txt2.bmp");
                            break;
                        case 2:
                            iShowBMP(250,180,"assets/icons/tutorial/txt3.bmp");
                            iShowBMP(110,280,"assets/icons/tutorial/img3.bmp");
                            break;
                        case 3:
                            iShowBMP(140,300,"assets/icons/tutorial/img4.bmp");
                            iShowBMP(230,235,"assets/icons/tutorial/txt4.bmp");
                            break;
                        case 4:
                            iShowBMP(120,180,"assets/icons/tutorial/img5.bmp");
                            iShowBMP(250,200,"assets/icons/tutorial/txt5.bmp");
                            break;
                        case 5:
                            iShowBMP(170,420,"assets/icons/tutorial/img6.bmp");
                            iShowBMP(170,220,"assets/icons/tutorial/txt6.bmp");
                            break;
                        case 6:
                            iShowBMP(90,200,"assets/icons/tutorial/img7.bmp");
                            iShowBMP(360,200,"assets/icons/tutorial/txt7.bmp");
                            break;
                        case 7:
                            iShowBMP(70,200,"assets/icons/tutorial/img8.bmp");
                            iShowBMP(330,180,"assets/icons/tutorial/txt8.bmp");
                            break;
                        case 8:
                            iShowBMP(310,420,"assets/icons/tutorial/img9.bmp");
                            iShowBMP(170,220,"assets/icons/tutorial/txt9.bmp");
                            break;
                        case 9:
                            iShowBMP(150,420,"assets/icons/tutorial/img10.bmp");
                            iShowBMP(170,220,"assets/icons/tutorial/txt10.bmp");
                            break;
                        case 10:
                            iShowBMP(130,180,"assets/icons/tutorial/img11.bmp");
                            iShowBMP(230,180,"assets/icons/tutorial/txt11.bmp");
                            break;
                        case 11:
                            iShowBMP(100,220,"assets/icons/tutorial/img12.bmp");
                            iShowBMP(180,250,"assets/icons/tutorial/txt12.bmp");
                            break;
                        case 12:
                            iShowBMP(240,360,"assets/icons/tutorial/img13.bmp");
                            iShowBMP(120,150,"assets/icons/tutorial/txt13.bmp");
                            break;
                        case 13:
                            iShowBMP(140,320,"assets/icons/tutorial/img14.bmp");
                            iShowBMP(230,200,"assets/icons/tutorial/txt14.bmp");
                            break;
                        case 14:
                            iShowBMP(100,220,"assets/icons/tutorial/img15.bmp");
                            iShowBMP(80,410,"assets/icons/tutorial/img16.bmp");
                            iShowBMP(310,170,"assets/icons/tutorial/txt15.bmp");
                            break;
                        case 15:
                            if(hoverState==6)iShowBMP(220,230,"assets/icons/hover/startbtn.bmp");
                            else iShowBMP(220,230,"assets/icons/startbtn.bmp");
                            iShowBMP(210,340,"assets/icons/tutorial/txt16.bmp");
                            break;
                    }
                }
                else if(startPageMode==2){
                    double trapX[4],trapY[4];
                    trapX[0]=0;
                    trapY[0]=0;
                    trapX[1]=720;
                    trapY[1]=0;
                    trapX[2]=720;
                    trapY[2]=435;
                    trapX[3]=0;
                    trapY[3]=235;
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledRectangle(0,0,720,670);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledPolygon(trapX,trapY,4);
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledRectangle(0,0,200,670);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledCircle(300,-97,400);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledRectangle(520,0,200,670);
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledCircle(420,767,400);
                    iSetColor(255,255,255);
                    iFilledRectangle(185,35,350,600);
                    iSetColor(255,255,255);
                    iShowBMP(210,495,"assets/icons/logo.bmp");
                    if(hoverState==10)iShowBMP2(340,55,"assets/icons/hover/back.bmp",0);
                    else iShowBMP2(340,55,"assets/icons/back.bmp",0);
                    iShowBMP(195,125,"assets/icons/tutorial/aboutus.bmp");
                }
                else if(startPageMode==3){
                    double trapX[4],trapY[4];
                    trapX[0]=0;
                    trapY[0]=0;
                    trapX[1]=720;
                    trapY[1]=0;
                    trapX[2]=720;
                    trapY[2]=435;
                    trapX[3]=0;
                    trapY[3]=235;
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledRectangle(0,0,720,670);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledPolygon(trapX,trapY,4);
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledRectangle(0,0,200,670);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledCircle(300,-97,400);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledRectangle(520,0,200,670);
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledCircle(420,767,400);
                    iSetColor(255,255,255);
                    iFilledRectangle(185,35,350,600);
                    iSetColor(255,255,255);
                    iShowBMP(210,495,"assets/icons/logo.bmp");
                    if(hoverState==10)iShowBMP2(340,55,"assets/icons/hover/back.bmp",0);
                    else iShowBMP2(340,55,"assets/icons/back.bmp",0);
                    iShowBMP(200,140,"assets/icons/tutorial/aboutprogram.bmp");
                }
                else if(startPageMode==4){
                    double trapX[4],trapY[4];
                    trapX[0]=0;
                    trapY[0]=0;
                    trapX[1]=720;
                    trapY[1]=0;
                    trapX[2]=720;
                    trapY[2]=435;
                    trapX[3]=0;
                    trapY[3]=235;
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledRectangle(0,0,720,670);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledPolygon(trapX,trapY,4);
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledRectangle(0,0,200,670);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledCircle(300,-97,400);
                    iSetColor(255-bgColor,255-bgColor,255-bgColor);
                    iFilledRectangle(520,0,200,670);
                    iSetColor(bgColor,bgColor,bgColor);
                    iFilledCircle(420,767,400);
                    iSetColor(255,255,255);
                    iFilledRectangle(120,135,480,400);
                    iSetColor(255,255,255);
                    iShowBMP(260,400,"assets/icons/logo.bmp");
                    if(hoverState==11)iShowBMP2(150,440,"assets/icons/hover/back.bmp",0);
                    else iShowBMP2(150,440,"assets/icons/back.bmp",0);
                    if(hoverState==13)iShowBMP(170,285,"assets/icons/hover/imgbtn.bmp");
                    else iShowBMP(170,285,"assets/icons/imgbtn.bmp");
                    if(hoverState==12)iShowBMP(180,165,"assets/icons/hover/paintbtn.bmp");
                    else iShowBMP(180,165,"assets/icons/paintbtn.bmp");
                }
            }
        }
        else{
            char snum[5];
            itoa(nSlide,snum);
            char s[28] = "assets/intro/image";
            if(nSlide<10)strcat(s,"00000");
            else if(nSlide<100)strcat(s,"0000");
            else strcat(s,"000");
            strcat(s,snum);
            strcat(s,".bmp");
                iShowBMP(40,155,s);
                if(nSlide<320)nSlide++;
                else {
                    intro=0;
                    PlaySound("assets/audio/harp.wav", NULL, SND_LOOP | SND_ASYNC);
                }
                Sleep(frameDelayMillis);
        }
    }
    else{
        double trapX[4],trapY[4];
        trapX[0]=0;
        trapY[0]=0;
        trapX[1]=720;
        trapY[1]=0;
        trapX[2]=720;
        trapY[2]=435;
        trapX[3]=0;
        trapY[3]=235;
        iSetColor(bgColor,bgColor,bgColor);
        iFilledRectangle(0,0,720,670);
        iSetColor(255-bgColor,255-bgColor,255-bgColor);
        iFilledPolygon(trapX,trapY,4);
        iSetColor(bgColor,bgColor,bgColor);
        iFilledRectangle(0,0,200,670);
        iSetColor(255-bgColor,255-bgColor,255-bgColor);
        iFilledCircle(300,-97,400);
        iSetColor(255-bgColor,255-bgColor,255-bgColor);
        iFilledRectangle(520,0,200,670);
        iSetColor(bgColor,bgColor,bgColor);
        iFilledCircle(420,767,400);
        iSetColor(255,255,255);
        iFilledRectangle(210,235,300,200);
        iSetColor(47,174,53);
        iFilledRectangle(210,235,150,40);
        iSetColor(234,77,30);
        iFilledRectangle(360,235,150,40);
        iSetColor(255,255,255);
        iText(265,247,"Save", GLUT_BITMAP_HELVETICA_18);
        iText(402,247,"Cancel", GLUT_BITMAP_HELVETICA_18);
        if(showCursor==1)iSetColor(150,0,0);
        else iSetColor(255,255,255);
        iLine(232+strlen(outputName)*9,317,232+strlen(outputName)*9,339);
        if(strlen(outputName)>0){
            iSetColor(0,0,0);
            iText(230,325,outputFullName,GLUT_BITMAP_9_BY_15);
        }
        else{
            iSetColor(120,120,120);
            iText(230,325,"Enter text from keyboard ...",GLUT_BITMAP_9_BY_15);
        }
        iSetColor(47,174,53);
        iRectangle(225,313,275,30);
        iSetColor(6,131,248);
        iFilledRectangle(210,385,300,50);
        iSetColor(255,255,255);
        iText(220,400,"ENTER   FILE   NAME . . .", GLUT_BITMAP_TIMES_ROMAN_24);
    }
}

//****************Brightness****************
void brightness(int x){
    knobPos=x;
    if(knobPos>=200){
        if((mode==1 || mode==2)){
            for(int i=gStartY;i<=gStartY+gDy;i++)
                for(int j=gStartX;j<=gStartX+gDx;j++)
                    for(int k=0;k<3;k++){
                        if(imgTmp[i][j][k]*(1+(x-200)*0.007)>=0 && imgTmp[i][j][k]*(1+(x-200)*0.007)<=255)
                        img[i][j][k]=imgTmp[i][j][k]*(1+(x-200)*0.007);
                    }
        }
        else{
            for(int i=0;i<nPoints;i++)
                for(int k=0;k<3;k++){
                    if(imgTmp[points[i][1]][points[i][0]][k]*(1+(x-200)*0.007)>=0 && imgTmp[points[i][1]][points[i][0]][k]*(1+(x-200)*0.007)<=255)
                    img[points[i][1]][points[i][0]][k]=imgTmp[points[i][1]][points[i][0]][k]*(1+(x-200)*0.007);
                }
        }
    }else{
        if((mode==1 || mode==2))
            for(int i=gStartY;i<=gStartY+gDy;i++)
                for(int j=gStartX;j<=gStartX+gDx;j++)
                    for(int k=0;k<3;k++)
                        img[i][j][k]=imgTmp[i][j][k]-((imgTmp[i][j][k])*(200-x)/280);
        else
            for(int i=0;i<nPoints;i++)
                for(int k=0;k<3;k++)
                    img[points[i][1]][points[i][0]][k]=imgTmp[points[i][1]][points[i][0]][k]-((imgTmp[points[i][1]][points[i][0]][k])*(200-x)/280);
    }
}

//****************Contrast****************
void contrast(int x){
    knobPos=x;
    if(knobPos>=200){
        if((mode==1 || mode==2)){
            int contrast=x-200;
            float factor = (259.0 * (contrast + 255.0)) / (255.0 * (259.0 - contrast));
            for(int i=gStartY;i<=gStartY+gDy;i++){
                for(int j=gStartX;j<=gStartX+gDx;j++){
                    for(int k=0;k<3;k++){
                        if(factor*(imgTmp[i][j][k]-128)+128>=0 && factor*(imgTmp[i][j][k]-128)+128<=255)
                            img[i][j][k]=factor*(imgTmp[i][j][k]-128)+128;
                    }
                }
            }
        }
        else{
            int contrast=x-200;
            float factor = (259.0 * (contrast + 255.0)) / (255.0 * (259.0 - contrast));
            for(int i=0;i<nPoints;i++){
                for(int k=0;k<3;k++){
                        if(factor*(imgTmp[points[i][1]][points[i][0]][k]-128)+128>=0 && factor*(imgTmp[points[i][1]][points[i][0]][k]-128)+128<=255)
                    img[points[i][1]][points[i][0]][k]=factor*(imgTmp[points[i][1]][points[i][0]][k]-128)+128;
                }
            }
        }
    }else{
        if((mode==1 || mode==2)){
            int contrast=x-200;
            float factor = (259.0 * (contrast + 255.0)) / (255.0 * (259.0 - contrast));
            for(int i=gStartY;i<=gStartY+gDy;i++){
                for(int j=gStartX;j<=gStartX+gDx;j++){
                    for(int k=0;k<3;k++){
                        if(factor*(imgTmp[i][j][k]-128)+128>=0 && factor*(imgTmp[i][j][k]-128)+128<=255)
                        img[i][j][k]=factor*(imgTmp[i][j][k]-128)+128;
                    }
                }
            }
        }
        else{
            int contrast=x-200;
            float factor = (259.0 * (contrast + 255.0)) / (255.0 * (259.0 - contrast));
            for(int i=0;i<nPoints;i++){
                for(int k=0;k<3;k++){
                        if(factor*(imgTmp[points[i][1]][points[i][0]][k]-128)+128>=0 && factor*(imgTmp[points[i][1]][points[i][0]][k]-128)+128<=255)
                    img[points[i][1]][points[i][0]][k]=factor*(imgTmp[points[i][1]][points[i][0]][k]-128)+128;
                }
            }
        }
    }
}

//****************Red-ness****************
void red(int x){
    knobPos=x;
    if(knobPos>=200){
        if(mode==1 || mode==2){
            for(int i=gStartY;i<=gStartY+gDy;i++){
                for(int j=gStartX;j<=gStartX+gDx;j++){
                    img[i][j][0]=imgTmp[i][j][0]+((255-imgTmp[i][j][0])*(x-200)/200);
                }
            }
        }
        else{
            for(int i=0;i<nPoints;i++){
                img[points[i][1]][points[i][0]][0]=imgTmp[points[i][1]][points[i][0]][0]+((255-imgTmp[points[i][1]][points[i][0]][0])*(x-200)/200);
            }
        }
    }else{
        if(mode==1 || mode==2){
            for(int i=gStartY;i<=gStartY+gDy;i++){
                for(int j=gStartX;j<=gStartX+gDx;j++){
                        img[i][j][0]=imgTmp[i][j][0]-((imgTmp[i][j][0])*(200-x)/200);
                }
            }
        }else{
            for(int i=0;i<nPoints;i++)
                    img[points[i][1]][points[i][0]][0]=imgTmp[points[i][1]][points[i][0]][0]-imgTmp[points[i][1]][points[i][0]][0]*(200-x)/200;
        }
    }
}

//****************Green-ness****************
void green(int x){
    knobPos=x;
    if(knobPos>=200){
        if(mode==1 || mode==2){
            for(int i=gStartY;i<=gStartY+gDy;i++){
                for(int j=gStartX;j<=gStartX+gDx;j++){
                    img[i][j][1]=imgTmp[i][j][1]+((255-imgTmp[i][j][1])*(x-200)/200);
                }
            }
        }
        else{
            for(int i=0;i<nPoints;i++){
                img[points[i][1]][points[i][0]][1]=imgTmp[points[i][1]][points[i][0]][1]+((255-imgTmp[points[i][1]][points[i][0]][1])*(x-200)/200);
            }
        }
    }else{
        if(mode==1 || mode==2){
            for(int i=gStartY;i<=gStartY+gDy;i++){
                for(int j=gStartX;j<=gStartX+gDx;j++){
                    img[i][j][1]=imgTmp[i][j][1]-((imgTmp[i][j][1])*(200-x)/200);
                }
            }
        }else{
            for(int i=0;i<nPoints;i++)
                img[points[i][1]][points[i][0]][1]=imgTmp[points[i][1]][points[i][0]][1]-imgTmp[points[i][1]][points[i][0]][1]*(200-x)/200;
        }
    }
}

//****************Blue-ness****************
void blue(int x){
    knobPos=x;
    if(knobPos>=200){
        if(mode==1 || mode==2){
            for(int i=gStartY;i<=gStartY+gDy;i++){
                for(int j=gStartX;j<=gStartX+gDx;j++){
                        img[i][j][2]=imgTmp[i][j][2]+((255-imgTmp[i][j][2])*(x-200)/200);
                }
            }
        }
        else{
            for(int i=0;i<nPoints;i++){
                img[points[i][1]][points[i][0]][2]=imgTmp[points[i][1]][points[i][0]][2]+((255-imgTmp[points[i][1]][points[i][0]][2])*(x-200)/200);
            }
        }
    }else{
        if(mode==1 || mode==2){
            for(int i=gStartY;i<=gStartY+gDy;i++){
                for(int j=gStartX;j<=gStartX+gDx;j++){
                        img[i][j][2]=imgTmp[i][j][2]-((imgTmp[i][j][2])*(200-x)/200);
                }
            }
        }else{
            for(int i=0;i<nPoints;i++)
                    img[points[i][1]][points[i][0]][2]=imgTmp[points[i][1]][points[i][0]][2]-imgTmp[points[i][1]][points[i][0]][2]*(200-x)/200;
        }
    }
}

//****************BW filter and color intensity****************
void bw(int x){
    knobPos=x;
    if(knobPos>=200){
        if(mode==1 ||mode==2){
            for(int i=gStartY;i<=gStartY+gDy;i++){
                for(int j=gStartX;j<=gStartX+gDx;j++){
                    float mean=(imgTmp[i][j][0]+imgTmp[i][j][1]+imgTmp[i][j][2])/3;
                    for(int k=0;k<3;k++)if(imgTmp[i][j][k]-((mean-imgTmp[i][j][k])*(x-200)/200)<=255 && imgTmp[i][j][k]-((mean-imgTmp[i][j][k])*(x-200)/200)>=0){
                        img[i][j][k]=imgTmp[i][j][k]-((mean-imgTmp[i][j][k])*(x-200)/200);
                    }
                }
            }
        }
        else{
             for(int i=0;i<nPoints;i++){
                float mean=(imgTmp[points[i][1]][points[i][0]][0]+imgTmp[points[i][1]][points[i][0]][1]+imgTmp[points[i][1]][points[i][0]][2])/3;
                for(int k=0;k<3;k++)if(imgTmp[points[i][1]][points[i][0]][k]-((mean-imgTmp[points[i][1]][points[i][0]][k])*(x-200)/200)<=255 && imgTmp[points[i][1]][points[i][0]][k]-((mean-imgTmp[points[i][1]][points[i][0]][k])*(x-200)/200)>=0){
                    img[points[i][1]][points[i][0]][k]=imgTmp[points[i][1]][points[i][0]][k]-((mean-imgTmp[points[i][1]][points[i][0]][k])*(x-200)/200);
                }
             }
        }
    }else{
        if(mode==1 || mode==2){
            for(int i=gStartY;i<=gStartY+gDy;i++){
                for(int j=gStartX;j<=gStartX+gDx;j++){
                    float mean=(imgTmp[i][j][0]+imgTmp[i][j][1]+imgTmp[i][j][2])/3;
                    for(int k=0;k<3;k++){
                        img[i][j][k]=imgTmp[i][j][k]+((mean-imgTmp[i][j][k])*(200-x)/200);
                    }
                }
            }
        }
        else{
            for(int i=0;i<nPoints;i++){
                float mean=(imgTmp[points[i][1]][points[i][0]][0]+imgTmp[points[i][1]][points[i][0]][1]+imgTmp[points[i][1]][points[i][0]][2])/3;
                for(int k=0;k<3;k++)if(imgTmp[points[i][1]][points[i][0]][k]-((mean-imgTmp[points[i][1]][points[i][0]][k])*(x-200)/200)<=255 && imgTmp[points[i][1]][points[i][0]][k]-((mean-imgTmp[points[i][1]][points[i][0]][k])*(x-200)/200)>=0){
                    img[points[i][1]][points[i][0]][k]=imgTmp[points[i][1]][points[i][0]][k]+((mean-imgTmp[points[i][1]][points[i][0]][k])*(200-x)/200);
                }
             }
        }
    }
}

//****************Variables for sketch function****************
typedef struct{
    int val;
    int index;
}num;
num biggest(int a,int b,int c){
    num res;
    if(a>b){
        if(a>c){
            res.val=a;
            res.index=0;
        }else{
            res.val=c;
            res.index=2;
        }
    }else{
        if(b>c){
            res.val=b;
            res.index=1;
        }
        else{
            res.val=c;
            res.index=2;
        }
    }
    return res;
}

//****************Sketch function****************
void sketch(int x){
    knobPos=x;
    if(mode==1 || mode==2){
        for(int i=gStartY;i<=gStartY+gDy;i++){
            for(int j=gStartX;j<=gStartX+gDx;j++){
                float mean=(imgTmp[i][j][0]+imgTmp[i][j][1]+imgTmp[i][j][2])/3;
                if(mean<x*255/400){
                    num maximum=biggest(imgTmp[i][j][0],imgTmp[i][j][1],imgTmp[i][j][2]);
                    img[i][j][0]=img[i][j][1]=img[i][j][2]=maximum.val/2;
                    img[i][j][maximum.index]=maximum.val;
                }else{
                    img[i][j][0]=img[i][j][1]=img[i][j][2]=200;
                }
            }
        }
    }
    else{
        for(int i=0;i<nPoints;i++){
            float mean=(imgTmp[points[i][1]][points[i][0]][0]+imgTmp[points[i][1]][points[i][0]][1]+imgTmp[points[i][1]][points[i][0]][2])/3;
            if(mean<20+x/3){
                num maximum=biggest(imgTmp[points[i][1]][points[i][0]][0],imgTmp[points[i][1]][points[i][0]][1],imgTmp[points[i][1]][points[i][0]][2]);
                img[points[i][1]][points[i][0]][0]=img[points[i][1]][points[i][0]][1]=img[points[i][1]][points[i][0]][2]=maximum.val/2;
                img[points[i][1]][points[i][0]][maximum.index]=maximum.val;
            }else{
                img[points[i][1]][points[i][0]][0]=img[points[i][1]][points[i][0]][1]=img[points[i][1]][points[i][0]][2]=200;
            }
         }
    }
}

//****************Y-Mirror****************
void yMirror(){
    for(int i=gStartY;i<=gStartY+gDy;i++){
        for(int j=gStartX;j<=gStartX+gDx;j++){
                for(int k=0;k<3;k++){
                    img[i][j][k]=imgTmp[i][2*gStartX+gDx-j][k];
                }
        }
    }
    archiving();
    tmp();
    s=0;
}

//****************X-Mirror****************
void xMirror(){
    for(int i=gStartY;i<=gStartY+gDy;i++){
        for(int j=gStartX;j<=gStartX+gDx;j++){
                for(int k=0;k<3;k++){
                    img[i][j][k]=imgTmp[2*gStartY+gDy-i][j][k];
                }
        }
    }
    archiving();
    tmp();
    s=0;
}

//****************RGB-Swap****************
void rgbSwap(){
    if(mode!=3){
        if(swapMode%6==1){
            for(int i=gStartY;i<gStartY+gDy;i++){
                for(int j=gStartX;j<gStartX+gDx;j++){
                    img[i][j][0]=imgTmp[i][j][0];
                    img[i][j][1]=imgTmp[i][j][2];
                    img[i][j][2]=imgTmp[i][j][1];
                }
            }
        }
        if(swapMode%6==2){
            for(int i=gStartY;i<gStartY+gDy;i++){
                for(int j=gStartX;j<gStartX+gDx;j++){
                    img[i][j][1]=imgTmp[i][j][1];
                    img[i][j][0]=imgTmp[i][j][2];
                    img[i][j][2]=imgTmp[i][j][0];
                }
            }
        }
        if(swapMode%6==3){
            for(int i=gStartY;i<gStartY+gDy;i++){
                for(int j=gStartX;j<gStartX+gDx;j++){
                    img[i][j][0]=imgTmp[i][j][0];
                    img[i][j][1]=imgTmp[i][j][2];
                    img[i][j][2]=imgTmp[i][j][1];
                }
            }
        }
        if(swapMode%6==4){
            for(int i=gStartY;i<gStartY+gDy;i++){
                for(int j=gStartX;j<gStartX+gDx;j++){
                    img[i][j][1]=imgTmp[i][j][1];
                    img[i][j][0]=imgTmp[i][j][2];
                    img[i][j][2]=imgTmp[i][j][0];
                }
            }
        }
        if(swapMode%6==5){
            for(int i=gStartY;i<gStartY+gDy;i++){
                for(int j=gStartX;j<gStartX+gDx;j++){
                    img[i][j][0]=imgTmp[i][j][0];
                    img[i][j][1]=imgTmp[i][j][2];
                    img[i][j][2]=imgTmp[i][j][1];
                }
            }
        }
        if(swapMode%6==0){
            for(int i=gStartY;i<gStartY+gDy;i++){
                for(int j=gStartX;j<gStartX+gDx;j++){
                    img[i][j][1]=imgTmp[i][j][1];
                    img[i][j][0]=imgTmp[i][j][2];
                    img[i][j][2]=imgTmp[i][j][0];
                }
            }
        }
    }else{
        if(swapMode%6==1){
            for(int i=0;i<nPoints;i++){
                img[points[i][1]][points[i][0]][0]=imgTmp[points[i][1]][points[i][0]][0];
                img[points[i][1]][points[i][0]][1]=imgTmp[points[i][1]][points[i][0]][2];
                img[points[i][1]][points[i][0]][2]=imgTmp[points[i][1]][points[i][0]][1];
            }
        }
        if(swapMode%6==2){
            for(int i=0;i<nPoints;i++){
                img[points[i][1]][points[i][0]][1]=imgTmp[points[i][1]][points[i][0]][1];
                img[points[i][1]][points[i][0]][0]=imgTmp[points[i][1]][points[i][0]][2];
                img[points[i][1]][points[i][0]][2]=imgTmp[points[i][1]][points[i][0]][0];
            }
        }
        if(swapMode%6==3){
            for(int i=0;i<nPoints;i++){
                img[points[i][1]][points[i][0]][0]=imgTmp[points[i][1]][points[i][0]][0];
                img[points[i][1]][points[i][0]][1]=imgTmp[points[i][1]][points[i][0]][2];
                img[points[i][1]][points[i][0]][2]=imgTmp[points[i][1]][points[i][0]][1];
            }
        }
        if(swapMode%6==4){
            for(int i=0;i<nPoints;i++){
                img[points[i][1]][points[i][0]][1]=imgTmp[points[i][1]][points[i][0]][1];
                img[points[i][1]][points[i][0]][0]=imgTmp[points[i][1]][points[i][0]][2];
                img[points[i][1]][points[i][0]][2]=imgTmp[points[i][1]][points[i][0]][0];
            }
        }
        if(swapMode%6==5){
            for(int i=0;i<nPoints;i++){
                img[points[i][1]][points[i][0]][0]=imgTmp[points[i][1]][points[i][0]][0];
                img[points[i][1]][points[i][0]][1]=imgTmp[points[i][1]][points[i][0]][2];
                img[points[i][1]][points[i][0]][2]=imgTmp[points[i][1]][points[i][0]][1];
            }
        }
        if(swapMode%6==0){
            for(int i=0;i<nPoints;i++){
                img[points[i][1]][points[i][0]][1]=imgTmp[points[i][1]][points[i][0]][1];
                img[points[i][1]][points[i][0]][0]=imgTmp[points[i][1]][points[i][0]][2];
                img[points[i][1]][points[i][0]][2]=imgTmp[points[i][1]][points[i][0]][0];
            }
        }
    }
    swapMode++;
    archiving();
    tmp();
}

//****************CW-Rotate****************
void cw(){
    archiving();
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            for(int k=0;k<3;k++){
                img[j][i][k]=imgTmp[i][width-j-1][k];
            }
        }
    }
    int tm=height;
    height=width;
    width=tm;
    oriX=410-width/2;
    oriY=310-height/2;
    startX=width*0.25;
    startY=height*0.25;
    dx=width*0.5;
    dy=height*0.5;
    gStartX=0;
    gStartY=0;
    gDx=width;
    gDy=height;
    textX=width/2;
    textY=height/2;
    tmp();
}

//****************CCW-Rotate****************
void ccw(){
    archiving();
    for(int i=0;i<height;i++){
        for(int j=0;j<width;j++){
            for(int k=0;k<3;k++){
                img[j][i][k]=imgTmp[height-i-1][j][k];
            }
        }
    }
    int tm=height;
    height=width;
    width=tm;
    oriX=410-width/2;
    oriY=310-height/2;
    startX=width*0.25;
    startY=height*0.25;
    dx=width*0.5;
    dy=height*0.5;
    gStartX=0;
    gStartY=0;
    gDx=width;
    gDy=height;
    textX=width/2;
    textY=height/2;
    tmp();
}

//****************Crop****************
void crop(){
    archiving();
    height=dy;
    width=dx;
    if(height%4!=0)height-=height%4;
    if(width%4!=0)width-=width%4;
    for(int i=0;i<dy;i++){
        for(int j=0;j<dx;j++){
            for(int k=0;k<3;k++){
                img[i][j][k]=imgTmp[i+startY][j+startX][k];
            }
        }
    }
    oriX=410-width/2;
    oriY=310-height/2;
    startX=width*0.25;
    startY=height*0.25;
    dx=width*0.5;
    dy=height*0.5;
    gStartX=0;
    gStartY=0;
    gDx=width;
    gDy=height;
    textX=width/2;
    textY=height/2;
    tmp();
    s=0;
}

//****************Negative****************
void negative(){
    if(mode!=3){
        for(int i=gStartY;i<gStartY+gDy;i++)
            for(int j=gStartX;j<gStartX+gDx;j++)
                for(int k=0;k<3;k++)
                    img[i][j][k]=255-img[i][j][k];
    }
    else{
        for(int i=0;i<nPoints;i++){
            for(int k=0;k<3;k++){
                img[points[i][1]][points[i][0]][k]=255-img[points[i][1]][points[i][0]][k];
            }
        }
    }
    s=0;
    archiving();
    tmp();
}

//****************Pixilette****************
void pixilette(int x){
    knobPos=x;
    int h=gDy*x/1600;
    int w=gDx*x/1600;
    int pixelAmount=w;
    if(h<w)pixelAmount=h;
    if(pixelAmount>0){
        for(int i=gStartY;i+pixelAmount<gStartY+gDy;i+=pixelAmount){
            for(int j=gStartX;j+pixelAmount<gStartX+gDx;j+=pixelAmount){
                int rSum=0,gSum=0,bSum=0,nP=0;
                for(int k=i;k<i+pixelAmount;k++){
                    for(int l=j;l<j+pixelAmount;l++){
                        rSum+=imgTmp[k][l][0];
                        gSum+=imgTmp[k][l][1];
                        bSum+=imgTmp[k][l][2];
                        nP++;
                    }
                }
                for(int k=i;k<i+pixelAmount;k++){
                    for(int l=j;l<j+pixelAmount;l++){
                        img[k][l][0]=rSum/nP;
                        img[k][l][1]=gSum/nP;
                        img[k][l][2]=bSum/nP;
                    }
                }
            }
        }
    }
}

//****************Encryption-Decryption****************
//****************Main Algorithm****************
void encDecAlgol(int div){
    int xTimes=width/div;
    int yTimes=height/div;
    for(int i=0;i<yTimes;i++){
        for(int j=0;j<xTimes;j++){
            for(int k=0;k<div;k++){
                for(int l=0;l<div;l++){
                    for(int m=0;m<3;m++){
                        img[div*i+k][div*j+l][m]=imgTmp[div*i+k][div*j+div-l-1][2-m];
                    }
                }
            }
        }
    }
    tmp();
    for(int i=0;i<yTimes;i++){
        for(int j=0;j<xTimes;j++){
            for(int k=0;k<div;k++){
                for(int l=0;l<div;l++){
                    for(int m=0;m<3;m++){
                        img[div*i+k][div*j+l][m]=imgTmp[div*i+div-k-1][div*j+l][2-m];
                    }
                }
            }
        }
    }
    tmp();
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++)
            for(int k=0;k<3;k++)
                img[i][j][k]=255-imgTmp[i][j][k];
    tmp();
}

//****************Encryption-1****************
void enc1(){
    archiving();
    int minDim=width,limit;
    if (minDim>height)minDim=height;
    if(minDim>500)limit=minDim/10;
    else if(minDim>350)limit=minDim/5;
    else if(minDim>200)limit=minDim/3;
    else if(minDim>100)limit=minDim/2;
    else limit=minDim;
    for(int i=limit;i>=2;i--)encDecAlgol(i);
}

//****************Decryption-1****************
void dec1(){
    archiving();
    int minDim=width,limit;
    if (minDim>height)minDim=height;
    if(minDim>500)limit=minDim/10;
    else if(minDim>350)limit=minDim/5;
    else if(minDim>200)limit=minDim/3;
    else if(minDim>100)limit=minDim/2;
    else limit=minDim;
    for(int i=2;i<=limit;i++)encDecAlgol(i);
}

//****************Encryption-2****************
void enc2(){
    archiving();
    int minDim=width,limit;
    if (minDim>height)minDim=height;
    if(minDim>500)limit=minDim/9;
    else if(minDim>350)limit=minDim/8;
    else if(minDim>200)limit=minDim/7;
    else if(minDim>100)limit=minDim/6;
    else limit=minDim/4;
    for(int i=limit;i>=2;i--)encDecAlgol(i);
}

//****************Decryption-2****************
void dec2(){
    archiving();
    int minDim=width,limit;
    if (minDim>height)minDim=height;
    if(minDim>500)limit=minDim/9;
    else if(minDim>350)limit=minDim/8;
    else if(minDim>200)limit=minDim/7;
    else if(minDim>100)limit=minDim/6;
    else limit=minDim/4;
    for(int i=2;i<=limit;i++)encDecAlgol(i);
}


//****************iGraphics Mouse-Move function****************
void iMouseMove(int mx, int my)
{
    if(isOutput==0 && startPage==0){
        if(mx>=160 && mx<=560 && my>=620 && my<=660){
            if(s==1)brightness(mx-160);
            if(s==2)contrast(mx-160);
            if(s==3)red(mx-160);
            if(s==4)green(mx-160);
            if(s==5)blue(mx-160);
            if(s==6)bw(mx-160);
            if(s==12)pixilette(mx-160);
            if(s==16)sketch(mx-160);

        }
        if(mode==4){
            if(textMove==1){
                if(textX+mx-textMoveX>=0 && textX+mx-textMoveX<=width-24)
                    textX+=mx-textMoveX;
                if(textY+my-textMoveY>=0 && textY+my-textMoveY<=height-24)
                    textY+=my-textMoveY;
                textMoveX=mx;
                textMoveY=my;
            }
            if(mx>=9 && mx<=25 && my>=120 &&my<=320){
                rPos=my-120;
                pickMode=0;
            }
            if(mx>=46 && mx<=62 && my>=120 &&my<=320){
                gPos=my-120;
                pickMode=0;
            }
            if(mx>=83 && mx<=99 && my>=120 &&my<=320){
                bPos=my-120;
                pickMode=0;
            }
            if(mx>=9 && mx<=25 && my>=350 &&my<=550){
                tPos=my-350;
                radius=2+tPos*28/200;
                pickMode=0;
            }
            if(textInput==0 && pickMode==0 && mx-radius>=oriX && mx+radius<=oriX+width && my-radius>=oriY && my+radius<=oriY+height){
                isPainting=1;
                for(int i=my-oriY-radius;i<=my-oriY+radius;i++){
                    for(int j=mx-oriX-radius;j<=mx-oriX+radius;j++){
                        if((my-oriY-i)*(my-oriY-i)+(mx-oriX-j)*(mx-oriX-j)<=radius*radius){
                            img[i][j][0]=paintR;
                            img[i][j][1]=paintG;
                            img[i][j][2]=paintB;
                        }
                    }
                }
            }

        }
        if(s!=11 && (mode==2 || s==7 || s==17 || s==18)){
            if(selector==1 && mx>=oriX && mx<oriX+startX+dx && my>=oriY && my<oriY+startY+dy){
                dx-=mx-oriX-startX;
                dy-=my-oriY-startY;
                startX=mx-oriX;
                startY=my-oriY;
                if(mode==2){
                    gDx=dx;
                    gDy=dy;
                    gStartX=startX;
                    gStartY=startY;
                }
            }
            if(selector==2 && mx<oriX+width && mx>oriX+startX && my>=oriY && my<oriY+startY+dy){
                dx=mx-oriX-startX;
                dy-=my-oriY-startY;
                startY=my-oriY;
                if(mode==2){
                    gStartY=startY;
                    gDx=dx;
                    gDy=dy;
                }
            }
            if(selector==3 && mx>=oriX && mx<oriX+startX+dx && my>=oriY+startY && my<oriY+height){
                dx-=mx-oriX-startX;
                dy=my-oriY-startY;
                startX=mx-oriX;
                if(mode==2){
                    gStartX=startX;
                    gDx=dx;
                    gDy=dy;
                }
            }
            if(selector==4 && mx>=oriX+startX && mx<oriX+width && my>=oriY+startY && my<oriY+height){
                dx=mx-oriX-startX;
                dy=my-oriY-startY;
                if(mode==2){
                    gDx=dx;
                    gDy=dy;
                }
            }
            if(selector==5){
                int X,Y;
                if(startX+mx-mouseX>=0 && startX+mx+dx-mouseX<width) X=mx;
                else if(startX+mx-mouseX<0) X=mouseX-startX;
                else if(startX+mx+dx-mouseX>=width)X=width+mouseX-dx-startX;
                if(startY+my-mouseY>=0 && startY+my+dy-mouseY<height) Y=my;
                else if(startY+my-mouseY<0) Y=mouseY-startY;
                else if(startY+my+dy-mouseY>=height)Y=height+mouseY-dy-startY;
                startX+=X-mouseX;
                startY+=Y-mouseY;
                mouseX=X;
                mouseY=Y;
                if(mode==2){
                    gStartX=startX;
                    gStartY=startY;
                }
            }
        }
    }
}

//****************iGraphics mouse function****************
void iMouse(int button, int state, int mx, int my)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        if(startPage==0){
            if(isOutput==0){
                if(mode!=4 && mx>=36 && mx<=72 && my>=556 && my<=592){
                    if(editorMode==1)editorMode=2;
                    else editorMode=1;
                }
                if(mode==4 && mx>=36 && mx<=72 && my>=570 && my<=606){
                    if(editorMode==1)editorMode=2;
                    else editorMode=1;
                }
                if(textInput==0 && s==0 && mx>=670 && mx<=710 && my>=620 && my<=660){
                    startPage=1;
                    startPageMode=0;
                    PlaySound("assets/audio/harp.wav", NULL, SND_LOOP | SND_ASYNC);
                }
                if(mode!=4){
                    if(mx>=10 && mx<=50 && my>=490 && my<=530){knobPos=200;if(s!=0)revTmp();s=1;tmp();}
                    if(mx>=60 && mx<=100 && my>=490 && my<=530){knobPos=200;if(s!=0)revTmp();s=2;tmp();}
                    if(mx>=10 && mx<=50 && my>=440 && my<=480){knobPos=200;if(s!=0)revTmp();s=3;tmp();}
                    if(mx>=60 && mx<=100 && my>=440 && my<=480){knobPos=200;if(s!=0)revTmp();s=4;tmp();}
                    if(mx>=10 && mx<=50 && my>=390 && my<=430){knobPos=200;if(s!=0)revTmp();s=5;tmp();}
                    if(mx>=60 && mx<=100 && my>=390 && my<=430){knobPos=200;if(s!=0)revTmp();s=6;tmp();}
                    if(mx>=10 && mx<=50 && my>=340 && my<=380){
                        s=7;mode=1;gStartX=0;
                        gStartY=0;
                        gDx=width;
                        gDy=height;
                    }
                    if(mx>=60 && mx<=100 && my>=340 && my<=380){knobPos=200;if(s!=0)revTmp();s=8;negative();}
                    if(mx>=10 && mx<=50 && my>=290 && my<=330){
                        if(mode==3){
                            MessageBox(0,"Mirror can't be applied in Poly Mode. Enable Full or Crop mode to use mirror.","Warning",0);
                        }
                        else{
                            knobPos=200;
                            if(s!=0)revTmp();
                            s=9;
                            tmp();
                            yMirror();
                        }
                    }
                    if(mx>=60 && mx<=100 && my>=290 && my<=330){
                        if(mode==3)MessageBox(0,"Mirror can't be applied in Poly Mode. Enable Full or Crop mode to use mirror.","Warning",0);
                        else{knobPos=200;if(s!=0)revTmp();s=10;tmp();xMirror();}}
                        if(mx>=10 && mx<=50 && my>=240 && my<=280){knobPos=200;if(s!=0)revTmp();s=11;tmp();
                        blurArchiveLevel=0;
                        blurArchiveMax=0;
                        if(mode==1 || mode==2){
                            for(int i=gStartY;i<gStartY+gDy;i++)
                                for(int j=gStartX;j<gStartX+gDx;j++)
                                    for(int k=0;k<3;k++){
                                        blurArchive[0][i][j][k][0]=img[i][j][k];
                                        blurPixels[0][i][j]=1;
                                    }
                        }
                        if(mode==3){
                            for(int i=0;i<nPoints;i++){
                                for(int k=0;k<3;k++){
                                    blurArchivePoints[0][i][k]=img[points[i][1]][points[i][2]][k];
                                    blurPixelPoints[0][i]=1;
                                }
                            }
                        }
                    }
                    if(mx>=60 && mx<=100 && my>=240 && my<=280){
                        if(mode!=3){
                                if(s!=0)revTmp();s=12;tmp(); knobPos=0;
                        }
                        else{
                            MessageBox(0,"Pixilette can't be applied in Poly Mode. Enable Full or Crop mode to use pixilette.","Warning",0);
                        }
                    }
                    if(mx>=10 && mx<=50 && my>=190 && my<=230){knobPos=200;if(s!=0)s=13;ccw();}
                    if(mx>=60 && mx<=100 && my>=190 && my<=230){knobPos=200;if(s!=0)s=14;cw();}
                    if(mx>=10 && mx<=50 && my>=140 && my<=180){knobPos=200;if(s!=0)s=15;rgbSwap();}
                    if(mx>=60 && mx<=100 && my>=140 && my<=180){
                        if(s!=0)revTmp();
                        s=16;
                        tmp();
                        knobPos=0;
                    }
                    if(s==0 && mode!=4){
                        if(mx>=260 && mx<=300 && my>=620 && my<=660){
                            mode=1;
                            s=0;
                            enc1();
                            gStartX=0;
                            gStartY=0;
                            gDx=width;
                            gDy=height;
                        }
                        if(mx>=360 && mx<=400 && my>=620 && my<=660){
                            mode=1;
                            s=0;
                            dec1();
                            gStartX=0;
                            gStartY=0;
                            gDx=width;
                            gDy=height;
                        }
                        if(mx>=310 && mx<=350 && my>=620 && my<=660){
                            mode=1;
                            s=0;
                            enc2();
                            gStartX=0;
                            gStartY=0;
                            gDx=width;
                            gDy=height;
                        }
                        if(mx>=410 && mx<=450 && my>=620 && my<=660){
                            mode=1;
                            s=0;
                            dec2();
                            gStartX=0;
                            gStartY=0;
                            gDx=width;
                            gDy=height;
                        }
                    }
                    if(mode!=4 && s==0 && mx>=160 && mx<=200 && my>=620 && my<=660){
                        s=17;
                        mode=1;
                        startX=width*0.25;
                        startY=height*0.25;
                        dx=width*0.5;
                        dy=height*0.5;
                        gStartX=0;
                        gStartY=0;
                        gDx=width;
                        gDy=height;
                    }
                }
                if(s==11){
                    if(mx>=370 && mx<=410 && my>=620 && my<=660){
                        if(mode==1 || mode==2){
                            if(blurArchiveLevel<49)blurArchiveLevel++;
                            if(blurArchiveLevel>blurArchiveMax){
                                blurArchiveMax++;
                                for(int i=gStartY;i<gStartY+gDy;i++){
                                    for(int j=gStartX;j<gStartX+gDx;j++){
                                        for(int k=0;k<3;k++){
                                            blurArchive[blurArchiveLevel][i][j][k][0]=blurArchive[blurArchiveLevel-1][i][j][k][0];
                                        }
                                        blurPixels[blurArchiveLevel][i][j]=blurPixels[blurArchiveLevel-1][i][j];
                                    }
                                }
                                for(int i=gStartY;i<gStartY+gDy;i++){
                                    for(int j=gStartX;j<gStartX+gDx;j++){
                                        for(int k=j-blurArchiveLevel;k<=j+blurArchiveLevel;k++){
                                            if(k>=gStartX && k<gStartX+gDx && i-blurArchiveLevel>=gStartY){
                                                for(int l=0;l<3;l++){
                                                    blurArchive[blurArchiveLevel][i][j][l][0]+=imgTmp[i-blurArchiveLevel][k][l];
                                                }
                                                blurPixels[blurArchiveLevel][i][j]++;
                                            }
                                        }
                                        for(int k=j-blurArchiveLevel;k<=j+blurArchiveLevel;k++){
                                            if(k>=gStartX && k<gStartX+gDx && i+blurArchiveLevel<gStartY+gDy){
                                                for(int l=0;l<3;l++){
                                                    blurArchive[blurArchiveLevel][i][j][l][0]+=imgTmp[i+blurArchiveLevel][k][l];

                                                }
                                                blurPixels[blurArchiveLevel][i][j]++;
                                            }
                                        }
                                        for(int k=i-blurArchiveLevel+1;k<=i+blurArchiveLevel-1;k++){
                                            if(k>=gStartY && k<gStartY+gDy && j-blurArchiveLevel>=gStartX){
                                                for(int l=0;l<3;l++){
                                                    blurArchive[blurArchiveLevel][i][j][l][0]+=imgTmp[k][j-blurArchiveLevel][l];

                                                }
                                                blurPixels[blurArchiveLevel][i][j]++;
                                            }
                                        }
                                        for(int k=i-blurArchiveLevel+1;k<=i+blurArchiveLevel-1;k++){
                                            if(k>=gStartY && k<gStartY+gDy && j+blurArchiveLevel<gStartX+gDx){
                                                for(int l=0;l<3;l++){
                                                    blurArchive[blurArchiveLevel][i][j][l][0]+=imgTmp[k][j+blurArchiveLevel][l];

                                                }
                                                blurPixels[blurArchiveLevel][i][j]++;
                                            }
                                        }
                                        for(int k=0;k<3;k++)
                                            img[i][j][k]=blurArchive[blurArchiveLevel][i][j][k][0]/blurPixels[blurArchiveLevel][i][j];
                                    }
                                }
                            }else{
                                for(int i=gStartY;i<gStartY+gDy;i++){
                                    for(int j=gStartX;j<gStartX+gDx;j++){
                                        for(int k=0;k<3;k++)
                                            img[i][j][k]=blurArchive[blurArchiveLevel][i][j][k][0]/blurPixels[blurArchiveLevel][i][j];
                                    }
                                }
                            }
                        }else{
                            if(blurArchiveLevel<49)blurArchiveLevel++;
                            if(blurArchiveLevel>blurArchiveMax){
                                blurArchiveMax++;
                                for(int i=0;i<nPoints;i++){
                                    for(int k=0;k<3;k++){
                                        blurArchivePoints[blurArchiveLevel][i][k]=blurArchivePoints[blurArchiveLevel-1][i][k];
                                    }
                                    blurPixelPoints[blurArchiveLevel][i]=blurPixelPoints[blurArchiveLevel-1][i];
                                }
                                for(int i=0;i<nPoints;i++){
                                    for(int k=points[i][0]-blurArchiveLevel;k<=points[i][0]+blurArchiveLevel;k++){
                                        if(k>=0 && k<width && points[i][1]-blurArchiveLevel>=0){
                                            for(int l=0;l<3;l++){
                                                blurArchivePoints[blurArchiveLevel][i][l]+=imgTmp[points[i][1]-blurArchiveLevel][k][l];
                                            }
                                            blurPixelPoints[blurArchiveLevel][i]++;
                                        }
                                    }
                                    for(int k=points[i][0]-blurArchiveLevel;k<=points[i][0]+blurArchiveLevel;k++){
                                        if(k>=0 && k<width && points[i][1]+blurArchiveLevel<height){
                                            for(int l=0;l<3;l++){
                                                blurArchivePoints[blurArchiveLevel][i][l]+=imgTmp[points[i][1]+blurArchiveLevel][k][l];
                                            }
                                            blurPixelPoints[blurArchiveLevel][i]++;
                                        }
                                    }
                                    for(int k=points[i][1]-blurArchiveLevel+1;k<=points[i][1]+blurArchiveLevel-1;k++){
                                        if(k>=0 && k<height && points[i][0]-blurArchiveLevel>=0){
                                            for(int l=0;l<3;l++){
                                                blurArchivePoints[blurArchiveLevel][i][l]+=imgTmp[k][points[i][0]-blurArchiveLevel][l];
                                            }
                                            blurPixelPoints[blurArchiveLevel][i]++;
                                        }
                                    }
                                    for(int k=points[i][1]-blurArchiveLevel+1;k<=points[i][1]+blurArchiveLevel-1;k++){
                                        if(k>=0 && k<height && points[i][0]+blurArchiveLevel<width){
                                            for(int l=0;l<3;l++){
                                                blurArchivePoints[blurArchiveLevel][i][l]+=imgTmp[k][points[i][0]+blurArchiveLevel][l];
                                            }
                                            blurPixelPoints[blurArchiveLevel][i]++;
                                        }
                                    }
                                    for(int k=0;k<3;k++)
                                        img[points[i][1]][points[i][0]][k]=blurArchivePoints[blurArchiveLevel][i][k]/blurPixelPoints[blurArchiveLevel][i];
                                }
                            }else{
                                for(int i=0;i<nPoints;i++)
                                    for(int k=0;k<3;k++)
                                        img[points[i][1]][points[i][0]][k]=blurArchivePoints[blurArchiveLevel][i][k]/blurPixelPoints[blurArchiveLevel][i];
                            }
                        }
                    }
                    if(mx>=320 && mx<=360 && my>=620 && my<=660)
                        if(blurArchiveLevel>0){
                            blurArchiveLevel--;
                            if(mode==1 || mode==2){
                                for(int i=gStartY;i<gStartY+gDy;i++){
                                    for(int j=gStartX;j<gStartX+gDx;j++){
                                        for(int k=0;k<3;k++)
                                            img[i][j][k]=blurArchive[blurArchiveLevel][i][j][k][0]/blurPixels[blurArchiveLevel][i][j];
                                    }
                                }
                            }
                            else{
                                for(int i=0;i<nPoints;i++)
                                    for(int k=0;k<3;k++)
                                        img[points[i][1]][points[i][0]][k]=blurArchivePoints[blurArchiveLevel][i][k]/blurPixelPoints[blurArchiveLevel][i];
                            }
                        }
                }
                if(s==17){
                    if(mx>=startX+oriX+dx/2-65 && mx<=startX+oriX+dx/2-5 && my>=startY+oriY+dy+10 && my<=startY+oriY+dy+35){
                        cStartX=startX;
                        cStartY=startY;
                        cDx=dx;
                        cDy=dy;
                        if(startX+dx+20<width)startX+=20;
                        else if(startX>20)startX-=20;
                        if(startY>20)startY-=20;
                        else if(startY+dy+20<height)startY+=20;
                        s=18;
                    }
                    if(mx>=startX+oriX+dx/2+5 && mx<=startX+oriX+dx/2+65 && my>=startY+oriY+dy+10 && my<=startY+oriY+dy+35)s=0;
                }
                else if(s==18){
                    if(mx>=startX+oriX+dx/2-65 && mx<=startX+oriX+dx/2-5 && my>=startY+oriY+dy+10 && my<=startY+oriY+dy+35){
                        archiving();
                        for(int i=0;i<=dy;i++){
                            for(int j=0;j<=dx;j++){
                                for(int k=0;k<3;k++){
                                    img[i+startY][j+startX][k]=imgTmp[i+cStartY][j+cStartX][k];
                                }
                            }
                        }
                        tmp();
                        s=0;
                    }
                    if(mx>=startX+oriX+dx/2+5 && mx<=startX+oriX+dx/2+65 && my>=startY+oriY+dy+10 && my<=startY+oriY+dy+35)s=0;
                }
                if(textInput==0 && s==0 && mx>=620 && mx<=660 && my>=620 && my<=660){
                    height=realHeight;
                    width=realWidth;
                    for(int i=0;i<height;i++)
                        for(int j=0;j<width;j++)
                            for(int k=0;k<3;k++)
                                img[i][j][k]=imgOriginal[i][j][k];
                    oriX=410-width/2;
                    oriY=310-height/2;
                    startX=width*0.25;
                    startY=height*0.25;
                    dx=width*0.5;
                    dy=height*0.5;
                    gStartX=0;
                    gStartY=0;
                    gDx=width;
                    gDy=height;
                    gDy=height;
                    textX=width/2;
                    textY=height/2;
                    tmp();
                    s=0;
                    mode=1;
                }
                if(textInput==0 && s==0 && mx>=570 && mx<=610 && my>=620 && my<=660){
                    isOutput=1;
                    outputName[0]='\0';
                    outputFullName[0]='\0';
                    bgColor=0;
                    bgSign=1;
                }
                if(textInput==0 && s==0 && mx>=520 && mx<=560 && my>=620 && my<=660){
                    importMode=1;
                }
                if(s>0 && s!=9 && s!=10 && mx>=620 && mx<=660 && my>=620 && my<=660){
                    if(s!=7){
                        archiving();
                        tmp();
                        s=0;
                    }
                    else{
                        crop();
                    }
                }
                if(s>0 && s!=9 && s!=10 && mx>=670 && mx<=710 && my>=620 && my<=660){
                    revTmp();
                    s=0;
                }
                if(textInput==0 && mx>=10 && mx<=50 && my>=620 && my<=660){
                    archiving();
                    archiveLevel--;
                    if(archiveLevel>1){
                        archiveLevel--;
                        height=archive[archiveLevel][600][0][0];
                        width=archive[archiveLevel][600][0][1];
                        for(int i=0;i<height;i++)
                            for(int j=0;j<width;j++)
                                for(int k=0;k<3;k++)
                                    imgTmp[i][j][k]=archive[archiveLevel][i][j][k];
                    }
                    revTmp();
                    oriX=410-width/2;
                    oriY=310-height/2;
                    startX=width*0.25;
                    startY=height*0.25;
                    dx=width*0.5;
                    dy=height*0.5;
                    gStartX=0;
                    gStartY=0;
                    gDx=width;
                    gDy=height;
                    textX=width/2;
                    textY=height/2;
                    s=0;
                }
                if(textInput==0 && mx>=60 && mx<=100 && my>=620 && my<=660){
                    if(archiveLevel<archiveMax-1){
                        archiveLevel++;
                        height=archive[archiveLevel][600][0][0];
                        width=archive[archiveLevel][600][0][1];
                        for(int i=0;i<height;i++)
                            for(int j=0;j<width;j++)
                                for(int k=0;k<3;k++)
                                    imgTmp[i][j][k]=archive[archiveLevel][i][j][k];
                    }
                    revTmp();
                    oriX=410-width/2;
                    oriY=310-height/2;
                    startX=width*0.25;
                    startY=height*0.25;
                    dx=width*0.5;
                    dy=height*0.5;
                    gStartX=0;
                    gStartY=0;
                    gDx=width;
                    gDy=height;
                    gDy=height;
                    textX=width/2;
                    textY=height/2;
                    s=0;
                }
                if(textInput==0 && mx>=10 && mx<=50 && my>=10 && my<=50){
                    mode=1;
                    revTmp();
                    gStartX=0;
                    gStartY=0;
                    gDx=width;
                    gDy=height;
                    s=0;
                }
                if(textInput==0 && mx>=60 && mx<=100 && my>=10 && my<=50){
                    mode=2;
                    revTmp();
                    startX=width*0.25;
                    startY=height*0.25;
                    dx=width*0.5;
                    dy=height*0.5;
                    gStartX=startX;
                    gStartY=startY;
                    gDx=dx;
                    gDy=dy;
                    s=0;
                }
                if(textInput==0 && mx>=10 && mx<=50 && my>=60 && my<=100){
                    mode=3;
                    revTmp();
                    vertices=0;
                    s=0;
                }
                if(textInput==0 && mx>=60 && mx<=100 && my>=60 && my<=100){
                    mode=4;
                    revTmp();
                    rPos=150,gPos=150,bPos=50,tPos=100,pickMode=0,radius=16;
                    archiving();
                    tmp();
                    s=0;
                }
                if(mode==2 || s==7 || s==17 || s==18){
                    if(s!=18){
                        if(mx>=startX+oriX-10 && mx<= startX+oriX+10 && my>=startY+oriY-10 && my<=startY+oriY+10)selector=1;
                        else if(mx>=startX+oriX+dx-10 && mx<= startX+oriX+dx+10 && my>=startY+oriY-10 && my<=startY+oriY+10)selector=2;
                        else if(mx>=startX+oriX-10 && mx<= startX+oriX+10 && my>=startY+oriY+dy-10 && my<=startY+oriY+dy+10)selector=3;
                        else if(mx>=startX+oriX+dx-10 && mx<= startX+oriX+dx+10 && my>=startY+oriY+dy-10 && my<=startY+oriY+dy+10)selector=4;
                        else if(mx>startX+oriX+10 && mx< startX+oriX+dx-10 && my>startY+oriY-10 && my<startY+oriY+dy-10){
                            selector=5;
                            mouseX=mx;
                            mouseY=my;
                        }
                        else selector=0;
                    }
                    else{
                        if(mx>startX+oriX+10 && mx< startX+oriX+dx-10 && my>startY+oriY-10 && my<startY+oriY+dy-10){
                            selector=5;
                            mouseX=mx;
                            mouseY=my;
                        }
                        else selector=0;
                    }
                }
                if(mode==3){
                    if(s!=11 && mx>=oriX && mx<oriX+width && my>=oriY && my<oriY+height){
                        polyX[vertices]=mx;
                        polyY[vertices]=my;
                        Point p;
                        p.x=mx;
                        p.y=my;
                        poly[vertices]=p;
                        vertices++;
                        nPoints=0;
                        polyBuilder();
                    }
                }
                if(mode==4){
                    if(mx>=110+0*24 && mx<=130+0*24 && my>=625 && my<=655){
                        rPos=200;
                        gPos=200;
                        bPos=200;
                        pickMode=0;
                    }
                    if(mx>=110+1*24 && mx<=130+1*24 && my>=625 && my<=655){
                        rPos=200*200/255;
                        gPos=200*200/255;
                        bPos=200*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+2*24 && mx<=130+2*24 && my>=625 && my<=655){
                        rPos=150*200/255;
                        gPos=150*200/255;
                        bPos=150*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+3*24 && mx<=130+3*24 && my>=625 && my<=655){
                        rPos=100*200/255;
                        gPos=100*200/255;
                        bPos=100*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+4*24 && mx<=130+4*24 && my>=625 && my<=655){
                        rPos=0*200/255;
                        gPos=0*200/255;
                        bPos=0*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+5*24 && mx<=130+5*24 && my>=625 && my<=655){
                        rPos=104*200/255;
                        gPos=72*200/255;
                        bPos=0*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+6*24 && mx<=130+6*24 && my>=625 && my<=655){
                        rPos=160*200/255;
                        gPos=0*200/255;
                        bPos=0*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+7*24 && mx<=130+7*24 && my>=625 && my<=655){
                        rPos=255*200/255;
                        gPos=0*200/255;
                        bPos=0*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+8*24 && mx<=130+8*24 && my>=625 && my<=655){
                        rPos=255*200/255;
                        gPos=0*200/255;
                        bPos=255*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+9*24 && mx<=130+9*24 && my>=625 && my<=655){
                        rPos=226*200/255;
                        gPos=134*200/255;
                        bPos=38*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+10*24 && mx<=130+10*24 && my>=625 && my<=655){
                        rPos=254*200/255;
                        gPos=227*200/255;
                        bPos=134*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+11*24 && mx<=130+11*24 && my>=625 && my<=655){
                        rPos=255*200/255;
                        gPos=255*200/255;
                        bPos=0*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+12*24 && mx<=130+12*24 && my>=625 && my<=655){
                        rPos=0*200/255;
                        gPos=255*200/255;
                        bPos=0*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+13*24 && mx<=130+13*24 && my>=625 && my<=655){
                        rPos=0*200/255;
                        gPos=255*200/255;
                        bPos=255*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+14*24 && mx<=130+14*24 && my>=625 && my<=655){
                        rPos=85*200/255;
                        gPos=156*200/255;
                        bPos=135*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+15*24 && mx<=130+15*24 && my>=625 && my<=655){
                        rPos=0*200/255;
                        gPos=0*200/255;
                        bPos=255*200/255;
                        pickMode=0;
                    }
                    if(mx>=110+16*24 && mx<=130+16*24 && my>=625 && my<=655){
                        rPos=0*200/255;
                        gPos=0*200/255;
                        bPos=160*200/255;
                        pickMode=0;
                    }
                    if(mx>=54 && mx<=91 && my>=419 && my<=456){
                        pickMode=1;
                    }
                    if(textInput==0 && pickMode==0 && mx-radius>=oriX && mx+radius<=oriX+width && my-radius>=oriY && my+radius<=oriY+height){
                        isPainting=1;
                        paintR=rPos*255/200;
                        paintG=gPos*255/200;
                        paintB=bPos*255/200;
                        for(int i=my-oriY-radius;i<=my-oriY+radius;i++){
                            for(int j=mx-oriX-radius;j<=mx-oriX+radius;j++){
                                if((my-oriY-i)*(my-oriY-i)+(mx-oriX-j)*(mx-oriX-j)<=radius*radius){
                                    img[i][j][0]=paintR;
                                    img[i][j][1]=paintG;
                                    img[i][j][2]=paintB;
                                }
                            }
                        }
                    }
                    if(mx>=54 && mx<=91 && my>=513 && my<=550){
                        textInput=1;
                        strcpy(text,"");
                        fontList=0;
                        rPos=40;
                        gPos=40;
                        bPos=40;
                        textX=width/2;
                        textY=height/2;
                    }
                    if(textInput==0 && mx>=54 && mx<=91 && my>=463 && my<=503){
                        rPos=200;
                        gPos=200;
                        bPos=200;
                    }
                }
                if(textInput==1){
                    iRectangle(headerX+195,10+543+27,50,25);
                    if(mx>=headerX+195 && mx<=headerX+195+50 && my>=10+543+27 && my<=10+543+27+25)textInput=0;
                    if(mx>=headerX+140 && mx<=headerX+140+50 && my>=10+543+27 && my<=10+543+27+25){
                        saveText=1;
                        iDraw();
                        int fontHeight,fontWidth;
                        switch(font){
                            case 1:
                                fontHeight=13;
                                fontWidth=8;
                                break;
                            case 2:
                                fontHeight=15;
                                fontWidth=9;
                                break;
                            case 3:
                                fontHeight=10;
                                fontWidth=10;
                                break;
                            case 4:
                                fontHeight=24;
                                fontWidth=24;
                                break;
                            case 5:
                                fontHeight=10;
                                fontWidth=10;
                                break;
                            case 6:
                                fontHeight=12;
                                fontWidth=12;
                                break;
                            case 7:
                                fontHeight=18;
                                fontWidth=18;
                                break;
                        }
                        archiving();
                        for(int i=textY-10;i<=textY+fontHeight;i++){
                            for(int j=textX;j<=width;j++){
                                int fontRGB[3];
                                iGetPixelColor(j+oriX-1,i+oriY-1,fontRGB);
                                for(int k=0;k<3;k++)img[i][j][k]=fontRGB[k];
                            }
                        }
                        tmp();
                        textInput=0;
                        saveText=0;
                    }
                    if(mx>=headerX+35 && mx<=headerX+135 && my>=10+543+27 && my<=35+543+27){
                        if(fontList==1)fontList=0;
                        else fontList=1;
                    }
                    if(fontList==1){
                        if(mx>=491 && mx<=651 && my>=365+0*30 && my<365+1*30){
                            font=7;
                            fontList=0;
                        }
                        if(mx>=491 && mx<=651 && my>=365+1*30 && my<365+2*30){
                            font=6;
                            fontList=0;
                        }
                        if(mx>=491 && mx<=651 && my>=365+2*30 && my<365+3*30){
                            font=5;
                            fontList=0;
                        }
                        if(mx>=491 && mx<=651 && my>=365+3*30 && my<365+4*30){
                            font=4;
                            fontList=0;
                        }
                        if(mx>=491 && mx<=651 && my>=365+4*30 && my<365+5*30){
                            font=3;
                            fontList=0;
                        }
                        if(mx>=491 && mx<=651 && my>=365+5*30 && my<365+6*30){
                            font=2;
                            fontList=0;
                        }
                        if(mx>=491 && mx<=651 && my>=365+6*30 && my<365+7*30){
                            font=1;
                            fontList=0;
                        }
                    }
                    if(mx>=oriX+textX && mx<=oriX+textX+30 && my>=oriY+textY+25 && my<=oriY+textY+55){
                        textMoveX=mx;
                        textMoveY=my;
                        textMove=1;
                    }
                }
            }
            else{
                if(mx>=210 && mx<=360 && my>=235 && my<=275){
                    if(strlen(outputName)>0){
                        isOutput=0;
                        outputBMP();
                    }else{
                        MessageBox(0,"Enter a name to save the image","Name Required",0);
                    }
                }
                if(mx>360 && mx<=510 && my>=235 && my<=275){
                    isOutput=0;
                }
            }
        }else{
            if(startPageMode==0){
                if(mx>=220 && mx<=502 && my>=395 && my<=478){
                       startPageMode=4;
                }
                if(mx>=224 && mx<=499 && my>=310 && my<=393){
                    startPageMode=1;
                    tutorialPage=0;
                }
                if(mx>=249 && mx<=473 && my>=225 && my<=308){
                    startPageMode=2;
                }
                if(mx>=202 && mx<=521 && my>=140 && my<=223){
                    startPageMode=3;
                }
                if(mx>=282 && mx<=440 && my>=55 && my<=138){
                    exit(0);
                }
            }
            if(startPageMode==2 || startPageMode==3){
                if(mx>=340 && mx<=380 && my>=55 && my<=95) startPageMode=0;
            }
            if(startPageMode==4){
                if(mx>=150 && mx<=190 && my>=440 && my<=480) startPageMode=0;
                if(mx>=100 && mx<=464 && my>=165 && my<=265){
                    radius=16,s=0,knobPos=200,mode=1,selector=0,vertices=0,nPoints=0,rPos=150,gPos=150,bPos=50,tPos=100,pickMode=0;
                    mode=4;
                    s=0;
                    archiveLevel=1;
                    archiveMax=1;
                    height=596;
                    width=596;
                    textX=width/2;
                    textY=height/2;
                    realHeight=596;
                    realWidth=596;
                    for(int i=0;i<height;i++){
                        for(int j=0;j<width;j++){
                            for(int k=2;k>=0;k--){
                                imgOriginal[i][j][k]=255;
                                img[i][j][k]=imgOriginal[i][j][k];
                                archive[0][i][j][k]=imgOriginal[i][j][k];
                                imgTmp[i][j][k]=imgOriginal[i][j][k];
                            }
                        }
                    }
                    oriX=410-width/2;
                    oriY=310-height/2;
                    startX=width*0.25;
                    startY=height*0.25;
                    dx=width*0.5;
                    dy=height*0.5;
                    textX=width/2;
                    textY=height/2;
                    gStartX=0;
                    gStartY=0;
                    gDx=width;
                    gDy=height;
                    startPage=0;
                    PlaySound("assets/audio/click.wav",NULL,SND_ASYNC);
                }
                if(mx>=170 && mx<=552 && my>=285 && my<=385){
                    radius=16,s=0,knobPos=200,mode=1,selector=0,vertices=0,nPoints=0,rPos=150,gPos=150,bPos=50,tPos=100,pickMode=0;
                    mode=1;
                    s=0;
                    archiveLevel=1;
                    archiveMax=1;
                    AUX_RGBImageRec *TextureImage;
                    TextureImage = auxDIBImageLoad(loc);
                    int i,j,k;
                    int realwidth = TextureImage->sizeX;
                    int realheight = TextureImage->sizeY;
                    realwidth>596?width=596:width=realwidth;
                    realheight>596?height=596:height=realheight;
                    realHeight=height;
                    realWidth=width;
                    for(i=0;i<height;i++){
                        for(j=0;j<width;j++){
                            for(k=2;k>=0;k--){
                                imgOriginal[i][j][k]=TextureImage->data[3*i*realwidth+3*j+k];
                                img[i][j][k]=imgOriginal[i][j][k];
                                archive[0][i][j][k]=imgOriginal[i][j][k];
                                imgTmp[i][j][k]=imgOriginal[i][j][k];
                            }
                        }
                    }
                    free(TextureImage->data);
                    free(TextureImage);
                    oriX=410-width/2;
                    oriY=310-height/2;
                    startX=width*0.25;
                    startY=height*0.25;
                    dx=width*0.5;
                    dy=height*0.5;
                    gStartX=0;
                    gStartY=0;
                    gDx=width;
                    gDy=height;
                    textX=width/2;
                    textY=height/2;
                    startPage=0;
                    PlaySound("assets/audio/click.wav",NULL,SND_ASYNC);
                }
            }
            if(startPageMode==1){
                if(tutorialPage==15 && mx>=220 && mx<=502 && my>=230 && my<=313){
                    startPageMode=4;
                }
                if(mx>=100 && mx<=140 && my>=516 && my<=556) startPageMode=0;
                if(tutorialPage>0 && mx>=100 && mx<=140 && my>=110 && my<=150)tutorialPage--;
                if(tutorialPage<15 && mx>=580 && mx<=620 && my>=110 && my<=150)tutorialPage++;
            }
        }
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
        if(mode==4 && pickMode==0 && isPainting==1){
            archiving();
            tmp();
            isPainting=0;
        }
        if(mode==4 && textMove==1)textMove=0;
        if(pickMode==1 && mx>=oriX && mx<oriX+width && my>=oriY && my<oriY+height){
            pickMode=0;
            int rgb[3];
            iGetPixelColor(mx,my,rgb);
            rPos=rgb[0]*200/255;
            gPos=rgb[1]*200/255;
            bPos=rgb[2]*200/255;
        }
	}
}

//****************iGraphics function****************
void iKeyboard(unsigned char key)
{
	if(isOutput==1 && ((key>='A' && key <='Z') || (key>='a' && key<='z') || (key>='0' && key<='9') || key=='-' || key=='_'))
	{
	    if(strlen(outputName)<25){
	    char a[2];
	    a[0]=key;
	    a[1]='\0';
        strcat(outputName,a);
        strcpy(outputFullName,outputName);
        strcat(outputFullName,".bmp");
	    }
	    else{
             MessageBox(0,"Name can't exceed 25 characters, can't add more characters","Too Long Name",0);
	    }
	}
	if(textInput==1){
        char a[1];
	    a[0]=key;
	    a[1]='\0';
	    strcat(text,a);
	}
	if(key==' '){
        if(s!=11 && mode==3 && vertices>0)vertices--;
	}
}

//****************iGraphics function****************
void iSpecialKeyboard(unsigned char key)
{
    if(isOutput==1){
        if(key==GLUT_KEY_LEFT){
            if(strlen(outputName)>1){
            outputName[strlen(outputName)-1]='\0';
            strcpy(outputFullName,outputName);
            strcat(outputFullName,".bmp");
            }else{
                outputName[0]='\0';
                outputFullName[0]='\0';
            }
        }
    }
    if(textInput==1){
        if(key==GLUT_KEY_LEFT){
            if(strlen(text)>0){
            text[strlen(text)-1]='\0';
            }
        }
    }
}

//****************iHraphics Function****************
void iPassiveMouseMove(int mx, int my)
{
    if(startPage==1){
        if(startPageMode==0){
            if(mx>=220 && mx<=502 && my>=395 && my<=478){
                hoverState=1;
            }
            else if(mx>=224 && mx<=499 && my>=310 && my<=393){
                hoverState=2;
            }
            else if(mx>=249 && mx<=473 && my>=225 && my<=308){
                hoverState=3;
            }
            else if(mx>=202 && mx<=521 && my>=140 && my<=223){
                hoverState=4;
            }
            else if(mx>=282 && mx<=440 && my>=55 && my<=138){
                hoverState=5;
            }
            else{
                hoverState=0;
            }
        }else if(startPageMode==1){
            if(tutorialPage==15 && mx>=220 && mx<=502 && my>=230 && my<=313)hoverState=6;
            else if(mx>=100 && mx<=140 && my>=516 && my<=556)hoverState=7;
            else if(tutorialPage>0 && mx>=100 && mx<=140 && my>=110 && my<=150)hoverState=8;
            else if(tutorialPage<15 && mx>=580 && mx<=620 && my>=110 && my<=150)hoverState=9;
            else hoverState=0;
        }else if(startPageMode==2 || startPageMode==3){
            if(mx>=340 && mx<=380 && my>=55 && my<=95)hoverState=10;
            else hoverState=0;
        }else if(startPageMode==4){
            if(mx>=150 && mx<=190 && my>=440 && my<=480) hoverState=11;
            else if(mx>=100 && mx<=464 && my>=165 && my<=265)hoverState=12;
            else if(mx>=170 && mx<=552 && my>=285 && my<=385)hoverState=13;
            else hoverState=0;
        }
    }else if(isOutput==0 && intro==0 && importMode==0){
        if(textInput==0 && mx>=10 && mx<=50 && my>=620 && my<=660){
            strcpy(tooltip,"Undo");
            tipX=mx+15;
            tipY=my-40;
            tipWidth=55;
        }
        else if(textInput==0 && mx>=60 && mx<=100 && my>=620 && my<=660){
            strcpy(tooltip,"Redo");
            tipX=mx+15;
            tipY=my-40;
            tipWidth=55;
        }
        else if(textInput==0){
            if(mx>=10 && mx<=50 && my>=10 && my<=50){
                strcpy(tooltip,"Full Editing Mode");
                tipX=mx+15;
                tipY=my+10;
                tipWidth=150;
            }
            else if(mx>=60 && mx<=100 && my>=10 && my<=50){
                strcpy(tooltip,"Crop Editing Mode");
                tipX=mx+15;
                tipY=my+10;
                tipWidth=165;
            }
            else if(mx>=10 && mx<=50 && my>=60 && my<=100){
                strcpy(tooltip,"Polygon Editing Mode");
                tipX=mx+15;
                tipY=my+10;
                tipWidth=195;
            }
            else if(mx>=60 && mx<=100 && my>=60 && my<=100){
                strcpy(tooltip,"Paint Mode");
                tipX=mx+15;
                tipY=my+10;
                tipWidth=110;
            }
            else if(s==0 && mx>=670 && mx<=710 && my>=620 && my<=660){
                strcpy(tooltip,"Close");
                tipX=mx-45;
                tipY=my-40;
                tipWidth=70;
            }
            else if(s==0 && mx>=620 && mx<=660 && my>=620 && my<=660){
                strcpy(tooltip,"Restore");
                tipX=mx-45;
                tipY=my-40;
                tipWidth=75;
            }
            else if(s==0 && mx>=570 && mx<=610 && my>=620 && my<=660){
                strcpy(tooltip,"Save");
                tipX=mx-45;
                tipY=my-40;
                tipWidth=55;
            }
            else if(s==0 && mx>=520 && mx<=560 && my>=620 && my<=660){
                strcpy(tooltip,"Import");
                tipX=mx-45;
                tipY=my-40;
                tipWidth=65;
            }
            else if((s==17 || s==18) && mx>=670 && mx<=710 && my>=620 && my<=660){
                strcpy(tooltip,"Close");
                tipX=mx-45;
                tipY=my-40;
                tipWidth=70;
            }
            else if((s==17 || s==18) && mx>=620 && mx<=660 && my>=620 && my<=660){
                strcpy(tooltip,"Restore");
                tipX=mx-45;
                tipY=my-40;
                tipWidth=75;
            }
            else if(mode!=4 && mx>=10 && mx<=50 && my>=490 && my<=530){
                strcpy(tooltip,"Brightness");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=100;
            }
            else if(mode!=4 && mx>=60 && mx<=100 && my>=490 && my<=530){
                strcpy(tooltip,"Contrast");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=80;
            }
            else if(mode!=4 && mx>=10 && mx<=50 && my>=440 && my<=480){
                strcpy(tooltip,"Red-ness");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=90;
            }
            else if(mode!=4 && mx>=60 && mx<=100 && my>=440 && my<=480){
                strcpy(tooltip,"Green-ness");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=110;
            }
            else if(mode!=4 && mx>=10 && mx<=50 && my>=390 && my<=430){
                strcpy(tooltip,"Blue-ness");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=100;
            }
            else if(mode!=4 && mx>=60 && mx<=100 && my>=390 && my<=430){
                strcpy(tooltip,"B/W Filter");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=90;
            }
            else if(mode!=4 && mx>=10 && mx<=50 && my>=340 && my<=380){
                strcpy(tooltip,"Crop");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=55;
            }
            else if(mode!=4 && mx>=60 && mx<=100 && my>=340 && my<=380){
                strcpy(tooltip,"Negative");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=85;
            }
            else if(mode!=4 && mx>=10 && mx<=50 && my>=290 && my<=330){
                strcpy(tooltip,"Y-Mirror");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=80;
            }
            else if(mode!=4 && mx>=60 && mx<=100 && my>=290 && my<=330){
                strcpy(tooltip,"X-Mirror");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=80;
            }
            else if(mode!=4 && mx>=10 && mx<=50 && my>=240 && my<=280){
                strcpy(tooltip,"Blur");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=45;
            }
            else if(mode!=4 && mx>=60 && mx<=100 && my>=240 && my<=280){
                strcpy(tooltip,"Pixilette");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=75;
            }
            else if(mode!=4 && mx>=10 && mx<=50 && my>=190 && my<=230){
                strcpy(tooltip,"CCW Rotation");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=130;
            }
            else if(mode!=4 && mx>=60 && mx<=100 && my>=190 && my<=230){
                strcpy(tooltip,"CW Rotation");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=120;
            }
            else if(mode!=4 && mx>=10 && mx<=50 && my>=140 && my<=180){
                strcpy(tooltip,"RGB-Swap");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=105;
            }
            else if(mode!=4 && mx>=60 && mx<=100 && my>=140 && my<=180){
                strcpy(tooltip,"Sketch");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=70;
            }
            else if(mode==4 && mx>=36 && mx<=72 && my>=570 && my<=606){
                strcpy(tooltip,"Change Theme");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=135;
            }
            else if(mode!=4 && mx>=36 && mx<=72 && my>=556 && my<=592){
                strcpy(tooltip,"Change Theme");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=135;
            }
            else if(mode!=4 && (s==0 || s==17 || s==18) && mx>=160 && mx<=200 && my>=620 && my<=660){
                strcpy(tooltip,"Copy");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=55;
            }
            else if(mode !=4 && s==0 && mx>=260 && mx<=300 && my>=620 && my<=660){
                strcpy(tooltip,"Encryption-1");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=120;
            }
            else if(mode !=4 && s==0 && mx>=310 && mx<=350 && my>=620 && my<=660){
                strcpy(tooltip,"Encryption-2");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=120;
            }
            else if(mode !=4 && s==0 && mx>=360 && mx<=400 && my>=620 && my<=660){
                strcpy(tooltip,"Decryption-1");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=120;
            }
            else if(mode !=4 && s==0 && mx>=410 && mx<=450 && my>=620 && my<=660){
                strcpy(tooltip,"Decryption-2");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=120;
            }
            else if(mode==4 && mx>=54 && mx<=91 && my>=419 && my<=456){
                strcpy(tooltip,"Color Picker");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=115;
            }
            else if(mode==4 && textInput==0 && mx>=54 && mx<=91 && my>=463 && my<=503){
                strcpy(tooltip,"Eraser");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=60;
            }
            else if(mode==4 && mx>=54 && mx<=91 && my>=513 && my<=550){
                strcpy(tooltip,"Add Text");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=90;
            }
            else if(mode==4 && mx>=110 && mx<=514 && my>=625 && my<=655){
                strcpy(tooltip,"Choose Color");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=125;
            }
            else{
                strcpy(tooltip,"");
                tipWidth=0;
            }
        }
        else if(textInput==1){
            if(mx>=36 && mx<=72 && my>=570 && my<=606){
                strcpy(tooltip,"Change Theme");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=135;
            }
            else if(mx>=54 && mx<=91 && my>=419 && my<=456){
                strcpy(tooltip,"Color Picker");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=115;
            }
            else if(mx>=110 && mx<=514 && my>=625 && my<=655){
                strcpy(tooltip,"Choose Color");
                tipX=mx+15;
                tipY=my-40;
                tipWidth=125;
            }
            else{
                strcpy(tooltip,"");
                tipWidth=0;
            }
        }
        else{
            strcpy(tooltip,"");
            tipWidth=0;
        }
        if(pickMode==1){
            if(mx>=oriX && mx<oriX+width && my>=oriY && my<oriY+height){
                if(mx<=533)colorX=mx+5;
                else colorX=533;
                colorY=my+5;
                mX=mx;
                mY=my;
            }else{
                colorX=0;
                colorY=0;
            }
        }
    }
}

//****************Cursor timer****************
void cursor(){
    showCursor*=-1;
}

//****************Background timer****************
void bg(){
    if(isOutput==1 || startPage==1){
        bgColor+=bgSign*5;
        if(bgColor<0 || bgColor>230)bgSign*=-1;
    }
}
//****************Main Function****************
int main(){
    iSetTimer(500,cursor);
    iSetTimer(4,bg);
	iInitialize(720,670, "iPhotoshop");
	return 0;
}

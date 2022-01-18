#include <iostream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <map>
#include "graphics.h"

using namespace std;

class Graph {
   int x,y,z;//êîîðäèíàòû îáúåêòà
   bool visibility;//êðèòåðèé âèäèìîñòè
   string name;//èìÿ îáúåêòà
   int ignoreColor;//èãíîðèðóåìûé(ôîíîâûé) öâåò
   int width, height;//øèðèíà è âûñîòà îáúåêòà
   IMAGE *mask;//ìàñêà
   IMAGE *picture;//èñõîäíàÿ êàðòèíêà
   IMAGE *changed_image;//èçìåíåííàÿ êàðòèíêà(äëÿ èñïîëüçîâàíèÿ)
private:
   draw();//îòðèñîâêà îáúåêòà
   IMAGE *create_mask();
public:
   Graph(string name, int, int); //êîíñòðóêòîð
   ~Graph();//äåñòðóêòîð
   void resize(int, int);//èçìåíåíèå ðàçìåðà êàðòèíêè
   int getx() const {return x;} //ïîëó÷èòü êîîðäèíàòó x
   int gety() const {return y;} //ïîëó÷èòü êîîðäèíàòó y
   void setxy(int, int);//óñòàíîâèòü x è y
   int getz() const {return z;} //ïîëó÷èòü z
   void setz(int);//óñòàíîâèòü z
    bool friend operator==(Graph& a, Graph& b){
      return a.x==b.x && a.y==b.y && a.name==b.name;
      }
   int getvisible() const {return beer;} //ïîëó÷èòü çíà÷åíèå beer
   void setvisible(bool);//óñòàíîâèòü çíà÷åíèå beer
   string getfilename() const;//ïîëó÷èòü èìÿ ôàéëà
   void setfilename(string name);//óñòàíîâèòü èìÿ ôàéëà
   int get_width() const {return width;} //óçíàòü øèðèíó è âûñîòó èçîáðàæåíèÿ
   int get_height() const {return height;} //óçíàòü øèðèíó è âûñîòó èçîáðàæåíèÿ
   int get_ignoreColor() {return ignoreColor;} //èãíîðèðóåìûé öâåò
   void set_ignoreColor(int);//èçìåíèòü èãíîðèðóåìûé öâåò
   friend class Screen;
};


class Screen {
   multimap<int, Graph *> object;
   int p=0;
   int screen_width;
   int screen_height;
   int color;
public:
   Screen();
   ~Screen();
   void add(Graph *);
   void erase(Graph *);
   Graph& get(int);
   int find(Graph);
   int getwidth() const {return screen_width;}
   int getheight() const {return screen_height;}
   int change_color(int);
   void refresh();
   friend class Graph;
};

Screen::Screen() {
   screen_width=1000;
   screen_height=600;
   color=10;
   initwindow(screen_width, screen_height);
   refresh();
}

Screen::~Screen() {

} 

void Screen::add(Graph * a){cout<<11<<endl;
      object.insert(make_pair(a->z, a));
   
      }
      
      
void Screen::erase(Graph * a){
         for (auto at=object.begin();at!=object.end();at++){
      if(at->second==a){
            object.erase(at);
         }
   }   
}
Graph& Screen::get(int I){
   auto at=object.begin();
   advance(at,I);
   return *at->second;
   }

int Screen::find(Graph a){
   int i=0;
   for (auto at : object){
      if(*at.second==a)
            return i;
      i++;
   }   
   return -1;
   }

void Screen::refresh() {
   p=1-p;
   setactivepage(p); // àêòèâíàÿ ñòðàíèöà == 1-âèäèìàÿ
   setbkcolor(BLACK); // öâåò ôîíà
   clearviewport(); // çàêðàñêà òåêóùåé ñòðàíèöû öâåòîì ôîíà
   for (auto at:object) {
      putimage(at.second->x,at.second->y,at.second->mask,AND_PUT);
      putimage(at.second->x,at.second->y,at.second->changed_image,OR_PUT);
   }
    setvisualpage(p); // äåëàåì àêòèâíóþ ñòðàíèöó âèäèìîé
}

Screen back;

IMAGE *Graph::create_mask() {
   int w=imagewidth(changed_image);
   int h=imageheight(changed_image);
   mask=createimage(w,h);
   int c=ignoreColor;
   for (int x=0; x<w; ++x)
      for (int y=0; y<h; ++y)
      {  int d=imagegetpixel(changed_image,x,y);
         if (c==d)
         {  imageputpixel(mask,x,y,WHITE);
            imageputpixel(changed_image,x,y,BLACK);
         }
         else
            imageputpixel(mask,x,y,BLACK);
      }
   return mask;
}

void Graph::resize(int w, int h) {
   width=w;
   height=h;
   int wp=imagewidth(changed_image);
   int hp=imageheight(changed_image);
   IMAGE *r=createimage(w,h);
   for (int x=0; x<w; ++x)
      for (int y=0; y<h; ++y)
      {
         imageputpixel(r,x,y,imagegetpixel(changed_image,x*wp/w,y*hp/h));
      }
   changed_image=r;
   mask=create_mask();
   freeimage(r);
   back.refresh();
}

Graph::Graph(string name, int x, int y) {
   setfillstyle(SOLID_FILL,BROWN);
   this->name = name;
   picture=loadBMP(name.c_str());
   changed_image=picture;
   mask=create_mask();
   width=100;
   height=100;
   this->x = x;
   this->y = y;
   x=back.getwidth()/2;
   y=back.getheight()/2;
   bar(x-width/2,y-height/2, x+width,x-height);
   visibility=1;
   z=back.object.size();
   ignoreColor=34;
}

Graph::~Graph() {
   freeimage(picture);
   freeimage(mask);
   freeimage(changed_image);
}

void Graph::setxy(int x_buf, int y_buf) {
   x=x_buf;
   y=y_buf;
   back.refresh();
}

void Graph::setz(int z_buf) {
   z=z_buf;
}

void Graph::setvisible(bool visible_buf) {
   visibility=visible_buf;
}

void Graph::setfilename(string name_buf) {
   name=name_buf;
}

void Graph::set_ignoreColor(int color) {
   ignoreColor=color;
}

int main()
{
   
   int t=0; // íîìåð êàäðà (âðåìÿ)
   cout<<9;
   back.add( new Graph("background.bmp",0, 0));
   back.add(new Graph("star1.bmp", 140, 255));
   back.get(back.find(Graph("star1.bmp", 140, 255))).resize(200,300);
   back.get(back.find(Graph("star1.bmp", 140, 255))).setxy(10,10);

   while (1)
   {
      ++t;
      back.refresh();
      delay(180); // çàäåðæêà íà 1/60 ñåêóíäû
      if (kbhit()) break; // åñëè íàæàòà êëàâèøà - çàâåðøèòü ðàáîòó
   }
   closegraph();
}
